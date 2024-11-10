#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>

#include "parser_internal.h"

static ast_node_t* parse_parameter_list(token_t** t,
                                        diagnostics_t* d);

ast_node_t* parse_function_decl(token_t** t,
                                variable_type_t return_type,
                                const char* name,
                                source_range_t location,
                                diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  ast_node_t* parameters = parse_parameter_list(t, d);
  if (should_exit(d)) {
    return NULL;
  }

  if ((*t)->kind == TOKEN_NEWLINE) {
    TOK_ITER(t);
  }

  if (comp_punctuator_kind(*t, 1, PUNCT_LBRACE) != PUNCT_LBRACE) {
    unexpected_token(*t, TOKEN_PUNCTUATOR, d);
    return NULL;
  }

  source_range_t body_location = (*t)->location;
  location = range_cat(&location, &body_location);

  TOK_ITER(t);

  ast_node_t* body = parse_block(t, body_location, d);
  if (body == NULL) {
    if (parameters != NULL) {
      ast_node_destroy(parameters);
    }
    return NULL;
  }

  location = range_cat(&location, &(*t)->prev->location);

  return ast_node_create_function_decl(return_type, name, parameters, body, location);
}

static ast_node_t* parse_parameter_list(token_t** t,
    diagnostics_t* d) {

  source_range_t list_location = (*t)->prev->location;

  token_kind_t token_kind = comp_token_kind(*t, 2, TOKEN_KEYWORD, TOKEN_PUNCTUATOR);
  if (token_kind == TOKEN_UNKNOWN) {
    unexpected_token(*t, TOKEN_UNKNOWN, d);
    return NULL;
  }
  if (token_kind == TOKEN_PUNCTUATOR) {
    if ((*t)->data.punctuator != PUNCT_RPAREN) {
      unexpected_token(*t, TOKEN_PUNCTUATOR, d);
      return NULL;
    }
    TOK_ITER(t);
    return NULL;
  }

  ast_node_t* list = ast_node_create(AST_PARAMETER_LIST);

  while (true) {
    source_range_t param_location = (*t)->location;

    variable_type_t type = parse_variable_type(t, false, d);
    if (!type) {
      return NULL;
    }

    if ((*t)->kind != TOKEN_IDENTIFIER) {
      unexpected_token(*t, TOKEN_IDENTIFIER, d);
      return NULL;
    }

    const char* name = (*t)->data.string;
    param_location = range_cat(&param_location, &(*t)->location);

    TOK_ITER(t);

    ast_node_t* param = ast_node_create_param(type, name, param_location);
    arrput(list->children, param);

    punctuator_kind_t punct_kind = comp_punctuator_kind(*t, 2, PUNCT_COMMA, PUNCT_RPAREN);
    if (punct_kind == PUNCT_RPAREN) {
      break;
    }
    TOK_ITER(t);
  }

  list->location = range_cat(&list_location, &(*t)->location);
  TOK_ITER(t);

  return list;
}
