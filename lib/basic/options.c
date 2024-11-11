#include <ti-basic-plus-plus/basic/options.h>

#include <ti-basic-plus-plus/basic/stringref.h>
#include <stb_ds.h>
#include <assert.h>

// Matches an argument against a given option. If the argument does not match
// the option, 0 is returned. Otherwise, returns the length of the matched
// option name in the argument.
static size_t option_match(const option_t* option, stringref_t arg);

argument_parser_error_t option_parse(int* argc,
                                     const char*** argv,
                                     const option_t* options,
                                     size_t num_options,
                                     input_option_t** input_options,
                                     const char*** input_paths) {

  assert(argc != NULL);
  assert(*argc > 0);
  assert(argv != NULL);
  assert(options != NULL);
  assert(input_options != NULL);
  
  stringref_t arg = stringref_create(**argv);

  if (arg.length == 0)
    goto DONE;  // Just in case

  if (arg.data[0] != '-') {  // Not an option, must be an input path
    arrput(*input_paths, arg.data);
    goto DONE;
  }

  // Skip '-'
  if (stringref_increment(&arg) == 0) {
    // Nothing left?!
    return INVALID_OPTION;
  }

  if (arg.data[0] == '-') {  // Second '-'
    // Skip it- it means nothing.
    if (stringref_increment(&arg) == 0) {
      return INVALID_OPTION;
    }
  }

  size_t matched_name_length = 0;

  const option_t* option = options;
  for (size_t i = 0; i < num_options; ++i, ++option) {
    matched_name_length = option_match(option, arg);
    if (matched_name_length > 0)
      break;
  }

  if (matched_name_length == 0) {
    return INVALID_OPTION;
  }

  assert(arg.length >= matched_name_length);

  // No value required.
  if (!option->has_value) {
    input_option_t input_option = {.option = option, .value = NULL};

    arrput(*input_options, input_option);

    goto DONE;
  }

  // Separated value.
  if (arg.length == matched_name_length) {
    if (*argc == 1) {
      return MISSING_VALUE;
    }

    --*argc;
    ++*argv;

    input_option_t input_option = {.option = option, .value = **argv};

    arrput(*input_options, input_option);

    goto DONE;
  }

  // Connected value.
  assert(arg.length > matched_name_length);

  // Skip the matched name.
  (void)stringref_increment_n(&arg, matched_name_length);

  if (arg.data[0] == '=') {
    if (stringref_increment(&arg) == 0) {
      return MISSING_VALUE;
    }
  }

  input_option_t input_option = {.option = option, .value = arg.data};

  arrput(*input_options, input_option);

DONE:
  --*argc;
  ++*argv;
  return OK;
}

static size_t option_match(const option_t* option, stringref_t arg) {
  assert(option != NULL);
  assert(stringref_is_empty(&arg) == false);

  stringref_t name;
  bool match_condition;
  for (size_t i = 0; i < 2; ++i) {
    const char* name_cstr = option->names[i];
    if (name_cstr == NULL) {
      continue;
    }

    name = stringref_create(name_cstr);

    if (option->has_value) {
      match_condition = stringref_starts_with(&arg, &name);
    } else {
      match_condition = stringref_eq(&arg, &name);
    }

    if (match_condition)
      return name.length;
  }

  return 0;

}
