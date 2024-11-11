#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stddef.h>

typedef size_t option_id_t;

typedef struct {
  option_id_t id;
  bool has_value;
  const char* value_name;
  const char* names[2];
  const char* help;
} option_t;

typedef struct {
  const option_t* option;
  const char* value;
} input_option_t;

typedef enum {
  OK = 0,
  INVALID_OPTION,
  MISSING_VALUE,
} argument_parser_error_t;

// Parses an option from the command line arguments.
//
// argc and argv are pointers to the arguments being parsed. On success, they
// are updated to point to the next argument.
//
// options is the list of available options to match against.
//
// If the parsed argument is an option, the corresponding input_option_t is
// appended to input_options.
// If the parsed argument is not an option, it is appended to input_paths.
//
//  Returns OK on success, or an error on failure.
argument_parser_error_t option_parse(int* argc,
                                     const char*** argv,
                                     const option_t* options,
                                     size_t num_options,
                                     input_option_t** input_options,
                                     const char*** input_paths);

#endif  // OPTIONS_H

