#ifndef VARIABLE_H
#define VARIABLE_H

#include <stdbool.h>
#include <ti-basic-plus-plus/lexer/keyword.h>

typedef enum variable_type {
  VAR_UNKNOWN = 0,

  VAR_VOID,
  VAR_NUMBER,
  VAR_STRING,
  VAR_MATRIX,
  VAR_LIST,
  VAR_LIST_ELEMENT,
} variable_type_t;

variable_type_t keyword_to_variable_type(keyword_kind_t kind);
const char* variable_type_to_string(variable_type_t type);

typedef struct variable {
  variable_type_t type;
  char id;
} variable_t;

bool match_variable(const char* string,
                    size_t length,
                    variable_t* var);

#endif  // VARIABLE_H

