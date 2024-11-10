#include <ti-basic-plus-plus/lexer/keyword.h>

#include <assert.h>
#include <string.h>

static const char* keyword_strings[] = {
    [KW_UNKNOWN] = NULL,    [KW_NUMBER] = "Number", [KW_STRING] = "String",
    [KW_MATRIX] = "Matrix", [KW_LIST] = "List",     [KW_IF] = "if",
    [KW_ELIF] = "elif",     [KW_ELSE] = "else",     [KW_WHILE] = "while",
    [KW_RETURN] = "return", [KW_GOTO] = "goto",
};

static const size_t keyword_string_count =
    sizeof(keyword_strings) / sizeof(keyword_strings[0]);

keyword_kind_t keyword_match(const char* string, size_t length) {
  assert(string != NULL);

  for (size_t i = 1; i < keyword_string_count; i++) {
    if (length != strlen(keyword_strings[i])) {
      continue;
    }

    if (strncmp(string, keyword_strings[i], length) == 0) {
      return (keyword_kind_t)i;
    }
  }

  return KW_UNKNOWN;
}

const char* keyword_to_string(keyword_kind_t kind) {
  if (kind == KW_UNKNOWN) {
    return "unknown";
  }

  return keyword_strings[kind];
}

