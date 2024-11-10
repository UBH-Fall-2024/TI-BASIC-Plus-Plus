#ifndef BUILTIN_FUNCTIONS_H
#define BUILTIN_FUNCTIONS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef enum builtin_function {
  FUNC_UNKNOWN = 0,
  FUNC_DISP,
  FUNC_LINE,
  FUNC_CIRCLE,
  FUNC_TEXT,
  FUNC_CLRDRAW,
} builtin_function_t;

builtin_function_t match_builtin_function(const char* string, size_t length);

void print_builtin_function(builtin_function_t func, FILE* file);

#endif // BUILTIN_FUNCTIONS_H
