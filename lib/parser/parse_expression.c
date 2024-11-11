#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>
#include <ti-basic-plus-plus/parser/operator.h>

#include "parser_internal.h"

typedef enum expr_fragment_type {
  UNKNOWN,
  UNIT,
  OPERATOR,
} expr_fragment_type_t;

typedef struct expr_fragment {
  expr_fragment_type_t type;

  union {
    ast_node_t* unit;
    operator_kind_t operator;
  } data;
} expr_fragment_t;

static ast_node_t* parse_unit(token_t** t, diagnostics_t* d);
static ast_node_t* fragments_to_expression(expr_fragment_t** fragments);

ast_node_t* parse_expression(token_t** t, diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  expr_fragment_t* fragments = NULL;
  expr_fragment_type_t expected = UNIT;

  while (true) {
    if (expected == UNIT) {
      ast_node_t* unit = parse_unit(t, d);
      if (unit == NULL) {
        goto CLEANUP;
      }

      expr_fragment_t fragment = {.type = UNIT, .data.unit = unit};
      arrput(fragments, fragment);

      expected = OPERATOR;
    } else if (expected == OPERATOR) {
      punctuator_kind_t op_punct = comp_punctuator_kind(
          *t, 7, PUNCT_ADD, PUNCT_SUB, PUNCT_MUL, PUNCT_DIV, PUNCT_ASGN,
          PUNCT_CMP_LT, PUNCT_CMP_GT);
      keyword_kind_t op_kw = comp_keyword_kind(*t, 2, KW_AND, KW_OR);
      if (op_punct == PUNCT_UNKNOWN && op_kw == KW_UNKNOWN) {
        expected = UNKNOWN;
        break;
      }

      TOK_ITER(t);

      operator_kind_t op;
      if (op_punct != PUNCT_UNKNOWN) {
        op = op_from_punctuator(op_punct);
      } else {
        op = op_from_keyword(op_kw);
      }

      expr_fragment_t fragment = {.type = OPERATOR, .data.operator= op };
      arrput(fragments, fragment);

      expected = UNIT;
    }
  }

  if (expected != UNKNOWN || !arrlen(fragments)) {
    arrfree(fragments);
    return NULL;
  }

  ast_node_t* expr = fragments_to_expression(&fragments);
  arrfree(fragments);
  return expr;

CLEANUP:
  for (size_t i = 0; i < arrlenu(fragments); i++) {
    if (fragments[i].type == UNIT) {
      ast_node_destroy(fragments[i].data.unit);
    }
  }
  arrfree(fragments);

  return NULL;
}

static ast_node_t* parse_function_call(token_t** t,
                                       const char* name,
                                       source_range_t start_location,
                                       diagnostics_t* d);

static void parse_prefix_operators(token_t** t, operator_kind_t** ops) {
  while (true) {
    if ((*t)->kind == TOKEN_KEYWORD && (*t)->data.keyword == KW_NOT) {
      arrput(*ops, OP_NOT);
      TOK_ITER(t);
      continue;
    }
    punctuator_kind_t punct_kind =
        comp_punctuator_kind(*t, 2, PUNCT_ADD, PUNCT_SUB);
    if (punct_kind == PUNCT_UNKNOWN) {
      break;
    }
    TOK_ITER(t);

    operator_kind_t op = OP_POS;
    if (punct_kind == PUNCT_SUB) {
      op = OP_NEG;
    }
    arrput(*ops, op);
  }
}

static ast_node_t* parse_unit(token_t** t, diagnostics_t* d) {
  ast_node_t* unit = NULL;

  operator_kind_t* prefix_ops = NULL;
  /* operator_kind_t* suffix_ops = NULL; // TODO: */

  do {
    parse_prefix_operators(t, &prefix_ops);

    token_kind_t token_kind =
        comp_token_kind(*t, 4, TOKEN_IDENTIFIER, TOKEN_NUMBER_LITERAL,
                        TOKEN_STRING_LITERAL, TOKEN_PUNCTUATOR);
    if (token_kind == TOKEN_PUNCTUATOR &&
        comp_punctuator_kind(*t, 1, PUNCT_LPAREN) == PUNCT_UNKNOWN) {
      token_kind = TOKEN_UNKNOWN;
    }

    if (token_kind == TOKEN_UNKNOWN) {
      unexpected_token(*t, TOKEN_UNKNOWN, d);
      break;
    } else if (token_kind == TOKEN_IDENTIFIER) {
      const char* name = (*t)->data.string;
      source_range_t name_location = (*t)->location;
      TOK_ITER(t);

      if (comp_punctuator_kind(*t, 1, PUNCT_LPAREN) != PUNCT_UNKNOWN) {
        TOK_ITER(t);
        unit = parse_function_call(t, name, name_location, d);
        if (unit == NULL) {
          break;
        }
      } else {
        unit = ast_node_create(AST_IDENTIFIER);
        unit->data.identifier = name;
        unit->location = name_location;
      }
    } else if (token_kind == TOKEN_NUMBER_LITERAL) {
      unit = ast_node_create(AST_NUMERIC_LITERAL);
      unit->location = (*t)->location;
      unit->data.numeric_literal = (*t)->data.number;
      TOK_ITER(t);
    } else if (token_kind == TOKEN_STRING_LITERAL) {
      unit = ast_node_create(AST_STRING_LITERAL);
      unit->location = (*t)->location;
      unit->data.string_literal = (*t)->data.string;
      TOK_ITER(t);
    }
    // Parenthesized expression.
    else if (token_kind == TOKEN_PUNCTUATOR) {
      TOK_ITER(t);
      unit = parse_expression(t, d);
      if (unit == NULL) {
        break;
      }

      if (comp_punctuator_kind(*t, 1, PUNCT_RPAREN) == PUNCT_UNKNOWN) {
        unexpected_token(*t, TOKEN_PUNCTUATOR, d);
        break;
      }

      TOK_ITER(t);
    }

    if (unit == NULL) {
      break;
    }

    if (prefix_ops) {
      for (int i = arrlenu(prefix_ops) - 1; i >= 0; i--) {
        unit = ast_node_create_unary_expr(unit, prefix_ops[i], unit->location);
      }
    }

  } while (false);

  return unit;
}

static ast_node_t** parse_argument_list(token_t** t, diagnostics_t* d) {
  if ((*t)->kind == TOKEN_PUNCTUATOR && (*t)->data.punctuator == PUNCT_RPAREN) {
    TOK_ITER(t);
    return NULL;
  }

  ast_node_t** list = NULL;

  while (true) {
    ast_node_t* expr = parse_expression(t, d);
    if (expr == NULL) {
      arrfree(list);
      return NULL;
    }

    arrput(list, expr);

    punctuator_kind_t punct_kind =
        comp_punctuator_kind(*t, 2, PUNCT_COMMA, PUNCT_RPAREN);
    if (punct_kind == PUNCT_UNKNOWN) {
      unexpected_token(*t, TOKEN_PUNCTUATOR, d);
      arrfree(list);
      return NULL;
    }

    TOK_ITER(t);
    if (punct_kind == PUNCT_RPAREN) {
      break;
    }
  }

  return list;
}

static ast_node_t* parse_function_call(token_t** t,
                                       const char* name,
                                       source_range_t start_location,
                                       diagnostics_t* d) {
  source_range_t location = start_location;

  ast_node_t** arguments = parse_argument_list(t, d);
  if (should_exit(d)) {
    arrfree(arguments);
    return NULL;
  }

  location = range_cat(&location, &(*t)->prev->location);

  return ast_node_create_function_call(name, arguments, location);
}

static ast_node_t* fragments_to_expression(expr_fragment_t** fragments) {
  size_t num_fragments = arrlenu(*fragments);

  if (num_fragments == 1) {
    assert((*fragments)[0].type == UNIT);
    return (*fragments)[0].data.unit;
  }

  int highest_precedence = 0;
  size_t highest_precedence_index = 0;

  for (size_t i = 1; i < num_fragments; i += 2) {
    assert((*fragments)[i].type == OPERATOR);
    int precedence = op_get_precedence((*fragments)[i].data.operator);
    if (precedence > highest_precedence) {
      highest_precedence = precedence;
      highest_precedence_index = i;
    }
  }

  ast_node_t* left = (*fragments)[highest_precedence_index - 1].data.unit;
  operator_kind_t op = (*fragments)[highest_precedence_index].data.operator;
  ast_node_t* right = (*fragments)[highest_precedence_index + 1].data.unit;

  source_range_t location = range_cat(&left->location, &right->location);

  ast_node_t* unit = ast_node_create_binary_expr(left, op, right, location);

  if (num_fragments == 3) {
    return unit;
  }

  (*fragments)[highest_precedence_index - 1].data.unit = unit;
  arrdeln(*fragments, highest_precedence_index, 2);

  return fragments_to_expression(fragments);
}

