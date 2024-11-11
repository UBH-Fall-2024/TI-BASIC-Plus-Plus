#include <ti-basic-plus-plus/calculator/builtin_functions.h>

#include <string.h>

builtin_function_t match_builtin_function(const char* string, size_t length) {
  if (length == 0) {
    return FUNC_UNKNOWN;
  }

  if (length == 4 && strncmp(string, "Disp", 4) == 0) {
    return FUNC_DISP;
  }
  else if (length == 4 && strncmp(string, "Line", 4) == 0) {
    return FUNC_LINE;
  }
  else if (length == 6 && strncmp(string, "Circle", 6) == 0) {
    return FUNC_CIRCLE;
  }
  else if (length == 4 && strncmp(string, "Text", 4) == 0) {
    return FUNC_TEXT;
  }
  else if (length == 7 && strncmp(string, "ClrDraw", 7) == 0) {
    return FUNC_CLRDRAW;
  }

  return FUNC_UNKNOWN;
}

void print_builtin_function(builtin_function_t func, FILE* file) {
  switch (func) {
    case FUNC_UNKNOWN:
      break;
    case FUNC_CLRDRAW:
      fprintf(file, "ClrDraw");
      break;
    case FUNC_DISP:
      fprintf(file, "Disp ");
      break;
    case FUNC_LINE:
      fprintf(file, "Line(");
      break;
    case FUNC_CIRCLE:
      fprintf(file, "Circle(");
      break;
    case FUNC_TEXT:
      fprintf(file, "Text(");
      break;
    default:
      break;
  }
}
