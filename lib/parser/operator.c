#include <ti-basic-plus-plus/parser/operator.h>

#include <assert.h>

const char* op_to_string(operator_kind_t kind) {
  switch (kind) {
  case OP_ADD:
    return "add";
  case OP_SUB:
    return "subtract";
  case OP_MUL:
    return "multiply";
  case OP_DIV:
    return "divide";
  /* case OP_MOD: */
  /*   return "modulo"; */
  case OP_ASGN:
    return "assign";
  case OP_LT:
    return "less than";
  case OP_GT:
    return "greater than";
  case OP_LE:
    return "less than or equal";
  case OP_GE:
    return "greater than or equal";
  case OP_EQ:
    return "equal";
  case OP_NE:
    return "not equal";
  case OP_AND:
    return "and";
  case OP_OR:
    return "or";
  case OP_NOT:
    return "not";
  case OP_POS:
    return "positive";
  case OP_NEG:
    return "negative";
  case OP_ARRAY:
    return "array access";
  default:
    assert(false);
    return NULL;
  }
}

operator_kind_t op_from_punctuator(punctuator_kind_t punctuator) {
  switch (punctuator) {
    case PUNCT_ADD:
      return OP_ADD;
    case PUNCT_SUB:
      return OP_SUB;
    case PUNCT_MUL:
      return OP_MUL;
    case PUNCT_DIV:
      return OP_DIV;
  /* case PUNCT_MOD: */
  /*     return OP_MOD; */
    case PUNCT_ASGN:
      return OP_ASGN;
    case PUNCT_CMP_LT:
      return OP_LT;
    case PUNCT_CMP_GT:
      return OP_GT;
    case PUNCT_CMP_LE:
      return OP_LE;
    case PUNCT_CMP_GE:
      return OP_GE;
    case PUNCT_CMP_EQ:
      return OP_EQ;
    case PUNCT_CMP_NE:
      return OP_NE;
    default:
      return OP_UNKNOWN;
  }
}

operator_kind_t op_from_keyword(keyword_kind_t keyword) {
  switch (keyword) {
  case KW_AND:
    return OP_AND;
  case KW_OR:
    return OP_OR;
  case KW_NOT:
    return OP_NOT;
  default:
    return OP_UNKNOWN;
  }
}

#define MAX_PREC_LVL 12
#define MIN_PREC_LVL 0

int op_get_precedence(operator_kind_t op) {
  switch (op) {
  case OP_ARRAY:
    return MAX_PREC_LVL;
  case OP_NOT:
  case OP_POS:
  case OP_NEG:
    return MAX_PREC_LVL - 1;
  case OP_MUL:
  case OP_DIV:
  /* case OP_MOD: */
    return MAX_PREC_LVL - 2;
  case OP_ADD:
  case OP_SUB:
    return MAX_PREC_LVL - 3;
  case OP_LT:
  case OP_GT:
  case OP_LE:
  case OP_GE:
    return MAX_PREC_LVL - 5;
  case OP_EQ:
  case OP_NE:
    return MAX_PREC_LVL - 6;
  case OP_AND:
    return MAX_PREC_LVL - 10;
  case OP_OR:
    return MAX_PREC_LVL - 11;
  case OP_ASGN:
    return MAX_PREC_LVL - 12;
  default:
    assert(false);
    return MAX_PREC_LVL;
  }
}
