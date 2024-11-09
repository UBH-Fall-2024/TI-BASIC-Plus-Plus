#ifndef DRIVER_OPTIONS_H
#define DRIVER_OPTIONS_H

#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <ti-basic-plus-plus/basic/options.h>
#include <ti-basic-plus-plus/basic/stringref.h>

enum {
  OPTION_HELP,                // -h, --help
  OPTION_VERSION,             // --version
  OPTION_WARNINGS_AS_ERRORS,  // -Werror
  OPTION_OUTPUT,             // -o<file>
  OPTION_VERBOSE,            // -v
  OPTION_SUPPRESS_WARNINGS,  // -w
  _OPTION_COUNT,
};

extern const option_t driver_options[];
extern const size_t driver_options_count;

typedef struct {
  const char* program_name;

  const char* input_path;
  char output_program_name[8];
  const char* output_path;

  enum {
    OUTPUT_ASCII,
    OUTPUT_8XP,
  } output_type;

  bool build;
  bool send;

  bool verbose;
  bool print_ast;
} driver_config_t;

#ifndef DRIVER_OPTIONS_IMPL
extern const driver_config_t driver_config;
#endif

void parse_arguments(int argc, const char** argv, diagnostics_t* d);

#endif  // DRIVER_OPTIONS_H

