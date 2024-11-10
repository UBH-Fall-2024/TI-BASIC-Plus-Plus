#include <ti-basic-plus-plus/calculator/builtin_functions.h>

#include <string.h>

builtin_function_t match_builtin_function(const char* string, size_t length) {
  if (length == 0) {
    return FUNC_UNKNOWN;
  }

  if (length == 4 && strncmp(string, "Disp", 4) == 0) {
    return FUNC_DISP;
  }

  return FUNC_UNKNOWN;
}

void print_builtin_function(builtin_function_t func, FILE* file) {
  switch (func) {
    case FUNC_UNKNOWN:
      break;
    case FUNC_DISP:
      fprintf(file, "Disp ");
      break;
    default:
      break;
  }
}
