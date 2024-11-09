#include <stdio.h>

#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <driver/options.h>

#include <assert.h>
#include <stb_ds.h>
#include <stdio.h>

static diagnostics_t d;

static void compile(void);

int main(int argc, const char** argv) {
  d = diagnostics_create();

  do {
    parse_arguments(argc, argv, &d);
    if (should_exit(&d)) {
      break;
    }

    if (driver_config.build) {
      compile();
      if (should_exit(&d)) {
        break;
      }
    }

    if (driver_config.send) {
      // TODO: Send to calculator.
    }
  } while (false);

  return get_exit_status(&d);
}

static void compile(void) {
  // Lexing

  // Parsing

  // Semantic analysis

  // (Optional) Encoding
}
