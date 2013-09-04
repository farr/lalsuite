/**
 * @file cmdline_skygridsetup.h
 * @brief The header file for the command line option parser
 * generated by GNU Gengetopt version 2.22.4
 * http://www.gnu.org/software/gengetopt.
 * DO NOT modify this file, since it can be overwritten
 * @author GNU Gengetopt by Lorenzo Bettini
 */

#ifndef CMDLINE_SKYGRIDSETUP_H
#define CMDLINE_SKYGRIDSETUP_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "skygridsetup"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "skygridsetup"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "1.0.2"
#endif

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  char * config_arg;	/**< @brief Configuration file in gengetopt format for passing parameters.  */
  char * config_orig;	/**< @brief Configuration file in gengetopt format for passing parameters original value given at command line.  */
  const char *config_help; /**< @brief Configuration file in gengetopt format for passing parameters help description.  */
  double Tcoh_arg;	/**< @brief SFT coherence time (default='1800').  */
  char * Tcoh_orig;	/**< @brief SFT coherence time original value given at command line.  */
  const char *Tcoh_help; /**< @brief SFT coherence time help description.  */
  double SFToverlap_arg;	/**< @brief SFT overlap in seconds, usually Tcoh/2 (default='900').  */
  char * SFToverlap_orig;	/**< @brief SFT overlap in seconds, usually Tcoh/2 original value given at command line.  */
  const char *SFToverlap_help; /**< @brief SFT overlap in seconds, usually Tcoh/2 help description.  */
  double t0_arg;	/**< @brief Start time of the search, needed only if --v2 is specified (in GPS seconds).  */
  char * t0_orig;	/**< @brief Start time of the search, needed only if --v2 is specified (in GPS seconds) original value given at command line.  */
  const char *t0_help; /**< @brief Start time of the search, needed only if --v2 is specified (in GPS seconds) help description.  */
  double Tobs_arg;	/**< @brief Total observation time, needed only if --v2 is specified (in seconds).  */
  char * Tobs_orig;	/**< @brief Total observation time, needed only if --v2 is specified (in seconds) original value given at command line.  */
  const char *Tobs_help; /**< @brief Total observation time, needed only if --v2 is specified (in seconds) help description.  */
  double fmin_arg;	/**< @brief Minimum frequency of band.  */
  char * fmin_orig;	/**< @brief Minimum frequency of band original value given at command line.  */
  const char *fmin_help; /**< @brief Minimum frequency of band help description.  */
  double fspan_arg;	/**< @brief Frequency span of band.  */
  char * fspan_orig;	/**< @brief Frequency span of band original value given at command line.  */
  const char *fspan_help; /**< @brief Frequency span of band help description.  */
  char * IFO_arg;	/**< @brief Interferometer of whose data is being analyzed.  */
  char * IFO_orig;	/**< @brief Interferometer of whose data is being analyzed original value given at command line.  */
  const char *IFO_help; /**< @brief Interferometer of whose data is being analyzed help description.  */
  char * outfilename_arg;	/**< @brief Output file name (default='skygrid.dat').  */
  char * outfilename_orig;	/**< @brief Output file name original value given at command line.  */
  const char *outfilename_help; /**< @brief Output file name help description.  */
  char * ephemDir_arg;	/**< @brief Path to ephemeris files (default='/opt/lscsoft/lalpulsar/share/lalpulsar').  */
  char * ephemDir_orig;	/**< @brief Path to ephemeris files original value given at command line.  */
  const char *ephemDir_help; /**< @brief Path to ephemeris files help description.  */
  char * ephemYear_arg;	/**< @brief Year or year range (e.g. 08-11) of ephemeris files (default='08-11').  */
  char * ephemYear_orig;	/**< @brief Year or year range (e.g. 08-11) of ephemeris files original value given at command line.  */
  const char *ephemYear_help; /**< @brief Year or year range (e.g. 08-11) of ephemeris files help description.  */
  char * skyRegion_arg;	/**< @brief Region of the sky to search (e.g. (ra1,dec1),(ra2,dec2),(ra3,dec3)...) or allsky (default='allsky').  */
  char * skyRegion_orig;	/**< @brief Region of the sky to search (e.g. (ra1,dec1),(ra2,dec2),(ra3,dec3)...) or allsky original value given at command line.  */
  const char *skyRegion_help; /**< @brief Region of the sky to search (e.g. (ra1,dec1),(ra2,dec2),(ra3,dec3)...) or allsky help description.  */
  int v2_flag;	/**< @brief Flag to use the newer style of CompDetectorVmax input arguments (always specify this unless you know what you are doing!) (default=off).  */
  const char *v2_help; /**< @brief Flag to use the newer style of CompDetectorVmax input arguments (always specify this unless you know what you are doing!) help description.  */
  
  unsigned int help_given ;	/**< @brief Whether help was given.  */
  unsigned int version_given ;	/**< @brief Whether version was given.  */
  unsigned int config_given ;	/**< @brief Whether config was given.  */
  unsigned int Tcoh_given ;	/**< @brief Whether Tcoh was given.  */
  unsigned int SFToverlap_given ;	/**< @brief Whether SFToverlap was given.  */
  unsigned int t0_given ;	/**< @brief Whether t0 was given.  */
  unsigned int Tobs_given ;	/**< @brief Whether Tobs was given.  */
  unsigned int fmin_given ;	/**< @brief Whether fmin was given.  */
  unsigned int fspan_given ;	/**< @brief Whether fspan was given.  */
  unsigned int IFO_given ;	/**< @brief Whether IFO was given.  */
  unsigned int outfilename_given ;	/**< @brief Whether outfilename was given.  */
  unsigned int ephemDir_given ;	/**< @brief Whether ephemDir was given.  */
  unsigned int ephemYear_given ;	/**< @brief Whether ephemYear was given.  */
  unsigned int skyRegion_given ;	/**< @brief Whether skyRegion was given.  */
  unsigned int v2_given ;	/**< @brief Whether v2 was given.  */

} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * The config file parser (deprecated version)
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_config_file() instead
 */
int cmdline_parser_configfile (const char *filename,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The config file parser
 * @param filename the name of the config file
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_config_file (const char *filename,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 * possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);

extern const char *cmdline_parser_IFO_values[];  /**< @brief Possible values for IFO. */


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CMDLINE_SKYGRIDSETUP_H */
