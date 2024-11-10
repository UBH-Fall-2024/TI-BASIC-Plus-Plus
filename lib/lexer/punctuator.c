#include <ti-basic-plus-plus/lexer/punctuator.h>

#include <assert.h>
#include <stdlib.h>

const char* punct_to_string(punctuator_kind_t kind) {
  switch (kind) {
    case PUNCT_UNKNOWN:
      return "unknown";
    case PUNCT_LPAREN:    // (
      return "(";
    case PUNCT_RPAREN:    // )
      return ")";
    case PUNCT_LBRACE:    // {
      return "{";
    case PUNCT_RBRACE:    // }
      return "}";
    case PUNCT_LBRACKET:  // [
      return "[";
    case PUNCT_RBRACKET:  // ]
      return "]";
    case PUNCT_ADD:       // +
      return "+";
    case PUNCT_SUB:       // -
      return "-";
    case PUNCT_MUL:       // *
      return "*";
    case PUNCT_DIV:       // /
      return "/";
    case PUNCT_MOD:       // %
      return "%";
    case PUNCT_ASGN:      // =
      return "=";
    case PUNCT_CMP_LT:    // <
      return "<";
    case PUNCT_CMP_GT:    // >
      return ">";
    case PUNCT_ASGN_ADD:  // +=
      return "+=";
    case PUNCT_ASGN_SUB:  // -=
      return "-=";
    case PUNCT_ASGN_MUL:  // *=
      return "*=";
    case PUNCT_ASGN_DIV:  // /=
      return "/=";
    case PUNCT_ASGN_MOD:  // %=
      return "%=";
    case PUNCT_CMP_EQ:    // ==
      return "==";
    case PUNCT_CMP_NE:    // !=
      return "!=";
    case PUNCT_CMP_LE:    // <=
      return "<=";
    case PUNCT_CMP_GE:    // >=
      return ">=";
    default:
      assert(false);
  }

  return NULL;
}
