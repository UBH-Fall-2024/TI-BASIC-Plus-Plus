#define DRIVER_OPTIONS_IMPL
#include <driver/options.h>

#include <assert.h>
#include <stb_ds.h>
#include <stdio.h>
#include <string.h>

driver_config_t driver_config;

#define OPT_NAME(name) &"-" name[1]

// clang-format off
const option_t driver_options[] = {
    {OPTION_WARNINGS_AS_ERRORS,  false,     NULL, {OPT_NAME("Werror"),           NULL}, "Treat warnings as errors"},
    {OPTION_OUTPUT,              true,  "<file>", {OPT_NAME("o"),                NULL}, "Write output to file"},
    {OPTION_VERBOSE,             false,     NULL, {OPT_NAME("v"),                NULL}, "Enable verbose output"},
    {OPTION_SUPPRESS_WARNINGS,   false,     NULL, {OPT_NAME("w"),                NULL}, "Suppress warnings"},
};
// clang-format on

const size_t driver_options_count = sizeof(driver_options) / sizeof(option_t);

static void print_usage(FILE* output);
static void print_version(void);

static void driver_config_init(void);

static void parse_driver_task(const char* task, diagnostics_t* d);
static void process_input_options(input_option_t* input_options,
                                  diagnostics_t* d);

static void driver_config_init(void) {
  memset(&driver_config, 0, sizeof(driver_config_t));
}

void parse_arguments(int argc, const char** argv, diagnostics_t* d) {
  assert(argc > 0 && argv != NULL);
  assert(d != NULL);

  driver_config_init();

  driver_config.program_name = argv[0];
  ++argv, --argc;

  if (!argc) {
    diag_report(d, ERROR, "missing driver task");
    return;
  }
  parse_driver_task(argv[0], d);
  ++argv, --argc;
  if (should_exit(d)) {
    return;
  }

  const char** input_paths = NULL;
  input_option_t* input_options = NULL;

  while (argc > 0) {
    argument_parser_error_t error =
        option_parse(&argc, &argv, driver_options, driver_options_count,
                     &input_options, &input_paths);

    if (error == INVALID_OPTION) {
      diag_report(d, ERROR, "invalid option '%s'", *argv);
      goto CLEANUP;
    } else if (error == MISSING_VALUE) {
      diag_report(d, ERROR, "missing value for option '%s'", *argv);
      goto CLEANUP;
    }
  }

  size_t num_input_paths = arrlenu(input_paths);
  if (num_input_paths < 1) {
    diag_report(d, ERROR, "no input file given");
  } else if (num_input_paths > 1) {
    diag_report(d, ERROR, "multiple input files given");
  }

  process_input_options(input_options, d);

CLEANUP:
  arrfree(input_paths);
  arrfree(input_options);
  return;
}

static void parse_driver_task(const char* task, diagnostics_t* d) {
  if (strcmp(task, "build") == 0) {
    driver_config.build = true;
  } else if (strcmp(task, "send") == 0) {
    driver_config.send = true;
  } else if (strcmp(task, "buildsend") == 0) {
    driver_config.build = true;
    driver_config.send = true;
  } else if (strcmp(task, "help") == 0) {
    print_usage(stdout);
    d->should_exit = true;
  } else if (strcmp(task, "version") == 0) {
    print_version();
    d->should_exit = true;
  } else {
    diag_report(d, ERROR, "invalid driver task '%s'", task);
    print_usage(stderr);
  }
}

static void process_input_options(input_option_t* input_options,
                                  diagnostics_t* d) {
  assert(d != NULL);
  bool seen_options[_OPTION_COUNT] = {0};

  option_id_t id;
  const char* value;
  for (size_t i = 0; i < arrlenu(input_options); ++i) {
    if (should_exit(d)) {
      return;
    }

    id = input_options[i].option->id;
    value = input_options[i].value;

    assert(input_options[i].option->has_value <= (value != NULL));

    if (seen_options[id]) {
      diag_report(d, ERROR, "duplicate option '%s'",
                  input_options[i].option->names[0]);
      return;
    }
    seen_options[id] = true;

    switch (id) {
      case OPTION_WARNINGS_AS_ERRORS:
        d->warnings_as_errors = true;
        break;
      case OPTION_OUTPUT:
        driver_config.output_path = value;
        break;
      case OPTION_VERBOSE:
        driver_config.verbose = true;
        break;
      case OPTION_SUPPRESS_WARNINGS:
        d->suppress_warnings = true;
        break;
      default:
        assert(false);
        return;
    }
  }
}

static void print_usage(FILE* output) {
  fprintf(output, "Usage: ti-basic++ <task> [options] <input file>\n");
  fprintf(output, "Tasks:\n");

  // clang-format off
  fputs("  build              Compile a TI-BASIC++ program\n", output);
  fputs("  send               Send a compiled .8xp program to a calculator\n", output);
  fputs("  buildsend          Compile a TI-BASIC++ program and send it to a calculator\n", output);
  fputs("  help               Print this help message, then exit\n", output);
  fputs("  version            Print the compiler version, then exit\n", output);
  // clang-format on

  fprintf(output, "\nOptions:\n");

  static const size_t MAX_HELP_OFFSET = 21;

  const option_t* option = driver_options;
  for (size_t i = 0; i < driver_options_count; ++i, ++option) {
    if (option->help == NULL)
      continue;

    assert(option->names[0] != NULL);

    size_t help_offset = 2 + 1 + strlen(option->names[0]);

    fprintf(output, "  -%s", option->names[0]);

    if (option->has_value) {
      if (option->value_name != NULL) {
        fprintf(output, " %s", option->value_name);
        help_offset += 1 + strlen(option->value_name);
      } else {
        fprintf(output, " <value>");
        help_offset += 8;
      }
    }

    if (help_offset >= MAX_HELP_OFFSET) {
      fputc('\n', output);
      help_offset = 0;
    }

    while (help_offset < MAX_HELP_OFFSET) {
      fputc(' ', output);
      ++help_offset;
    }

    fprintf(output, "%s\n", option->help);
  }
}

static void print_version(void) {
  printf("ti-basic++ version: HACKATHON!!!!\n");
}

