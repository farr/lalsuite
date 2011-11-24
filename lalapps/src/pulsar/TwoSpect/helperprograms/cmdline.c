/*
  File autogenerated by gengetopt version 2.22.4
  generated with the following command:
  /Users/evgoet/opt/bin/gengetopt --string-parser --conf-parser

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "cmdline.h"

const char *gengetopt_args_info_purpose = "Helper program to output the skygrid";

const char *gengetopt_args_info_usage = "Usage: skygridsetup [OPTIONS]...";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                Print help and exit",
  "  -V, --version             Print version and exit",
  "      --config=STRING       Configuration file in gengetopt format for passing \n                              parameters",
  "      --Tcoh=DOUBLE         SFT coherence time  (default=`1800')",
  "      --fmin=DOUBLE         Minimum frequency of band",
  "      --fspan=DOUBLE        Frequency span of band",
  "      --IFO=STRING          Interferometer of whose data is being analyzed  \n                              (default=`H1')",
  "      --outfilename=STRING  Output file name  (default=`logfile.txt')",
  "      --ephemDir=STRING     Path to ephemeris files  \n                              (default=`/opt/lscsoft/lalpulsar/share/lalpulsar')",
  "      --ephemYear=STRING    Year or year range (e.g. 08-11) of ephemeris files  \n                              (default=`08-11')",
  "      --skyRegion=STRING    Region of the sky to search (e.g. \n                              (ra1,dec1),(ra2,dec2),(ra3,dec3)...) or allsky  \n                              (default=`allsky')",
  "      --SFToverlap=DOUBLE   SFT overlap in seconds, usually Tcoh/2  \n                              (default=`900')",
    0
};

typedef enum {ARG_NO
  , ARG_STRING
  , ARG_DOUBLE
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

struct line_list
{
  char * string_arg;
  struct line_list * next;
};

static struct line_list *cmd_line_list = 0;
static struct line_list *cmd_line_list_tmp = 0;

static void
free_cmd_list(void)
{
  /* free the list of a previous call */
  if (cmd_line_list)
    {
      while (cmd_line_list) {
        cmd_line_list_tmp = cmd_line_list;
        cmd_line_list = cmd_line_list->next;
        free (cmd_line_list_tmp->string_arg);
        free (cmd_line_list_tmp);
      }
    }
}


static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->config_given = 0 ;
  args_info->Tcoh_given = 0 ;
  args_info->fmin_given = 0 ;
  args_info->fspan_given = 0 ;
  args_info->IFO_given = 0 ;
  args_info->outfilename_given = 0 ;
  args_info->ephemDir_given = 0 ;
  args_info->ephemYear_given = 0 ;
  args_info->skyRegion_given = 0 ;
  args_info->SFToverlap_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->config_arg = NULL;
  args_info->config_orig = NULL;
  args_info->Tcoh_arg = 1800;
  args_info->Tcoh_orig = NULL;
  args_info->fmin_orig = NULL;
  args_info->fspan_orig = NULL;
  args_info->IFO_arg = gengetopt_strdup ("H1");
  args_info->IFO_orig = NULL;
  args_info->outfilename_arg = gengetopt_strdup ("logfile.txt");
  args_info->outfilename_orig = NULL;
  args_info->ephemDir_arg = gengetopt_strdup ("/opt/lscsoft/lalpulsar/share/lalpulsar");
  args_info->ephemDir_orig = NULL;
  args_info->ephemYear_arg = gengetopt_strdup ("08-11");
  args_info->ephemYear_orig = NULL;
  args_info->skyRegion_arg = gengetopt_strdup ("allsky");
  args_info->skyRegion_orig = NULL;
  args_info->SFToverlap_arg = 900;
  args_info->SFToverlap_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->config_help = gengetopt_args_info_help[2] ;
  args_info->Tcoh_help = gengetopt_args_info_help[3] ;
  args_info->fmin_help = gengetopt_args_info_help[4] ;
  args_info->fspan_help = gengetopt_args_info_help[5] ;
  args_info->IFO_help = gengetopt_args_info_help[6] ;
  args_info->outfilename_help = gengetopt_args_info_help[7] ;
  args_info->ephemDir_help = gengetopt_args_info_help[8] ;
  args_info->ephemYear_help = gengetopt_args_info_help[9] ;
  args_info->skyRegion_help = gengetopt_args_info_help[10] ;
  args_info->SFToverlap_help = gengetopt_args_info_help[11] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{

  free_string_field (&(args_info->config_arg));
  free_string_field (&(args_info->config_orig));
  free_string_field (&(args_info->Tcoh_orig));
  free_string_field (&(args_info->fmin_orig));
  free_string_field (&(args_info->fspan_orig));
  free_string_field (&(args_info->IFO_arg));
  free_string_field (&(args_info->IFO_orig));
  free_string_field (&(args_info->outfilename_arg));
  free_string_field (&(args_info->outfilename_orig));
  free_string_field (&(args_info->ephemDir_arg));
  free_string_field (&(args_info->ephemDir_orig));
  free_string_field (&(args_info->ephemYear_arg));
  free_string_field (&(args_info->ephemYear_orig));
  free_string_field (&(args_info->skyRegion_arg));
  free_string_field (&(args_info->skyRegion_orig));
  free_string_field (&(args_info->SFToverlap_orig));
  
  

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->config_given)
    write_into_file(outfile, "config", args_info->config_orig, 0);
  if (args_info->Tcoh_given)
    write_into_file(outfile, "Tcoh", args_info->Tcoh_orig, 0);
  if (args_info->fmin_given)
    write_into_file(outfile, "fmin", args_info->fmin_orig, 0);
  if (args_info->fspan_given)
    write_into_file(outfile, "fspan", args_info->fspan_orig, 0);
  if (args_info->IFO_given)
    write_into_file(outfile, "IFO", args_info->IFO_orig, 0);
  if (args_info->outfilename_given)
    write_into_file(outfile, "outfilename", args_info->outfilename_orig, 0);
  if (args_info->ephemDir_given)
    write_into_file(outfile, "ephemDir", args_info->ephemDir_orig, 0);
  if (args_info->ephemYear_given)
    write_into_file(outfile, "ephemYear", args_info->ephemYear_orig, 0);
  if (args_info->skyRegion_given)
    write_into_file(outfile, "skyRegion", args_info->skyRegion_orig, 0);
  if (args_info->SFToverlap_given)
    write_into_file(outfile, "SFToverlap", args_info->SFToverlap_orig, 0);
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char **argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  FIX_UNUSED (args_info);
  FIX_UNUSED (prog_name);
  return EXIT_SUCCESS;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_DOUBLE:
    if (val) *((double *)field) = strtod (val, &stop_char);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_DOUBLE:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "config",	1, NULL, 0 },
        { "Tcoh",	1, NULL, 0 },
        { "fmin",	1, NULL, 0 },
        { "fspan",	1, NULL, 0 },
        { "IFO",	1, NULL, 0 },
        { "outfilename",	1, NULL, 0 },
        { "ephemDir",	1, NULL, 0 },
        { "ephemYear",	1, NULL, 0 },
        { "skyRegion",	1, NULL, 0 },
        { "SFToverlap",	1, NULL, 0 },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hV", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);


        case 0:	/* Long option with no short option */
          /* Configuration file in gengetopt format for passing parameters.  */
          if (strcmp (long_options[option_index].name, "config") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->config_arg), 
                 &(args_info->config_orig), &(args_info->config_given),
                &(local_args_info.config_given), optarg, 0, 0, ARG_STRING,
                check_ambiguity, override, 0, 0,
                "config", '-',
                additional_error))
              goto failure;
          
          }
          /* SFT coherence time.  */
          else if (strcmp (long_options[option_index].name, "Tcoh") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->Tcoh_arg), 
                 &(args_info->Tcoh_orig), &(args_info->Tcoh_given),
                &(local_args_info.Tcoh_given), optarg, 0, "1800", ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "Tcoh", '-',
                additional_error))
              goto failure;
          
          }
          /* Minimum frequency of band.  */
          else if (strcmp (long_options[option_index].name, "fmin") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->fmin_arg), 
                 &(args_info->fmin_orig), &(args_info->fmin_given),
                &(local_args_info.fmin_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "fmin", '-',
                additional_error))
              goto failure;
          
          }
          /* Frequency span of band.  */
          else if (strcmp (long_options[option_index].name, "fspan") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->fspan_arg), 
                 &(args_info->fspan_orig), &(args_info->fspan_given),
                &(local_args_info.fspan_given), optarg, 0, 0, ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "fspan", '-',
                additional_error))
              goto failure;
          
          }
          /* Interferometer of whose data is being analyzed.  */
          else if (strcmp (long_options[option_index].name, "IFO") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->IFO_arg), 
                 &(args_info->IFO_orig), &(args_info->IFO_given),
                &(local_args_info.IFO_given), optarg, 0, "H1", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "IFO", '-',
                additional_error))
              goto failure;
          
          }
          /* Output file name.  */
          else if (strcmp (long_options[option_index].name, "outfilename") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->outfilename_arg), 
                 &(args_info->outfilename_orig), &(args_info->outfilename_given),
                &(local_args_info.outfilename_given), optarg, 0, "logfile.txt", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "outfilename", '-',
                additional_error))
              goto failure;
          
          }
          /* Path to ephemeris files.  */
          else if (strcmp (long_options[option_index].name, "ephemDir") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->ephemDir_arg), 
                 &(args_info->ephemDir_orig), &(args_info->ephemDir_given),
                &(local_args_info.ephemDir_given), optarg, 0, "/opt/lscsoft/lalpulsar/share/lalpulsar", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "ephemDir", '-',
                additional_error))
              goto failure;
          
          }
          /* Year or year range (e.g. 08-11) of ephemeris files.  */
          else if (strcmp (long_options[option_index].name, "ephemYear") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->ephemYear_arg), 
                 &(args_info->ephemYear_orig), &(args_info->ephemYear_given),
                &(local_args_info.ephemYear_given), optarg, 0, "08-11", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "ephemYear", '-',
                additional_error))
              goto failure;
          
          }
          /* Region of the sky to search (e.g. (ra1,dec1),(ra2,dec2),(ra3,dec3)...) or allsky.  */
          else if (strcmp (long_options[option_index].name, "skyRegion") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->skyRegion_arg), 
                 &(args_info->skyRegion_orig), &(args_info->skyRegion_given),
                &(local_args_info.skyRegion_given), optarg, 0, "allsky", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "skyRegion", '-',
                additional_error))
              goto failure;
          
          }
          /* SFT overlap in seconds, usually Tcoh/2.  */
          else if (strcmp (long_options[option_index].name, "SFToverlap") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->SFToverlap_arg), 
                 &(args_info->SFToverlap_orig), &(args_info->SFToverlap_given),
                &(local_args_info.SFToverlap_given), optarg, 0, "900", ARG_DOUBLE,
                check_ambiguity, override, 0, 0,
                "SFToverlap", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */




  cmdline_parser_release (&local_args_info);

  if ( error )
    return (EXIT_FAILURE);

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}

#ifndef CONFIG_FILE_LINE_SIZE
#define CONFIG_FILE_LINE_SIZE 2048
#endif
#define ADDITIONAL_ERROR " in configuration file "

#define CONFIG_FILE_LINE_BUFFER_SIZE (CONFIG_FILE_LINE_SIZE+3)
/* 3 is for "--" and "=" */

static int
_cmdline_parser_configfile (const char *filename, int *my_argc)
{
  FILE* file;
  char my_argv[CONFIG_FILE_LINE_BUFFER_SIZE+1];
  char linebuf[CONFIG_FILE_LINE_SIZE];
  int line_num = 0;
  int result = 0, equal;
  char *fopt, *farg;
  char *str_index;
  size_t len, next_token;
  char delimiter;

  if ((file = fopen(filename, "r")) == 0)
    {
      fprintf (stderr, "%s: Error opening configuration file '%s'\n",
               CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  while ((fgets(linebuf, CONFIG_FILE_LINE_SIZE, file)) != 0)
    {
      ++line_num;
      my_argv[0] = '\0';
      len = strlen(linebuf);
      if (len > (CONFIG_FILE_LINE_BUFFER_SIZE-1))
        {
          fprintf (stderr, "%s:%s:%d: Line too long in configuration file\n",
                   CMDLINE_PARSER_PACKAGE, filename, line_num);
          result = EXIT_FAILURE;
          break;
        }

      /* find first non-whitespace character in the line */
      next_token = strspn (linebuf, " \t\r\n");
      str_index  = linebuf + next_token;

      if ( str_index[0] == '\0' || str_index[0] == '#')
        continue; /* empty line or comment line is skipped */

      fopt = str_index;

      /* truncate fopt at the end of the first non-valid character */
      next_token = strcspn (fopt, " \t\r\n=");

      if (fopt[next_token] == '\0') /* the line is over */
        {
          farg  = 0;
          equal = 0;
          goto noarg;
        }

      /* remember if equal sign is present */
      equal = (fopt[next_token] == '=');
      fopt[next_token++] = '\0';

      /* advance pointers to the next token after the end of fopt */
      next_token += strspn (fopt + next_token, " \t\r\n");

      /* check for the presence of equal sign, and if so, skip it */
      if ( !equal )
        if ((equal = (fopt[next_token] == '=')))
          {
            next_token++;
            next_token += strspn (fopt + next_token, " \t\r\n");
          }
      str_index  += next_token;

      /* find argument */
      farg = str_index;
      if ( farg[0] == '\"' || farg[0] == '\'' )
        { /* quoted argument */
          str_index = strchr (++farg, str_index[0] ); /* skip opening quote */
          if (! str_index)
            {
              fprintf
                (stderr,
                 "%s:%s:%d: unterminated string in configuration file\n",
                 CMDLINE_PARSER_PACKAGE, filename, line_num);
              result = EXIT_FAILURE;
              break;
            }
        }
      else
        { /* read up the remaining part up to a delimiter */
          next_token = strcspn (farg, " \t\r\n#\'\"");
          str_index += next_token;
        }

      /* truncate farg at the delimiter and store it for further check */
      delimiter = *str_index, *str_index++ = '\0';

      /* everything but comment is illegal at the end of line */
      if (delimiter != '\0' && delimiter != '#')
        {
          str_index += strspn(str_index, " \t\r\n");
          if (*str_index != '\0' && *str_index != '#')
            {
              fprintf
                (stderr,
                 "%s:%s:%d: malformed string in configuration file\n",
                 CMDLINE_PARSER_PACKAGE, filename, line_num);
              result = EXIT_FAILURE;
              break;
            }
        }

    noarg:
      if (!strcmp(fopt,"include")) {
        if (farg && *farg) {
          result = _cmdline_parser_configfile(farg, my_argc);
        } else {
          fprintf(stderr, "%s:%s:%d: include requires a filename argument.\n",
                  CMDLINE_PARSER_PACKAGE, filename, line_num);
        }
        continue;
      }
      len = strlen(fopt);
      strcat (my_argv, len > 1 ? "--" : "-");
      strcat (my_argv, fopt);
      if (len > 1 && ((farg && *farg) || equal))
        strcat (my_argv, "=");
      if (farg && *farg)
        strcat (my_argv, farg);
      ++(*my_argc);

      cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
      cmd_line_list_tmp->next = cmd_line_list;
      cmd_line_list = cmd_line_list_tmp;
      cmd_line_list->string_arg = gengetopt_strdup(my_argv);
    } /* while */

  if (file)
    fclose(file);
  return result;
}

int
cmdline_parser_configfile (
  const char *filename,
                           struct gengetopt_args_info *args_info,
                           int override, int initialize, int check_required)
{
  struct cmdline_parser_params params;

  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;
  
  return cmdline_parser_config_file (filename, args_info, &params);
}

int
cmdline_parser_config_file (const char *filename,
                           struct gengetopt_args_info *args_info,
                           struct cmdline_parser_params *params)
{
  int i, result;
  int my_argc = 1;
  char **my_argv_arg;
  char *additional_error;

  /* store the program name */
  cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
  cmd_line_list_tmp->next = cmd_line_list;
  cmd_line_list = cmd_line_list_tmp;
  cmd_line_list->string_arg = gengetopt_strdup (CMDLINE_PARSER_PACKAGE);

  result = _cmdline_parser_configfile(filename, &my_argc);

  if (result != EXIT_FAILURE) {
    my_argv_arg = (char **) malloc((my_argc+1) * sizeof(char *));
    cmd_line_list_tmp = cmd_line_list;

    for (i = my_argc - 1; i >= 0; --i) {
      my_argv_arg[i] = cmd_line_list_tmp->string_arg;
      cmd_line_list_tmp = cmd_line_list_tmp->next;
    }

    my_argv_arg[my_argc] = 0;

    additional_error = (char *)malloc(strlen(filename) + strlen(ADDITIONAL_ERROR) + 1);
    strcpy (additional_error, ADDITIONAL_ERROR);
    strcat (additional_error, filename);
    result =
      cmdline_parser_internal (my_argc, my_argv_arg, args_info,
                              params,
                              additional_error);

    free (additional_error);
    free (my_argv_arg);
  }

  free_cmd_list();
  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

static unsigned int
cmdline_parser_create_argv(const char *cmdline_, char ***argv_ptr, const char *prog_name)
{
  char *cmdline, *p;
  size_t n = 0, j;
  int i;

  if (prog_name) {
    cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
    cmd_line_list_tmp->next = cmd_line_list;
    cmd_line_list = cmd_line_list_tmp;
    cmd_line_list->string_arg = gengetopt_strdup (prog_name);

    ++n;
  }

  cmdline = gengetopt_strdup(cmdline_);
  p = cmdline;

  while (p && strlen(p))
    {
      j = strcspn(p, " \t");
      ++n;
      if (j && j < strlen(p))
        {
          p[j] = '\0';

          cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
          cmd_line_list_tmp->next = cmd_line_list;
          cmd_line_list = cmd_line_list_tmp;
          cmd_line_list->string_arg = gengetopt_strdup (p);

          p += (j+1);
          p += strspn(p, " \t");
        }
      else
        {
          cmd_line_list_tmp = (struct line_list *) malloc (sizeof (struct line_list));
          cmd_line_list_tmp->next = cmd_line_list;
          cmd_line_list = cmd_line_list_tmp;
          cmd_line_list->string_arg = gengetopt_strdup (p);

          break;
        }
    }

  *argv_ptr = (char **) malloc((n + 1) * sizeof(char *));
  cmd_line_list_tmp = cmd_line_list;
  for (i = (n-1); i >= 0; --i)
    {
      (*argv_ptr)[i] = cmd_line_list_tmp->string_arg;
      cmd_line_list_tmp = cmd_line_list_tmp->next;
    }

  (*argv_ptr)[n] = 0;

  free(cmdline);
  return n;
}

int
cmdline_parser_string(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name)
{
  return cmdline_parser_string2(cmdline, args_info, prog_name, 0, 1, 1);
}

int
cmdline_parser_string2(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name,
    int override, int initialize, int check_required)
{
  struct cmdline_parser_params params;

  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  return cmdline_parser_string_ext(cmdline, args_info, prog_name, &params);
}

int
cmdline_parser_string_ext(const char *cmdline, struct gengetopt_args_info *args_info, const char *prog_name,
    struct cmdline_parser_params *params)
{
  char **argv_ptr = 0;
  int result;
  unsigned int argc;
  
  argc = cmdline_parser_create_argv(cmdline, &argv_ptr, prog_name);
  
  result =
    cmdline_parser_internal (argc, argv_ptr, args_info, params, 0);
  
  if (argv_ptr)
    {
      free (argv_ptr);
    }

  free_cmd_list();
  
  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

