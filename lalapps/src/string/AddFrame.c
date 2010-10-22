/*
*  Copyright (C) 2010 Florent Robinet
*/

#include <stdio.h>
#include <stdlib.h>

#include <lal/LALFrameIO.h>
#include <lal/FrameCache.h>
#include <lal/FrameStream.h>
#include <lal/LALConstants.h>
#include <lal/Date.h>
#include <lal/Units.h>
#include <lal/TimeSeries.h>

int Min(int a, int b);
/***************************************************************************/

int main(int argc,char *argv[])
{
  FrameH *frame;                /* output frame        */
  char filename[256];           /* output file name    */

  REAL8TimeSeries *ht_1;        /* H1 strain data      */
  REAL8TimeSeries *ht_2;        /* H2 strain data      */
  REAL8TimeSeries *ht_p;        /* H+ strain data      */
  REAL8TimeSeries *ht_m;        /* H- strain data      */

  char cache_name_1[256];       /* H1 frame cache name */
  char cache_name_2[256];       /* H2 frame cache name */
  FrCache *cache_1;             /* H1 frame cache      */
  FrCache *cache_2;             /* H2 frame cache      */
  FrStream *stream_1;           /* H1 stream           */
  FrStream *stream_2;           /* H2 stream           */

  int i, p, gps_start_i, gps_end_i, start, end;
  LIGOTimeGPS gps_start, gps_end;

  static const char func[] = "AddFrame";
  cache_name_1[0]='\0';
  cache_name_2[0]='\0';
  filename[0]='\0';
  ht_1=NULL;
  ht_2=NULL;
  
  /* read arguments */
  if(argc!=4){
    printf("%s(): usage: lalapps_StringAddFrame [gps_start] [gps_end] [outdir]\n",func);
    printf("    There should be the H1 and H2 cache files ready in [outdir] named H1.cache and H2.cache\n");
    return 1;
  }
  gps_start_i=atoi(argv[1]);
  gps_end_i=atoi(argv[2]);
  sprintf(cache_name_1,"%s/H1.cache", argv[3]);
  sprintf(cache_name_2,"%s/H2.cache", argv[3]);
  
  /* create Frame cache, open frame stream and delete frame cache */
  cache_1 = XLALFrImportCache(cache_name_1);
  if(!cache_1){
    printf("%s(): no cache named %s\n",func,cache_name_1);
    return 2;
  }
  stream_1 = XLALFrCacheOpen(cache_1);
  XLALFrDestroyCache(cache_1);
  if(!stream_1){
    printf("%s(): no stream for H1\n",func);
    return 2;
  }
  if(!stream_1) XLAL_ERROR(func, XLAL_EFUNC);
 
  cache_2 = XLALFrImportCache(cache_name_2);
  if(!cache_2){
    printf("%s(): no cache named %s\n",func,cache_name_2);
    return 3;
  }
  stream_2 = XLALFrCacheOpen(cache_2);
  XLALFrDestroyCache(cache_2);
  if(!stream_2){
    printf("%s(): no stream for H2\n",func);
    return 3;
  }
 
  /* turn on checking for missing data */
  XLALFrSetMode(stream_1, LAL_FR_VERBOSE_MODE);
  XLALFrSetMode(stream_2, LAL_FR_VERBOSE_MODE);

  /* divide the time period into 128s segments */
  for(i=gps_start_i; i<gps_end_i; i+=128){
   
    start=i;
    end=Min(i+128,gps_end_i);
    printf("Building frame %d-%d...\n",start,end);
    
    /* define time segment */
    gps_start.gpsSeconds     = start;
    gps_start.gpsNanoSeconds = 0;
    gps_end.gpsSeconds     = end;
    gps_end.gpsNanoSeconds = 0;
  
    /* read H1 and H2 data */
    ht_1 = XLALFrReadREAL8TimeSeries(stream_1, "H1:LSC-STRAIN", &gps_start, XLALGPSDiff(&gps_end, &gps_start), 0);
    if(!ht_1) {
      XLALFrClose(stream_1);
      printf("%s(): cannot read data for H1:LSC-STRAIN\n",func);
    }
    ht_2 = XLALFrReadREAL8TimeSeries(stream_2, "H2:LSC-STRAIN", &gps_start, XLALGPSDiff(&gps_end, &gps_start), 0);
    if(!ht_2) {
      XLALFrClose(stream_2);
      printf("%s(): cannot read data for H2:LSC-STRAIN\n",func);
    }

    /* units */
    ht_1->sampleUnits = lalStrainUnit;
    ht_2->sampleUnits = lalStrainUnit;

    /* create H+ and H- time series */
    ht_p = XLALCreateREAL8TimeSeries("H1:LSC-STRAIN_HPLUS", &gps_start, ht_1->f0, ht_1->deltaT, &ht_1->sampleUnits, ht_1->data->length);
    ht_m = XLALCreateREAL8TimeSeries("H2:LSC-STRAIN_HMINUS", &gps_start, ht_2->f0, ht_2->deltaT, &ht_2->sampleUnits, ht_2->data->length);
    
    /* fill h+ and h- data vectors */
    for ( p = 0 ; p < (int)ht_1->data->length; p++ ){
      ht_p->data->data[p]=ht_1->data->data[p]+ht_2->data->data[p];
      ht_m->data->data[p]=ht_1->data->data[p]-ht_2->data->data[p];
    }
    
    /* save time series into a frame */
    frame = XLALFrameNew(&gps_start, XLALGPSDiff(&gps_end, &gps_start), "LIGO", 0, 1,LAL_LHO_4K_DETECTOR_BIT);
    XLALFrameAddREAL8TimeSeriesProcData( frame, ht_1 );
    XLALFrameAddREAL8TimeSeriesProcData( frame, ht_2 );
    XLALFrameAddREAL8TimeSeriesProcData( frame, ht_p );
    XLALFrameAddREAL8TimeSeriesProcData( frame, ht_m );
    sprintf(filename,"%s/H-H1H2_COHERENT-%d-%d.gwf", argv[3],start,end-start);
    XLALFrameWrite(frame, filename, 0);
    
    /* cleaning */
    XLALDestroyREAL8TimeSeries(ht_1);
    XLALDestroyREAL8TimeSeries(ht_2);
    XLALDestroyREAL8TimeSeries(ht_p);
    XLALDestroyREAL8TimeSeries(ht_m);
    FrameFree(frame);
  }
  
  /* close streams */
  XLALFrClose(stream_1);
  XLALFrClose(stream_2);
  
  return 0;
}

/***************************************************************************/

int Min(int a, int b){
  if(a>b) return b;
  else return a;
}
