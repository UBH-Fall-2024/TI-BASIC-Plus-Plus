#ifndef KEYWORD_H
#define KEYWORD_H

#include <stddef.h>

typedef enum keyword_kind {
  KW_UNKNOWN = 0,

  KW_NUMBER,
  KW_STRING,
  KW_MATRIX,
  KW_LIST,

  KW_IF,
  KW_ELIF,
  KW_ELSE,

  KW_WHILE,
  KW_RETURN,
  KW_GOTO,
} keyword_kind_t;

keyword_kind_t keyword_match(const char* string, size_t length);
const char* keyword_to_string(keyword_kind_t kind);

#endif // KEYWORD_H
