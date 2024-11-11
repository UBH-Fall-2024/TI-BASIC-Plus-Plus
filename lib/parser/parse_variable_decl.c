#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>

#include "parser_internal.h"

ast_node_t* parse_variable_decl(token_t** t,
                                variable_type_t type,
                                const char* name,
                                source_range_t location,
                                diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  bool has_variable = false;
  variable_t variable;
  ast_node_t* expr = NULL;

  token_kind_t token_kind;
  punctuator_kind_t punctuator_kind =
      comp_punctuator_kind(*t, 2, PUNCT_ASGN, PUNCT_COLON);

CHECK_ASGN_PUNCT:
  token_kind = comp_token_kind(*t, 3, TOKEN_PUNCTUATOR, TOKEN_NEWLINE, TOKEN_EOF);
  if (token_kind == TOKEN_UNKNOWN) {
    unexpected_token(*t, TOKEN_UNKNOWN, d);
    return NULL;
  }

  if (token_kind == TOKEN_NEWLINE || token_kind == TOKEN_EOF) {
    // <type> <name>
    goto VARIABLE_END;
  }

  if (punctuator_kind == PUNCT_UNKNOWN) {
    unexpected_token(*t, TOKEN_PUNCTUATOR, d);
    return NULL;
  }

  TOK_ITER(t);

  if (punctuator_kind == PUNCT_COLON) {
    // <type> <name> : <variable>
    assert(!has_variable);

    if ((*t)->kind != TOKEN_IDENTIFIER) {
      unexpected_token(*t, TOKEN_IDENTIFIER, d);
      return NULL;
    }

    if (!match_variable((*t)->data.string, arrlenu((*t)->data.string),
                        &variable)) {
      diag_report_source(d, ERROR, &(*t)->location, "invalid variable name");
      return NULL;
    }

    if (variable.type != type) {
      diag_report_source(d, ERROR, &(*t)->location, "variable type mismatch");
      return NULL;
    }

    location = range_cat(&location, &(*t)->location);

    has_variable = true;
    TOK_ITER(t);
    punctuator_kind = comp_punctuator_kind(*t, 1, PUNCT_ASGN);

    goto CHECK_ASGN_PUNCT;
  }

  // <type> <name> = <expr>
  // <type> <name> : <variable> = <expr>

  expr = parse_expression(t, d);
  if (expr == NULL) {
    return NULL;
  }
  location = range_cat(&location, &expr->location);

  token_kind =
      comp_token_kind(*t, 2, TOKEN_NEWLINE, TOKEN_EOF);
  if (token_kind == TOKEN_UNKNOWN) {
    unexpected_token(*t, TOKEN_UNKNOWN, d);
    if (expr != NULL) {
      ast_node_destroy(expr);
    }
    return NULL;
  }

VARIABLE_END:
  return ast_node_create_variable_decl(type, name, has_variable, variable, expr,
                                       location);
}

