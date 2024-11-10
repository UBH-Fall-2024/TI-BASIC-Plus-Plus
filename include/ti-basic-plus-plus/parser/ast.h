#ifndef AST_H
#define AST_H

#include <stb_ds.h>
#include <stdio.h>
#include <ti-basic-plus-plus/basic/source_location.h>
#include <ti-basic-plus-plus/calculator/variable.h>

typedef enum {
  AST_ROOT,
  AST_FUNCTION,
  AST_STATEMENT,
  AST_BLOCK,
  AST_VARIABLE_RESERVATION,
} ast_node_kind_t;

typedef struct ast_node {
  ast_node_kind_t kind;

  source_range_t location;
  source_range_t error_location;
  bool has_error_location;

  struct ast_node** children;

  union {
    variable_t reserved_variable;
  } data;
} ast_node_t;

ast_node_t* ast_node_create(ast_node_kind_t kind);
void ast_node_destroy(ast_node_t* node);

ast_node_t* ast_node_create_reservation(variable_t variable,
                                        source_range_t location);

void print_ast(ast_node_t* root, FILE* stream);

#endif  // AST_H

