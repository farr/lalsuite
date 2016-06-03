/*
 *  Copyright (C) 2014 Michael Puerrer, John Veitch
 *  Reduced Order Model for SEOBNR
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with with program; see the file COPYING. If not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 */

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <stdbool.h>
#include <alloca.h>
#include <string.h>
#include <libgen.h>


#include <gsl/gsl_errno.h>
#include <gsl/gsl_chebyshev.h>

// #include <gsl/gsl_blas.h>
// #include <gsl/gsl_min.h>
#include <lal/Units.h>
#include <lal/SeqFactories.h>
#include <lal/LALConstants.h>
#include <lal/XLALError.h>
#include <lal/FrequencySeries.h>
#include <lal/Date.h>
#include <lal/StringInput.h>
#include <lal/Sequence.h>
#include <lal/LALStdio.h>
#include <lal/FileIO.h>

#include <lal/LALSimInspiral.h>
#include <lal/LALSimIMR.h>

//gsl_bspline needs to be imported here, because this was not done in ROMUtilities!
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_spline.h>
//needed for read_vector
#include "LALSimIMRSEOBNRROMUtilities.c"

#include <lal/LALConfig.h>
#ifdef LAL_PTHREAD_LOCK
#include <pthread.h>
#endif


/******************************************************************
 * The double-spin SEOBNRv2 ROM consists of a number of submodels *
 * for subdomains of the whole parameter space:                   *
 * These submodels may change in the future.                      *
 *                                                                *
 * "Core" submodel 1                                              *
 * B-spline points: 54x24x24                                      *
 * Frequency points: {133, 139}                                   *
 *                                                                *
 * "Near q=1" submodel 2                                          *
 * B-spline points: 11x60x60                                      *
 * Frequency points: {200, 187}                                   *
 *                                                                *
 * "High q, high chi1" submodel 3                                 *
 * B-spline points: 24x30x24                                      *
 * frequency points: {133, 139}                                   *
 *                                                                *
 *****************************************************************/


/********* Input data for spline basis points **************/

//Prepending G to recognise it as a global variable
#define Gntimes 69072 //(will be reduced in the future)
#define Gnamp 21
#define Gnphase 7
#define Gnq 16
#define Gnlambda1 16
#define Gnlambda2 16

#ifdef LAL_PTHREAD_LOCK
static pthread_once_t EOB_ROM_EOS_is_initialized = PTHREAD_ONCE_INIT;
#endif

static const double Gparams_min[] = {0.5,50.,50.}; //qmin,lambda1min,lambda2min
static const double Gparams_max[] = {1.0,5000.,5000.}; //qmax,lambda1max,lambda2max

/*************** type definitions ******************/

typedef struct tagEOB_ROM_EOSdataDS_coeff
{
  gsl_vector* c_amp;
  gsl_vector* c_phi;
} EOB_ROM_EOSdataDS_coeff;

struct tagEOB_ROM_EOSdataDS_submodel
{
  gsl_vector* cvec_amp;      // amplitude projection coefficients
  gsl_vector* cvec_phi;      // phase projection coefficients
  gsl_matrix *Bamp;          // Reduced SVD basis for amplitude
  gsl_matrix *Bphi;          // Reduced SVD basis for phase
  gsl_vector* times;  // AMplitude prefactor coefficient
  const double *params_min;
  const double *params_max;
  int n_amp;                 // Number frequency points for amplitude
  int n_phi;                 // Number of frequency points for phase
  int nq, nl1, nl2, ntimes;         // Number of points in eta, chi1, chi2
};
typedef struct tagEOB_ROM_EOSdataDS_submodel EOB_ROM_EOSdataDS_submodel;

struct tagEOB_ROM_EOSdataDS
{
  UINT4 setup;
  EOB_ROM_EOSdataDS_submodel* sub1;
  EOB_ROM_EOSdataDS_submodel* sub2;
  EOB_ROM_EOSdataDS_submodel* sub3;
};
typedef struct tagEOB_ROM_EOSdataDS EOB_ROM_EOSdataDS;

static EOB_ROM_EOSdataDS __lalsim_EOB_ROM_EOSDS_data;

typedef int (*load_dataPtr)(const char*, gsl_vector *, gsl_vector *, gsl_matrix *, gsl_matrix *, gsl_vector *);

// typedef struct tagChebyshevData
// {
//   gsl_cheb_series *csx ;
//   gsl_cheb_series *csy ;
//   gsl_cheb_series *csz ;
// } ChebyshevData;

/**************** Internal functions **********************/

static void EOB_ROM_EOS_Init_LALDATA(void);
static int EOB_ROM_EOS_Init(const char dir[]);
static bool EOB_ROM_EOS_IsSetup(void);

static int EOB_ROM_EOSdataDS_Init(EOB_ROM_EOSdataDS *romdata, const char dir[]);
static void EOB_ROM_EOSdataDS_Cleanup(EOB_ROM_EOSdataDS *romdata);

static int EOB_ROM_EOSdataDS_Init_submodel(
  EOB_ROM_EOSdataDS_submodel **submodel,
  const int n_amp,
  const int n_phi,
  const int nq,
  const int nl1,
  const int nl2,
  const int ntimes,
  const double *params_min,
  const double *params_max,
  const char dir[],
  load_dataPtr load_data
);


// static void ChebyshevData_Init(
//   ChebyshevData **chebydata);
//
// static void ChebyshevData_Destroy(
//   ChebyshevData *chebydata);

static double gsl_cheb_evaluate_polynomial(int n, double x);
static double gsl_cheb_eval_3d(gsl_vector *c_ijk, int nx, int ny, int nz, double x, double y, double z);
static int chebyshev_interpolation3d(
  double q,
  double lambda1,
  double lambda2,
  int nx, int ny, int nz,
  gsl_vector *cvec_amp,
  gsl_vector *cvec_phi,
  //gsl_vector *times,
  int nk_amp,
  int nk_phi,
  gsl_vector *interp_amp,
  gsl_vector *interp_phi);

static void EOB_ROM_EOSdataDS_Cleanup_submodel(EOB_ROM_EOSdataDS_submodel *submodel);

static int EOBROMEOSCore_datatest(
  REAL8TimeSeries **hPlus,
  REAL8TimeSeries **hCross,
  double phiRef,
  double deltaT,
  double fRef,
  double distance,
  double inclination,
  double Mtot_sec,
  double eta,
  double lambda1,
  double lambda2
);

// static void SEOBNRROMdataDS_coeff_Init(SEOBNRROMdataDS_coeff **romdatacoeff, int nk_amp, int nk_phi);
// static void SEOBNRROMdataDS_coeff_Cleanup(SEOBNRROMdataDS_coeff *romdatacoeff);

// static size_t NextPow2(const size_t n);

static int load_data_romeos(const char dir[], gsl_vector *cvec_amp, gsl_vector *cvec_phi, gsl_matrix *Bamp, gsl_matrix *Bphi, gsl_vector *times);

/********************* Definitions begin here ********************/

/** Setup SEOBNRv2ROMDoubleSpin model using data files installed in dir
 */
static int EOB_ROM_EOS_Init(const char dir[]) {
  if(__lalsim_EOB_ROM_EOSDS_data.setup) {
    XLALPrintError("Error: DSEOB_ROM_EOSdata was already set up!");
    XLAL_ERROR(XLAL_EFAILED);
  }

  EOB_ROM_EOSdataDS_Init(&__lalsim_EOB_ROM_EOSDS_data, dir);

  if(__lalsim_EOB_ROM_EOSDS_data.setup) {
    return(XLAL_SUCCESS);
  }
  else {
    return(XLAL_EFAILED);
  }
}

/** Helper function to check if the SEOBNRv2ROMDoubleSpin model has been initialised */
static bool EOB_ROM_EOS_IsSetup(void) {
  if(__lalsim_EOB_ROM_EOSDS_data.setup)
    return true;
  else
    return false;
}

// Read binary ROM data for basis functions and coefficients for submodel 1
static int load_data_romeos(const char dir[], gsl_vector *cvec_amp, gsl_vector *cvec_phi, gsl_matrix *Bamp, gsl_matrix *Bphi, gsl_vector *times) {
  // Load binary data for amplitude and phase spline coefficients and reduced bases as computed in Mathematica
  // "Core" submodel 1
  // B-spline points: 54x24x24
  // Frequency points: {133, 139}
  int ret = XLAL_SUCCESS;
  ret |= read_vector(dir, "EOB_ROM_EOS_Amp_ciall.dat", cvec_amp);
  ret |= read_vector(dir, "EOB_ROM_EOS_Phase_ciall.dat", cvec_phi);
  ret |= read_matrix(dir, "EOB_ROM_EOS_Bamp_matrix.dat", Bamp);
  ret |= read_matrix(dir, "EOB_ROM_EOS_Bphase_matrix.dat", Bphi);
  ret |= read_vector(dir, "EOB_ROM_EOS_times.dat", times);
  return(ret);
}

/* Set up a new ROM submodel, using data contained in dir */
static int EOB_ROM_EOSdataDS_Init_submodel(
  EOB_ROM_EOSdataDS_submodel **submodel,
  const int n_amp,
  const int n_phi,
  const int nq,
  const int nl1,
  const int nl2,
  const int ntimes,
  const double *params_min,
  const double *params_max,
  const char dir[],
  load_dataPtr load_data
) {
  int ret = XLAL_FAILURE;

  if(!submodel) exit(1);
  /* Create storage for submodel structures */
  if (!*submodel)
    *submodel = XLALCalloc(1,sizeof(EOB_ROM_EOSdataDS_submodel));
  else
    EOB_ROM_EOSdataDS_Cleanup_submodel(*submodel);

  int N = nq*nl1*nl2; // Total number of points over parameter space = size of the data matrix for one SVD-mode

  // Initalize actual ROM data
  (*submodel)->cvec_amp = gsl_vector_alloc(N*n_amp);
  (*submodel)->cvec_phi = gsl_vector_alloc(N*n_phi);
  (*submodel)->Bamp = gsl_matrix_alloc(n_amp, ntimes);
  (*submodel)->Bphi = gsl_matrix_alloc(n_phi, ntimes);
  (*submodel)->times = gsl_vector_alloc(ntimes);

  // Load ROM data for this submodel
  ret=load_data(dir, (*submodel)->cvec_amp, (*submodel)->cvec_phi, (*submodel)->Bamp, (*submodel)->Bphi, (*submodel)->times);

  // Initialize other members
  (*submodel)->n_amp = n_amp;
  (*submodel)->n_phi = n_phi;
  (*submodel)->nq = nq;
  (*submodel)->nl1 = nl1;
  (*submodel)->nl2 = nl2;
  (*submodel)->ntimes = ntimes;

  (*submodel)->params_min = params_min;
  (*submodel)->params_max = params_max;

  return ret;
}

/* Deallocate contents of the given EOB_ROM_EOSdataDS_submodel structure */
static void EOB_ROM_EOSdataDS_Cleanup_submodel(EOB_ROM_EOSdataDS_submodel *submodel) {
  if(submodel->cvec_amp) gsl_vector_free(submodel->cvec_amp);
  if(submodel->cvec_phi) gsl_vector_free(submodel->cvec_phi);
  if(submodel->Bamp) gsl_matrix_free(submodel->Bamp);
  if(submodel->Bphi) gsl_matrix_free(submodel->Bphi);
  if(submodel->times) gsl_vector_free(submodel->times);
}

/* Set up a new ROM model, using data contained in dir */
int EOB_ROM_EOSdataDS_Init(EOB_ROM_EOSdataDS *romdata, const char dir[]) {
  int ret = XLAL_FAILURE;

  /* Create storage for structures */
  if(romdata->setup) {
    XLALPrintError("WARNING: You tried to setup the EOB_ROM_EOS model that was already initialised. Ignoring\n");
    return (XLAL_FAILURE);
  }

  gsl_set_error_handler(&err_handler);

  load_dataPtr load_data = &load_data_romeos;
  ret = EOB_ROM_EOSdataDS_Init_submodel(&(romdata)->sub1, Gnamp, Gnphase, Gnq, Gnlambda1, Gnlambda2, Gntimes, Gparams_min, Gparams_max, dir, load_data);
  if (ret==XLAL_SUCCESS) XLALPrintInfo("%s : submodel 1 loaded sucessfully.\n", __func__);

  if(XLAL_SUCCESS==ret)
    romdata->setup=1;
  else
    EOB_ROM_EOSdataDS_Cleanup(romdata);

  return (ret);
}

/* Deallocate contents of the given EOB_ROM_EOSdataDS structure */
static void EOB_ROM_EOSdataDS_Cleanup(EOB_ROM_EOSdataDS *romdata) {
  EOB_ROM_EOSdataDS_Cleanup_submodel((romdata)->sub1);
  XLALFree((romdata)->sub1);
  (romdata)->sub1 = NULL;
  romdata->setup=0;
}

/* Structure for internal use */
// static void EOB_ROM_EOSdataDS_coeff_Init(EOB_ROM_EOSdataDS_coeff **romdatacoeff, int nk_amp, int nk_phi) {
//
//   if(!romdatacoeff) exit(1);
//   /* Create storage for structures */
//   if(!*romdatacoeff)
//     *romdatacoeff=XLALCalloc(1,sizeof(EOB_ROM_EOSdataDS_coeff));
//   else
//     EOB_ROM_EOSdataDS_coeff_Cleanup(*romdatacoeff);
//
//   (*romdatacoeff)->c_amp = gsl_vector_alloc(nk_amp);
//   (*romdatacoeff)->c_phi = gsl_vector_alloc(nk_phi);
// }
//
// /* Deallocate contents of the given EOB_ROM_EOSdataDS_coeff structure */
// static void EOB_ROM_EOSdataDS_coeff_Cleanup(EOB_ROM_EOSdataDS_coeff *romdatacoeff) {
//   if(romdatacoeff->c_amp) gsl_vector_free(romdatacoeff->c_amp);
//   if(romdatacoeff->c_phi) gsl_vector_free(romdatacoeff->c_phi);
//   XLALFree(romdatacoeff);
// }

/* Return the closest higher power of 2  */
// Note: NextPow(2^k) = 2^k for integer values k.
// static size_t NextPow2(const size_t n) {
//   return 1 << (size_t) ceil(log2(n));
// }

// static void ChebyshevData_Init(
//   ChebyshevData **chebydata
// )
// {
//   if(!chebydata) exit(1);
//   if(*chebydata) ChebyshevData_Destroy(*chebydata);
//
//   gsl_cheb_series *csx = gsl_cheb_alloc(3);
//   gsl_cheb_series *csy = gsl_cheb_alloc(3);
//   gsl_cheb_series *csz = gsl_cheb_alloc(3);
//
//   (*chebydata)=XLALCalloc(1,sizeof(ChebyshevData));
// }
//
// static void ChebyshevData_Destroy(ChebyshevData *chebydata)
// {
//   if(!chebydata) return;
//   if(chebydata->csx) gsl_bspline_free(chebydata->csx);
//   if(chebydata->csy) gsl_bspline_free(chebydata->csy);
//   if(chebydata->csz) gsl_bspline_free(chebydata->csz);
//   XLALFree(chebydata);
// }



/* To evaluate T_n(x) this function will call gsl_cheb_eval(csx,x)
 * gsl_cheb_eval(csx,x) <=> f(x) = (c_0 / 2) + \sum_{n=1} c_n T_n(x)
 *   Define f^N(x) := gsl_cheb_eval(csxN,x), where csxN is a list of ones of length n
 * We can then calculate any T_n(x) as:
 *   T_n(x) = f^n(x) - f^{n-1}(x)
 */
static double gsl_cheb_evaluate_polynomial(int n, double x){

  double Tnx = 0.0 ;
  double a=0.0, b=0.0 ;
  //T_0(x)
  if (n==0){
    Tnx = 1.0 ;
  }
  //T_1(x)
  else if (n==1){
    Tnx = x ;
  }
  //T_2(x)
  else if (n==2){
    Tnx = 2.0*x*x - 1.0 ;
  }
  //T_n(x)
  else {
    gsl_cheb_series *csx1 = gsl_cheb_alloc(n);
    gsl_cheb_series *csx2 = gsl_cheb_alloc(n-1);
    //testing
    csx1->a = -1.0 ;
    csx1->b = 1.0 ;
    csx2->a = -1.0 ;
    csx2->b = 1.0 ;
    int i=0;
    for (i = 0; i < n; i++){
      csx1->c[i]=1.0;
      csx2->c[i]=1.0;
    }
    csx1->c[n]=1.0;

    //f^n
    a = gsl_cheb_eval(csx1, x);
    //f^{n-1}
    b = gsl_cheb_eval(csx2, x);
    //fprintf(stdout,"     a=%.5e, b=%.5e\n",a,b);
    Tnx = a-b;
    gsl_cheb_free(csx1);
    gsl_cheb_free(csx2);
  }

  return Tnx ;

}

/* Wrapper function to evaluate 3d chebyshev polynomial.
 * p(x,y,z) = \sum_{i,j,k} c_{ijk} T_i(x) T_j(y) T_k(z)
 */
static double gsl_cheb_eval_3d(gsl_vector *c_ijk, int nx, int ny, int nz, double x, double y, double z){

  double sum=0.0;
  int i,j,k;
  int index=0;
  double Tix=0.,Tjy=0.,Tkz=0.;

  for (i=0;i<nx;i++){
    Tix=gsl_cheb_evaluate_polynomial(i,x);
//     fprintf(stdout,"T%dx = %.5e\n",i,Tix);
    for (j=0;j<ny;j++){
      Tjy=gsl_cheb_evaluate_polynomial(j,y);
//       fprintf(stdout,"  T%dy = %.5e\n",j,Tjy);
      for (k=0;k<nz;k++){
        Tkz=gsl_cheb_evaluate_polynomial(k,z);
//         fprintf(stdout,"    T%dz = %.5e\n",k,Tkz);
        sum+=gsl_vector_get(c_ijk,index)*Tix*Tjy*Tkz;
//         fprintf(stdout,"    cijk = %.5e\n",gsl_vector_get(c_ijk,index));
        index+=1;
      }
    }
  }

  return sum ;

}

/*
 * This function calculates the phase and amplitude interpolants at a time Tj.
 *  pj(q,l1,l2) = sum_l sum_m sum_n b_lmn Tl(q) Tm(l1) Tn(l2)
 * GSL can only evaluate a 1D chebyshev polynomial with gsl_cheb_eval_n, which calculates
 *  f(x) = (c_0 / 2) + \sum_{n=1}^(N_coeffs) c_n T_n(x)
 * This means that p(x,y,z) will have to be evaluated as
 *  p(x,y,z) = f'(x,csx,nx) f'(y,csy,ny) f'(z,csz,nz)
 * where csx are the chebyshev coefficients for parameter x , nx its size and f' is defined as
 *  f'(x;csx,nx) = f(x,csx,nx) + csx[0]/2.0
 */
/* NOTE: when we calculate p(x,y,z) = \sum_{i,j,k} c_{ijk} T_i(q) T_j(l1) T_k(l2)
 *       (done here with gsl_cheb_eval_3d for each k in nk_amp and nk_phi),
 *       we get an array for amp and phi with length nk_amp and nk_phi resp.
 *       All this is just for a single Tj. What do we do with these?
 */
static int chebyshev_interpolation3d(
  double q,
  double lambda1,
  double lambda2,
  int nx, int ny, int nz,
  gsl_vector *cvec_amp,
  gsl_vector *cvec_phi,
  //gsl_vector *times,
  int nk_amp,
  int nk_phi,
  gsl_vector *interp_amp,   //return: A(T_j;q,lambda1,lambda2)    <=> p_j(q,lambda1,lambda2)
  gsl_vector *interp_phi)   //return: \Phi(T_j;q,lambda1,lambda2) <=> p_j(q,lambda1,lambda2)
{

  double sum = 0.0;
  int k = 0;
  int N=nx*ny*nz ;

  //TODO: ranges should be given as function argument
  //Rescale so that x,y and z are always between -1 and 1
  double xrescale = (q-0.5*(Gparams_max[0]+Gparams_min[0])) / (0.5*(Gparams_max[0]-Gparams_min[0]));
  double yrescale = (lambda1-0.5*(Gparams_max[1]+Gparams_min[1])) / (0.5*(Gparams_max[1]-Gparams_min[1]));
  double zrescale = (lambda2-0.5*(Gparams_max[2]+Gparams_min[2])) / (0.5*(Gparams_max[2]-Gparams_min[2]));

  fprintf(stdout,"%.5e %.5e %.5e\n",q,lambda1,lambda2);
  fprintf(stdout,"%.5e %.5e %.5e\n",xrescale,yrescale,zrescale);
  /*-- Calculate interp_amp --*/
  for (k=0; k<nk_amp; k++) { // For each empirical node
    gsl_vector v = gsl_vector_subvector(cvec_amp, k*N, N).vector; // Pick out the coefficient matrix corresponding to the k-th node.
    sum = gsl_cheb_eval_3d(&v, nx, ny, nz, xrescale, yrescale, zrescale) ;
//     fprintf(stdout,"sum_amp%d - %.5e\n",k,sum);
    gsl_vector_set(interp_amp, k, sum); //write p_k(x,y,z)
  }

  /*-- Calculate interp_phi --*/
  for (k=0; k<nk_phi; k++) { // For each empirical node
    gsl_vector v = gsl_vector_subvector(cvec_phi, k*N, N).vector; // Pick out the coefficient matrix corresponding to the k-th node.
    sum = gsl_cheb_eval_3d(&v, nx, ny, nz, xrescale, yrescale, zrescale) ;
//     fprintf(stdout,"sum_phi%d - %.5e\n",k,sum);
    gsl_vector_set(interp_phi, k, sum); //write p_k(x,y,z)
  }

  return 0;

}

static int EOBROMEOSCore_datatest(
  REAL8TimeSeries **hPlus,
  REAL8TimeSeries **hCross,
  double phiRef, // orbital reference phase
  double deltaT,
  double fRef,
  double distance,
  double inclination,
  double Mtot, // in Msol
  double eta,
  double lambda1,
  double lambda2
)
{

  /* Check output arrays */
  if(!hPlus || !hCross)
    XLAL_ERROR(XLAL_EFAULT);
  EOB_ROM_EOSdataDS *romdata=&__lalsim_EOB_ROM_EOSDS_data;
  if(*hPlus || *hCross)
  {
    XLALPrintError("(*hPlus) and (*hCross) are supposed to be NULL, but got %p and %p",(*hPlus),(*hCross));
    XLAL_ERROR(XLAL_EFAULT);
  }
  int retcode=0;

  /* Select ROM submodel */
  EOB_ROM_EOSdataDS_submodel *submodel;
  submodel = romdata->sub1;

  fprintf(stdout,"--- EOSROMEOSCore input parameters ---\n");
  fprintf(stdout,"  phiRef       = %.2f\n",phiRef);
  fprintf(stdout,"  fRef         = %.2f\n",fRef);
  fprintf(stdout,"  distance     = %.2f\n",distance);
  fprintf(stdout,"  inclination  = %.2f\n",inclination);
  fprintf(stdout,"  Mtot         = %.2f\n",Mtot);
  fprintf(stdout,"  eta          = %.2f\n",eta);
  fprintf(stdout,"  lambda1      = %.2e\n",lambda1);
  fprintf(stdout,"  lambda2      = %.2e\n",lambda2);
  fprintf(stdout,"  deltaT       = %.2f\n\n",deltaT);

  fprintf(stdout,"--- submodel check ---\n");
  fprintf(stdout,"  cvec_amp size = %d\n",(int) (submodel->cvec_amp->size));
  fprintf(stdout,"  cvec_phi size = %d\n",(int) (submodel->cvec_phi->size));
  fprintf(stdout,"  Bamp size     = %dx%d\n",(int) (submodel->Bamp->size1),(int) (submodel->Bamp->size2));
  fprintf(stdout,"  Bphi size     = %dx%d\n",(int) (submodel->Bphi->size1),(int) (submodel->Bphi->size2));
  fprintf(stdout,"  times size    = %d\n",(int) (submodel->times->size));
  fprintf(stdout,"  q-min         = %.2f\n",submodel->params_min[0]);
  fprintf(stdout,"  q-max         = %.2f\n",submodel->params_max[0]);
  fprintf(stdout,"  lambda1-min   = %.2f\n",submodel->params_min[1]);
  fprintf(stdout,"  lambda1-max   = %.2f\n",submodel->params_max[1]);
  fprintf(stdout,"  lambda2-min   = %.2f\n",submodel->params_min[2]);
  fprintf(stdout,"  lambda2-max   = %.2f\n",submodel->params_max[2]);

  double x = sqrt(1.0-4.0*eta) ;
  double q = (1+x)/(1-x);
  fprintf(stdout,"  q             = %.2f\n\n",q);

  //Allocate space for the nodes
  gsl_vector *amp_at_nodes = gsl_vector_alloc(submodel->times->size);
  gsl_vector *phi_at_nodes = gsl_vector_alloc(submodel->times->size);
  //Allocate space for the interpolants
  //gsl_vector *amp_interp = gsl_vector_alloc (Gntimes) ;
  //gsl_vector *phi_interp = gsl_vector_alloc (Gntimes) ;

  double *amp_interp = calloc(Gntimes,sizeof(double));
  double *phi_interp = calloc(Gntimes,sizeof(double));
  double *freqs = calloc(Gntimes,sizeof(double));
  double *physical_times = calloc(Gntimes,sizeof(double));

  fprintf(stdout,"--- calculating chebyshev interpolants (A(T_j),Phi(T_j)) ---\n");
  retcode = chebyshev_interpolation3d(q,lambda1,lambda2,
                                      Gnq, Gnlambda1, Gnlambda2,
                                      submodel->cvec_amp,submodel->cvec_phi,
                                      Gnamp,Gnphase,amp_at_nodes,phi_at_nodes);

  fprintf(stdout,"\n--- calculating A(t) and Phi(t) ---\n");
  double BjAmp_tn=0.0;
  double BjPhi_tn=0.0;
  int n,j;
  double c3 = LAL_C_SI*LAL_C_SI*LAL_C_SI ;
  double time_to_phys = LAL_G_SI*Mtot*LAL_MSUN_SI/c3 ;
  for (n=0;n<Gntimes;n++){
    BjAmp_tn=0.0 ;
    BjPhi_tn=0.0 ;
    for (j=0;j<Gnamp;j++){
      BjAmp_tn+=gsl_vector_get(amp_at_nodes,j)*gsl_matrix_get(submodel->Bamp,j,n);
    }
    for (j=0;j<Gnphase;j++){
      BjPhi_tn+=gsl_vector_get(phi_at_nodes,j)*gsl_matrix_get(submodel->Bphi,j,n);
    }
    //convert times in to seconds
    physical_times[n]=gsl_vector_get(submodel->times,n)*time_to_phys;
    amp_interp[n]=BjAmp_tn;
    phi_interp[n]=BjPhi_tn;
    fprintf(stdout,"%.9e %.9e %.9e\n",amp_interp[n],phi_interp[n],physical_times[n]);
  }

  //Build the waveform in physical units from A and Phi
  //starts at timedomainrom.py line 233


//   double c3 = LAL_C_SI*LAL_C_SI*LAL_C_SI ;
//   time_to_phys = LAL_G_SI*Mtot*LAL_MSUN_SI/c3 ;
//   for

//   gsl_interp_cspline
//   gsl_interp *ampoft = gsl_interp_alloc (const gsl_interp_type * T, Gntimes);

  fprintf(stdout,"--- Resampling A(t) and Phi(t) to arbitrary deltaT ---\n");
  gsl_interp_accel *acc = gsl_interp_accel_alloc();
  gsl_spline *ampoft_spline = gsl_spline_alloc (gsl_interp_cspline, Gntimes);
  gsl_spline *phioft_spline = gsl_spline_alloc (gsl_interp_cspline, Gntimes);

  fprintf(stdout,"    -.- initializing splines\n");
  gsl_spline_init(ampoft_spline, physical_times, amp_interp, Gntimes);
  gsl_spline_init(phioft_spline, physical_times, phi_interp, Gntimes);

  double der ;
  //int i_end_mono ;
  fprintf(stdout,"    -.- calculating frequencies (derivative of phi(t)/2pi)\n");
  for (n=0;n<Gntimes;n++) {
    der = gsl_spline_eval_deriv (phioft_spline, physical_times[n], acc);
    freqs[n] = 0.5*der/LAL_PI;//omegaoft(time_phys)/(2*np.pi)
    fprintf(stdout,"%.9e %.9e\n",physical_times[n],freqs[n]);
    //determine from where f is monotonically increasing
  }

  //fprintf(stdout,"    -.- calculating t(f)\n");
  //# Find region where frequency is monotonically increasing, then construct t(f)

  //i_end_mono = next( (i for i in range(len(freq)-1) if freq[i]>=freq[i+1]), (len(freq)-1) )
  //toffreq = scipy.interpolate.UnivariateSpline(freq[:i_end_mono], time_phys[:i_end_mono], k=order, s=0)

  //EVALUATE the spline at any time
/*  double yi = 0.0 ;
  for (n=0;n<Gntimes;n++)
  {
    yi = gsl_spline_eval(spline, xi, acc);
    printf ("%g %g\n", xi, yi);
  }
*/


//   # Resample with even spacing
//   tstart = toffreq([f_lower])[0]
//   time_phys_res = np.arange(tstart, time_phys[-1], delta_t)
//   amp_res = ampoft(time_phys_res)
//   phase_res = phaseoft(time_phys_res)
//   # Zero the phase at the beginning
//   phase_res -= phase_res[0]
//
//   # Rescale amplitude
//   #h22_to_h = np.sqrt(5.0/np.pi)/8.0
//   #amp_units = G_SI*mtot/(C_SI**2*distance*MPC_SI)
//   #amp_rescale = amp_units*h22_to_h*amp_res
//   h22_to_h = 4.0*eta*np.sqrt(5.0/np.pi)/8.0
//   amp_units = G_SI*mtot/(C_SI**2*distance*MPC_SI)
//   amp_rescale = amp_units*h22_to_h*amp_res
//
//   # Adjust for inclination angle [0, pi]
//   inc_plus = (1.0+np.cos(inclination)**2)/2.0
//   inc_cross = np.cos(inclination)
//
//   hplus = inc_plus*amp_rescale*np.cos(phase_res)
//   hcross = inc_cross*amp_rescale*np.sin(phase_res)

  gsl_spline_free (ampoft_spline);
  gsl_spline_free (phioft_spline);
  gsl_interp_accel_free (acc);

  gsl_vector_free(amp_at_nodes);
  gsl_vector_free(phi_at_nodes);

  free(amp_interp);
  free(phi_interp);
  free(freqs);
  free(physical_times);

  if (retcode==0){
    return(XLAL_SUCCESS);
  } else {
    return(retcode);
  }

}

/**
 * @addtogroup LALSimIMRSEOBNRROM_c
 *
 * @{
 *
 * @name SEOBNRv2 Reduced Order Model (Double Spin)
 *
 * @author Michael Puerrer, John Veitch
 *
 * @brief C code for SEOBNRv2 reduced order model (double spin version).
 * See CQG 31 195010, 2014, arXiv:1402.4146 for the basic approach.
 * Further details in PRD 93, 064041, 2016, arXiv:1512.02248.
 *
 * This is a frequency domain model that approximates the time domain SEOBNRv2 model.
 *
 * The binary data files are available at https://dcc.ligo.org/T1400701-v1.
 * Put the untared data into a location in your LAL_DATA_PATH.
 *
 * @note Note that due to its construction the iFFT of the ROM has a small (~ 20 M) offset
 * in the peak time that scales with total mass as compared to the time-domain SEOBNRv2 model.
 *
 * @note Parameter ranges:
 *   * 0.01 <= eta <= 0.25
 *   * -1 <= chi_i <= 0.99
 *   * Mtot >= 12Msun
 *
 *  Aligned component spins chi1, chi2.
 *  Symmetric mass-ratio eta = m1*m2/(m1+m2)^2.
 *  Total mass Mtot.
 *
 * @{
 */


//Function to test data reading
int XLALSimIMREOBROMEOS_datatest(
  REAL8TimeSeries **hPlus, /**< Output: Frequency-domain waveform h+ */
  REAL8TimeSeries **hCross, /**< Output: Frequency-domain waveform hx */
  REAL8 phiRef,                                 /**< Orbital phase at reference frequency*/
  REAL8 deltaT,                                 /**< Sampling frequency (Hz) */
  REAL8 fLow,                                   /**< Starting GW frequency (Hz) */
  REAL8 fRef,                                   /**< Reference frequency (Hz); 0 defaults to fLow */
  REAL8 distance,                               /**< Distance of source (m) */
  REAL8 inclination,                            /**< Inclination of source (rad) */
  REAL8 m1SI,                                   /**< Mass of companion 1 (kg) */
  REAL8 m2SI,                                   /**< Mass of companion 2 (kg) */
  REAL8 lambda1,                                /**< Dimensionless aligned component spin 1 */
  REAL8 lambda2)                                /**< Dimensionless aligned component spin 2 */
{
  /* Internally we need m1 > m2, so change around if this is not the case */
  if (m1SI < m2SI) {
    // Swap m1 and m2
    double m1temp = m1SI;
    double l1temp = lambda1;
    m1SI = m2SI;
    lambda1 = lambda2;
    m2SI = m1temp;
    lambda2 = l1temp;
  }

  /* Get masses in terms of solar mass */
  double mass1 = m1SI / LAL_MSUN_SI;
  double mass2 = m2SI / LAL_MSUN_SI;
  double Mtot = mass1+mass2;
  double eta = mass1 * mass2 / (Mtot*Mtot);    /* Symmetric mass-ratio */
//   double Mtot_sec = Mtot * LAL_MTSUN_SI;       /* Total mass in seconds */

  REAL8 m1sec = (m1SI/LAL_MSUN_SI)*LAL_MTSUN_SI ;
  REAL8 m2sec = (m2SI/LAL_MSUN_SI)*LAL_MTSUN_SI ;
  REAL8 m1sec5=m1sec*m1sec*m1sec*m1sec*m1sec ;
  REAL8 m2sec5=m2sec*m2sec*m2sec*m2sec*m2sec ;
  lambda1*=m1sec5 ;
  lambda2*=m2sec5 ;

  if (fRef==0.0) fRef=fLow;

  // Load ROM data if not loaded already
  fprintf(stdout,"initializing with EOB_ROM_EOS_Init_LALDATA()\n");
  #ifdef LAL_PTHREAD_LOCK
  (void) pthread_once(&EOB_ROM_EOS_is_initialized, EOB_ROM_EOS_Init_LALDATA);
  #else
  EOB_ROM_EOS_Init_LALDATA();
  #endif

  if(!EOB_ROM_EOS_IsSetup()) XLAL_ERROR(XLAL_EFAILED,"Error setting up EOB_ROM_EOS data - check your $LAL_DATA_PATH\n");

  // Use fLow, fHigh, deltaF to compute freqs sequence
  // Instead of building a full sequency we only transfer the boundaries and let
  // the internal core function do the rest (and properly take care of corner cases).

  int retcode = EOBROMEOSCore_datatest(hPlus,hCross, phiRef, deltaT, fRef, distance, inclination, Mtot, eta, lambda1, lambda2);

  EOB_ROM_EOSdataDS_Cleanup(&__lalsim_EOB_ROM_EOSDS_data);

  return(retcode);
}



/** Setup SEOBNRv2ROMDoubleSpin model using data files installed in $LAL_DATA_PATH
 */
static void EOB_ROM_EOS_Init_LALDATA(void)
{
  if (EOB_ROM_EOS_IsSetup()) return;

  // If we find one ROM datafile in a directory listed in LAL_DATA_PATH,
  // then we expect the remaining datafiles to also be there.
  char datafile[] = "EOB_ROM_EOS_Phase_ciall.dat";

  char *path = XLALFileResolvePathLong(datafile, PKG_DATA_DIR);
  if (path==NULL)
    XLAL_ERROR_VOID(XLAL_EIO, "Unable to resolve data file %s in $LAL_DATA_PATH\n", datafile);
  char *dir = dirname(path);
  int ret = EOB_ROM_EOS_Init(dir);
  XLALFree(path);

  if(ret!=XLAL_SUCCESS)
    XLAL_ERROR_VOID(XLAL_FAILURE, "Unable to find EOB_ROM_EOS data files in $LAL_DATA_PATH\n");
}
