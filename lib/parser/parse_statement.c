#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>

#include "parser_internal.h"

ast_node_t* parse_if_statement(token_t** t,
                               source_range_t start_location,
                               diagnostics_t* d) {
  assert(t != NULL);
  assert(*t != NULL);
  assert(d != NULL);

  ast_node_t* if_node = ast_node_create(AST_IF_STATEMENT);
  if_node->location = start_location;

  ast_node_t* expr = parse_expression(t, d);
  if (expr == NULL) {
    return NULL;
  }
  arrput(if_node->children, expr);
  if_node->location = range_cat(&if_node->location, &expr->location);

  if ((*t)->kind == TOKEN_NEWLINE) {
    TOK_ITER(t);
  }

  source_range_t body_location = (*t)->location;
  if ((*t)->kind != TOKEN_PUNCTUATOR || (*t)->data.punctuator != PUNCT_LBRACE) {
    unexpected_token(*t, TOKEN_PUNCTUATOR, d);
    ast_node_destroy(if_node);
    return NULL;
  }
  TOK_ITER(t);

  ast_node_t* body = parse_block(t, body_location, d);
  if (body == NULL) {
    ast_node_destroy(if_node);
    return NULL;
  }
  arrput(if_node->children, body);
  if_node->location = range_cat(&if_node->location, &body->location);

  if ((*t)->kind != TOKEN_NEWLINE) {
    unexpected_token(*t, TOKEN_NEWLINE, d);
    ast_node_destroy(if_node);
    return NULL;
  }

  TOK_ITER(t);

  while ((*t)->kind == TOKEN_NEWLINE) {
    TOK_ITER(t);
  }

  while ((*t)->kind == TOKEN_KEYWORD && (*t)->data.keyword == KW_ELIF) {
    TOK_ITER(t);

    ast_node_t* elif_expr = parse_expression(t, d);
    if (elif_expr == NULL) {
      ast_node_destroy(if_node);
      return NULL;
    }

    arrput(if_node->children, elif_expr);
    if_node->location = range_cat(&if_node->location, &elif_expr->location);

    if ((*t)->kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
    }

    source_range_t elif_body_location = (*t)->location;
    if ((*t)->kind != TOKEN_PUNCTUATOR ||
        (*t)->data.punctuator != PUNCT_LBRACE) {
      unexpected_token(*t, TOKEN_PUNCTUATOR, d);
      ast_node_destroy(if_node);
      return NULL;
    }
    TOK_ITER(t);

    ast_node_t* elif_body = parse_block(t, elif_body_location, d);
    if (elif_body == NULL) {
      ast_node_destroy(if_node);
      return NULL;
    }

    arrput(if_node->children, elif_body);
    if_node->location = range_cat(&if_node->location, &elif_body->location);

    if ((*t)->kind != TOKEN_NEWLINE) {
      unexpected_token(*t, TOKEN_NEWLINE, d);
      ast_node_destroy(if_node);
      return NULL;
    }

    TOK_ITER(t);

    while ((*t)->kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
    }
  }

  if ((*t)->kind == TOKEN_KEYWORD && (*t)->data.keyword == KW_ELSE) {
    TOK_ITER(t);

    if ((*t)->kind == TOKEN_NEWLINE) {
      TOK_ITER(t);
    }

    source_range_t else_body_location = (*t)->location;
    if ((*t)->kind != TOKEN_PUNCTUATOR ||
        (*t)->data.punctuator != PUNCT_LBRACE) {
      unexpected_token(*t, TOKEN_PUNCTUATOR, d);
      ast_node_destroy(if_node);
      return NULL;
    }
    TOK_ITER(t);

    ast_node_t* else_body = parse_block(t, else_body_location, d);
    if (else_body == NULL) {
      ast_node_destroy(if_node);
      return NULL;
    }

    arrput(if_node->children, else_body);
    if_node->location = range_cat(&if_node->location, &else_body->location);

    if ((*t)->kind != TOKEN_NEWLINE) {
      unexpected_token(*t, TOKEN_NEWLINE, d);
      ast_node_destroy(if_node);
      return NULL;
    }

    TOK_ITER(t);
  }

  return if_node;
}

ast_node_t* parse_while_statement(token_t** t,
                                  source_range_t start_location,
                                  diagnostics_t* d) {
  ast_node_t* expr = parse_expression(t, d);
  if (expr == NULL) {
    return NULL;
  }

  if ((*t)->kind == TOKEN_NEWLINE) {
    TOK_ITER(t);
  }

  source_range_t body_location = (*t)->location;
  if ((*t)->kind != TOKEN_PUNCTUATOR || (*t)->data.punctuator != PUNCT_LBRACE) {
    unexpected_token(*t, TOKEN_PUNCTUATOR, d);
    ast_node_destroy(expr);
    return NULL;
  }

  TOK_ITER(t);

  ast_node_t* body = parse_block(t, body_location, d);
  if (body == NULL) {
    ast_node_destroy(expr);
    return NULL;
  }

  ast_node_t* while_node = ast_node_create(AST_WHILE_STATEMENT);
  while_node->location = range_cat(&start_location, &body->location);
  arrput(while_node->children, expr);
  arrput(while_node->children, body);

  return while_node;
}

ast_node_t* parse_return_statement(token_t** t,
                                   source_range_t start_location,
                                   diagnostics_t* d) {
  ast_node_t* expr = parse_expression(t, d);
  if (expr == NULL) {
    return NULL;
  }

  if ((*t)->kind != TOKEN_NEWLINE) {
    unexpected_token(*t, TOKEN_NEWLINE, d);
    ast_node_destroy(expr);
    return NULL;
  }

  TOK_ITER(t);

  ast_node_t* return_node = ast_node_create(AST_RETURN_STATEMENT);
  return_node->location = range_cat(&start_location, &expr->location);
  arrput(return_node->children, expr);

  return return_node;
}

