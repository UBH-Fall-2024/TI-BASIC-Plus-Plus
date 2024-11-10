#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <ti-basic-plus-plus/basic/source_location.h>

typedef enum {
  AST_ROOT,
  AST_FUNCTION,
  AST_STATEMENT,
  AST_BLOCK,

} ast_node_kind_t;

typedef struct ast_node {
  ast_node_kind_t kind;

  source_range_t location;
  source_range_t error_location;
  bool has_error_location;

  struct ast_node** children;
} ast_node_t;

void print_ast(ast_node_t* root, FILE* stream);

#endif // AST_H
