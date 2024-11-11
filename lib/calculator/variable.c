#include <ti-basic-plus-plus/calculator/variable.h>

#include <assert.h>
#include <string.h>

variable_type_t keyword_to_variable_type(keyword_kind_t kind) {
  switch (kind) {
    case KW_VOID:
      return VAR_VOID;
    case KW_NUMBER:
      return VAR_NUMBER;
    case KW_STRING:
      return VAR_STRING;
    case KW_MATRIX:
      return VAR_MATRIX;
    case KW_LIST:
      return VAR_LIST;
    default:
      return VAR_UNKNOWN;
  }
}

const char* variable_type_to_string(variable_type_t type) {
  switch (type) {
    case VAR_UNKNOWN:
      return "unknown";
    case VAR_VOID:
      return "void";
    case VAR_NUMBER:
      return "Number";
    case VAR_STRING:
      return "String";
    case VAR_MATRIX:
      return "Matrix";
    case VAR_LIST:
      return "List";
    default:
      assert(false);
  }
  return NULL;
}

bool match_variable(const char* string,
                    size_t length,
                    variable_t* var) {
  if (length == 0) {
    return false;
  }

  // List
  if (string[0] == 'L' && length == 2) {
    char c = string[1];
    if (c < '0' || c > '9') {
      return false;
    }

    var->type = VAR_LIST;
    var->id = c;
    return true;
  }
  // String
  if (strncmp(string, "String", 6) == 0) {
    if (length != 7) {
      return false;
    }

    string += 6;

    char c = string[0];
    if (c < '0' || c > '9') {
      return false;
    }

    var->type = VAR_STRING;
    var->id = c;
    return true;
  }
  // Matrix
  if (strncmp(string, "Matrix", 6) == 0) {
    if (length != 7) {
      return false;
    }

    string += 6;

    char c = string[0];
    if (c < 'A' || c > 'J') {
      return false;
    }

    var->type = VAR_MATRIX;
    var->id = c;
    return true;
  }

  // Theta
  if (length == 5) {
    if (strncmp(string, "Theta", 5) != 0) {
      return false;
    }

    var->type = VAR_NUMBER;
    var->id = 't';
    return true;
  }

  // A-Z Number
  if (length != 1) {
    return false;
  }

  char c = string[0];
  if (c < 'A' || c > 'Z') {
    return false;
  }
  var->type = VAR_NUMBER;
  var->id = c;
  return true;
}

