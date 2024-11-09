#include <stdio.h>

#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <driver/options.h>

#include <assert.h>
#include <stb_ds.h>
#include <stdio.h>

int main(int argc, const char** argv) {
  diagnostics_t d = diagnostics_create();

  do {
    parse_arguments(argc, argv, &d);
    if (should_exit(&d)) {
      break;
    }

    // TODO: Validate arguments.

    /* compile(&d); */
    if (should_exit(&d)) {
      break;
    }

    // TODO: Serialization, sending, etc.
  } while (false);

  return get_exit_status(&d);
}
