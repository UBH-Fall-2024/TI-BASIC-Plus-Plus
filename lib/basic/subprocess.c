#include <ti-basic-plus-plus/basic/subprocess.h>

#include <assert.h>
#include <stb_ds.h>
#include <unistd.h>

static char* make_command(const char** argv) {
  const size_t argc = arrlenu(argv);

  if (argc == 0) {
    return NULL;
  }

  char* command = NULL;
  for (size_t i = 0; i < argc; i++) {
    const char* arg = argv[i];
    size_t arg_length = strlen(arg);

    char* arg_start = arraddnptr(command, arg_length + 2 + 1);
    if (i > 0) {
      arg_start[-1] = ' ';
    }
    (arg_start++)[0] = '"';

    memcpy(arg_start, arg, arg_length);
    arg_start += arg_length;

    arg_start[0] = '"';
    arg_start[1] = '\0';
  }

  // Redirect stderr to stdout.
  {
    char* start = arraddnptr(command, 5);
    memcpy(start - 1, " 2>&1", 5);
    start[4] = '\0';
  }

  return command;
}

bool begin_subprocess(const char** argv, subprocess_t* subprocess) {
  assert(argv != NULL);
  assert(subprocess != NULL);

  char* command = make_command(argv);
  if (command == NULL) {
    return false;
  }

  FILE* stream = popen(command, "r");

  arrfree(command);

  if (stream == NULL) {
    return false;
  }

  subprocess->stream = stream;
  return true;
}

bool end_subprocess(subprocess_t* subprocess, bool print_output) {
  assert(subprocess != NULL);

  if (print_output) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), subprocess->stream) != NULL) {
      printf("%s", buffer);
    }
  }

  int status = pclose(subprocess->stream);
  if (status == -1) {
    return false;
  }

  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

