#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>

#include "parser_internal.h"

// <type> <identifier> ...
static ast_node_t* parse_variable_or_function_decl(
    token_t** t,
    variable_type_t type,
    source_range_t start_location,
    diagnostics_t* d);
// reserve <variable>
static ast_node_t* parse_reserve_decl(token_t** t,
                                      source_range_t start_location,
                                      diagnostics_t* d);

ast_node_t* parse_global_decl(token_t** t, diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  while (*t != NULL) {
    token_kind_t token_kind =
        comp_token_kind(*t, 3, TOKEN_NEWLINE, TOKEN_KEYWORD, TOKEN_EOF);
    if (token_kind == TOKEN_UNKNOWN) {
      unexpected_token(*t, TOKEN_UNKNOWN, d);
      return NULL;
    }
    if (token_kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
      continue;
    }
    if (token_kind == TOKEN_EOF) {
      return NULL;
    }

    keyword_kind_t keyword_kind = comp_keyword_kind(
        *t, 6, KW_VOID, KW_NUMBER, KW_STRING, KW_MATRIX, KW_LIST, KW_RESERVE);
    if (keyword_kind == KW_UNKNOWN) {
      unexpected_token(*t, TOKEN_KEYWORD, d);
      return NULL;
    }

    source_range_t start_location = (*t)->location;

    TOK_ITER(t);

    switch (keyword_kind) {
      // Types, could be a variable decl or a function decl
      case KW_VOID:
      case KW_NUMBER:
      case KW_STRING:
      case KW_MATRIX:
      case KW_LIST:
        return parse_variable_or_function_decl(
            t, keyword_to_variable_type(keyword_kind), start_location, d);
      case KW_RESERVE:
        return parse_reserve_decl(t, start_location, d);
      default:
        assert(false);
    }
  }

  return NULL;
}

static ast_node_t* parse_variable_or_function_decl(token_t** t,
                                                   variable_type_t type,
                                                   source_range_t location,
                                                   diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  source_range_t start_location = location;

  // <name>
  if ((*t)->kind != TOKEN_IDENTIFIER) {
    unexpected_token(*t, TOKEN_IDENTIFIER, d);
    return NULL;
  }

  const char* name = (*t)->data.string;
  location = range_cat(&location, &(*t)->location);

  TOK_ITER(t);

  token_kind_t token_kind =
      comp_token_kind(*t, 2, TOKEN_PUNCTUATOR, TOKEN_NEWLINE);
  if (token_kind == TOKEN_UNKNOWN) {
    unexpected_token(*t, TOKEN_UNKNOWN, d);
    return NULL;
  }

  if (token_kind == TOKEN_NEWLINE) {
    goto PARSE_VARIABLE;
  }

  punctuator_kind_t punct_kind =
      comp_punctuator_kind(*t, 3, PUNCT_LPAREN, PUNCT_COLON, PUNCT_ASGN);
  if (punct_kind == PUNCT_UNKNOWN) {
    unexpected_token(*t, TOKEN_PUNCTUATOR, d);
    return NULL;
  }

  if (punct_kind == PUNCT_LPAREN) {
    TOK_ITER(t);
    return parse_function_decl(t, type, name, start_location, d);
  }

PARSE_VARIABLE:
  if (type == VAR_VOID) {
    diag_report_source(d, ERROR, &start_location,
                       "variables of type 'void' are not allowed");
    return NULL;
  }

  return parse_variable_decl(t, type, name, location, d);
}

static ast_node_t* parse_reserve_decl(token_t** t,
                                      source_range_t start_location,
                                      diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

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

  token_kind_t token_kind =
      comp_token_kind(*t, 2, TOKEN_NEWLINE, TOKEN_EOF);
  if (token_kind == TOKEN_UNKNOWN) {
    unexpected_token(*t, TOKEN_UNKNOWN, d);
    return NULL;
  }

  return ast_node_create_reservation(variable,
                                     range_cat(&start_location, &end_location));
}

