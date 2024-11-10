#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>
#include <ti-basic-plus-plus/calculator/variable.h>

// <type> <identifier> ...
ast_node_t* parse_variable_or_function_decl(token_t** t,
                                            keyword_kind_t type,
                                            diagnostics_t* d);
// reserve <variable>
ast_node_t* parse_reserve_decl(token_t** t, diagnostics_t* d);

ast_node_t* parse_global_decl(token_t** t, diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  ast_node_t* node = NULL;

  while (*t != NULL) {
    token_kind_t token_kind =
        comp_token_kind(*t, 2, TOKEN_NEWLINE, TOKEN_KEYWORD);
    if (token_kind == TOKEN_UNKNOWN) {
      unexpected_token(*t, TOKEN_UNKNOWN, d);
      return NULL;
    }
    if (token_kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
      continue;
    }

    keyword_kind_t keyword_kind = comp_keyword_kind(
        *t, 6, KW_VOID, KW_NUMBER, KW_STRING, KW_MATRIX, KW_LIST, KW_RESERVE);
    if (keyword_kind == KW_UNKNOWN) {
      unexpected_token(*t, TOKEN_KEYWORD, d);
      return NULL;
    }

    switch (keyword_kind) {
      // Types, could be a variable decl or a function decl
      case KW_VOID:
      case KW_NUMBER:
      case KW_STRING:
      case KW_MATRIX:
      case KW_LIST:
        return parse_variable_or_function_decl(t, keyword_kind, d);
      case KW_RESERVE:
        return parse_reserve_decl(t, d);
      default:
        assert(false);
    }
  }

  return node;
}

ast_node_t* parse_variable_or_function_decl(token_t** t,
                                            keyword_kind_t type,
                                            diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  (void)type;

  /* ast_node_t* node = NULL; */

  return NULL;
}

ast_node_t* parse_reserve_decl(token_t** t, diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  source_range_t start_location = (*t)->location;

  // reverve
  TOK_ITER(t);

  // <variable>
  if ((*t)->kind != TOKEN_IDENTIFIER) {
    unexpected_token(*t, TOKEN_IDENTIFIER, d);
    return NULL;
  }

  variable_t variable;
  if (!match_variable((*t)->data.string, arrlenu((*t)->data.string),
                      &variable)) {
    diag_report_source(d, ERROR, &(*t)->location, "invalid variable name");
    return NULL;
  }

  source_range_t end_location = (*t)->location;

  TOK_ITER(t);

  return ast_node_create_reservation(variable,
                                     range_cat(&start_location, &end_location));
}

