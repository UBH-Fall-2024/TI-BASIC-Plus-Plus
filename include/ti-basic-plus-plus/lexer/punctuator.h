#ifndef PUNCTUATOR_H
#define PUNCTUATOR_H

#include <stdbool.h>

typedef enum punctuator_kind {
  PUNCT_UNKNOWN = 0,
  PUNCT_LPAREN,    // (
  PUNCT_RPAREN,    // )
  PUNCT_LBRACE,    // {
  PUNCT_RBRACE,    // }
  PUNCT_LBRACKET,  // [
  PUNCT_RBRACKET,  // ]
  PUNCT_COLON,     // :
  PUNCT_DOT,       // .
  PUNCT_COMMA,     // ,
  PUNCT_ADD,       // +
  PUNCT_SUB,       // -
  PUNCT_MUL,       // *
  PUNCT_DIV,       // /
  /* PUNCT_MOD,       // % */
  PUNCT_ASGN,      // =
  PUNCT_CMP_LT,    // <
  PUNCT_CMP_GT,    // >
  // 2-character punctuators start here
  /* PUNCT_ASGN_ADD,  // += */
  /* PUNCT_ASGN_SUB,  // -= */
  /* PUNCT_ASGN_MUL,  // *= */
  /* PUNCT_ASGN_DIV,  // /= */
  /* PUNCT_ASGN_MOD,  // %= */
  PUNCT_CMP_LE,    // <=
  PUNCT_CMP_GE,    // >=
  PUNCT_CMP_EQ,    // ==
  PUNCT_CMP_NE,    // !=
} punctuator_kind_t;

const char* punct_to_string(punctuator_kind_t kind);

#endif  // PUNCTUATOR_H

