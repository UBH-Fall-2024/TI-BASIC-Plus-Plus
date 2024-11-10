#ifndef OPERATOR_H
#define OPERATOR_H

#include <ti-basic-plus-plus/lexer/punctuator.h>
#include <ti-basic-plus-plus/lexer/keyword.h>

typedef enum operator_kind {
  OP_UNKNOWN = 0,
  OP_ADD,  // +
  OP_SUB,  // -
  OP_MUL,  // *
  OP_DIV,  // /
  /* OP_MOD,       // % */
  OP_ASGN,   // =
  OP_LT,     // <
  OP_GT,     // >
  OP_LE,     // <=
  OP_GE,     // >=
  OP_EQ,     // ==
  OP_NE,     // !=
  OP_AND,    // and
  OP_OR,     // or
  OP_NOT,    // not
  OP_POS,    // +
  OP_NEG,    // -
  OP_ARRAY,  // []
} operator_kind_t;

const char* op_to_string(operator_kind_t kind);

operator_kind_t op_from_punctuator(punctuator_kind_t punctuator);
operator_kind_t op_from_keyword(keyword_kind_t keyword);

int op_get_precedence(operator_kind_t op);

#endif  // OPERATOR_H

