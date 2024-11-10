#include <ti-basic-plus-plus/parser/ast.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ast_node_t* ast_node_create(ast_node_kind_t kind) {
  ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
  assert(node != NULL);

  (void)memset(node, 0, sizeof(ast_node_t));
  node->kind = kind;

  return node;
}

ast_node_t* ast_node_create_reservation(variable_t variable,
                                        source_range_t location) {
  ast_node_t* node = ast_node_create(AST_VARIABLE_RESERVATION);
  node->data.reserved_variable = variable;
  node->location = location;

  return node;
}

void ast_node_destroy(ast_node_t* node) {
  assert(node != NULL);

  for (size_t i = 0; i < arrlenu(node->children); i++) {
    ast_node_destroy(node->children[i]);
  }

  arrfree(node->children);
  free(node);
}

static inline void print_indent(FILE* stream, int depth) {
  for (int i = 0; i < depth; i++) {
    fprintf(stream, "\t");
  }
}

static void print_ast_node_internal(ast_node_t* node, FILE* stream, int depth);

void print_ast(ast_node_t* root, FILE* stream) {
  assert(root != NULL);
  assert(stream != NULL);

  print_ast_node_internal(root, stream, 0);
}

static void print_ast_node_internal(ast_node_t* node, FILE* stream, int depth) {
  assert(node != NULL);
  assert(stream != NULL);

  const char* name = NULL;

  switch (node->kind) {
    case AST_ROOT:
      name = "Root";
      break;
    case AST_FUNCTION:
      name = "Function";
      break;
    case AST_STATEMENT:
      name = "Statement";
      break;
    case AST_BLOCK:
      name = "Block";
      break;
    case AST_VARIABLE_RESERVATION:
      name = "Variable Reservation";
      break;
    default:
      assert(false);
  }

  print_indent(stream, depth);
  fprintf(stream, "%s\n", name);

  print_range(&node->location, depth + 1, stream);

  switch (node->kind) {
    case AST_ROOT:
      break;
    case AST_FUNCTION:
      break;
    case AST_STATEMENT:
      break;
    case AST_BLOCK:
      break;
    case AST_VARIABLE_RESERVATION:
      print_indent(stream, depth + 1);
      fprintf(stream, "Type: %s\n",
              variable_type_to_string(node->data.reserved_variable.type));
      print_indent(stream, depth + 1);
      fprintf(stream, "Variable: %c\n", node->data.reserved_variable.id);
      break;
    default:
      assert(false);
  }

  for (size_t i = 0; i < arrlenu(node->children); i++) {
    print_ast_node_internal(node->children[i], stream, depth + 1);
  }
}

