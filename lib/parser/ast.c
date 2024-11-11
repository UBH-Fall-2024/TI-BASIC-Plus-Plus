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

ast_node_t* ast_node_create_variable_decl(variable_type_t type,
                                          const char* name,
                                          bool has_variable,
                                          variable_t variable,
                                          ast_node_t* expr,
                                          source_range_t location) {
  ast_node_t* node = ast_node_create(AST_VARIABLE_DECL);
  node->data.variable_decl.type = type;
  node->data.variable_decl.name = name;
  node->data.variable_decl.has_variable = has_variable;
  node->data.variable_decl.variable = variable;
  if (expr != NULL) {
    arrput(node->children, expr);
  }
  node->location = location;

  return node;
}

ast_node_t* ast_node_create_param(variable_type_t type,
                                  const char* name,
                                  source_range_t param_location) {
  ast_node_t* node = ast_node_create(AST_PARAMETER);
  node->data.variable_decl.type = type;
  node->data.variable_decl.name = name;
  node->location = param_location;
  return node;
}

ast_node_t* ast_node_create_function_decl(variable_type_t return_type,
                                          const char* name,
                                          ast_node_t* parameters,
                                          ast_node_t* body,
                                          source_range_t location) {
  ast_node_t* node = ast_node_create(AST_FUNCTION_DECL);
  node->data.function_decl.return_type = return_type;
  node->data.function_decl.name = name;
  if (parameters != NULL) {
    arrput(node->children, parameters);
  }
  if (body != NULL) {
    arrput(node->children, body);
  }
  node->location = location;
  return node;
}

ast_node_t* ast_node_create_unary_expr(ast_node_t* operand,
                                       operator_kind_t op,
                                       source_range_t location) {
  assert(operand != NULL);

  ast_node_t* node = ast_node_create(AST_UNARY_EXPRESSION);
  arrput(node->children, operand);
  node->data.operator= op;
  node->location = location;
  return node;
}

ast_node_t* ast_node_create_binary_expr(ast_node_t* left,
                                        operator_kind_t op,
                                        ast_node_t* right,
                                        source_range_t location) {
  assert(left != NULL);
  assert(right != NULL);

  ast_node_t* node = ast_node_create(AST_BINARY_EXPRESSION);
  arrput(node->children, left);
  arrput(node->children, right);
  node->data.operator= op;
  node->location = location;
  return node;
}

ast_node_t* ast_node_create_function_call(const char* name,
                                          ast_node_t** arguments,
                                          source_range_t location) {
  ast_node_t* node = ast_node_create(AST_FUNCTION_CALL);
  node->data.function_call.name = name;
  node->children = arguments;
  node->location = location;
  return node;
}

void ast_node_destroy(ast_node_t* node) {
  assert(node != NULL);

  if (!node->dont_free_children) {
    for (size_t i = 0; i < arrlenu(node->children); i++) {
      ast_node_destroy(node->children[i]);
    }
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
    case AST_FUNCTION_DECL:
      name = "Function Declaration";
      break;
    case AST_VARIABLE_DECL:
      name = "Variable Declaration";
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
    case AST_PARAMETER:
      name = "Parameter";
      break;
    case AST_PARAMETER_LIST:
      name = "Parameter List";
      break;
    case AST_BINARY_EXPRESSION:
      name = "Binary Expression";
      break;
    case AST_UNARY_EXPRESSION:
      name = "Unary Expression";
      break;
    case AST_IDENTIFIER:
      name = "Identifier";
      break;
    case AST_NUMERIC_LITERAL:
      name = "Numeric Literal";
      break;
    case AST_STRING_LITERAL:
      name = "String Literal";
      break;
    case AST_FUNCTION_CALL:
      name = "Function Call";
      break;
    case AST_IF_STATEMENT:
      name = "If Statement";
      break;
    case AST_WHILE_STATEMENT:
      name = "While Statement";
      break;
    /* case AST_FOR_STATEMENT: */
    /*   name = "For Statement"; */
    /*   break; */
    case AST_RETURN_STATEMENT:
      name = "Return Statement";
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
    case AST_FUNCTION_DECL:
      break;
    case AST_VARIABLE_DECL:
      print_indent(stream, depth + 1);
      fprintf(stream, "Type: %s\n",
              variable_type_to_string(node->data.variable_decl.type));
      print_indent(stream, depth + 1);
      fprintf(stream, "Name: %.*s\n",
              (int)arrlen(node->data.variable_decl.name),
              node->data.variable_decl.name);
      if (node->data.variable_decl.has_variable) {
        print_indent(stream, depth + 1);
        fprintf(stream, "Variable: %c\n", node->data.variable_decl.variable.id);
      }
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
    case AST_PARAMETER:
      print_indent(stream, depth + 1);
      fprintf(stream, "Type: %s\n",
              variable_type_to_string(node->data.variable_decl.type));
      print_indent(stream, depth + 1);
      fprintf(stream, "Name: %.*s\n",
              (int)arrlen(node->data.variable_decl.name),
              node->data.variable_decl.name);
      break;
    case AST_PARAMETER_LIST:
      break;
    case AST_BINARY_EXPRESSION:
    case AST_UNARY_EXPRESSION:
      print_indent(stream, depth + 1);
      fprintf(stream, "Operator: %s\n", op_to_string(node->data.operator));
      break;
    case AST_IDENTIFIER:
      print_indent(stream, depth + 1);
      fprintf(stream, "Value: %.*s\n", (int)arrlen(node->data.identifier),
              node->data.identifier);
      break;
    case AST_NUMERIC_LITERAL:
      print_indent(stream, depth + 1);
      fprintf(stream, "Value: %f\n", node->data.numeric_literal);
      break;
    case AST_STRING_LITERAL:
      print_indent(stream, depth + 1);
      fprintf(stream, "Value: %.*s\n", (int)arrlen(node->data.string_literal),
              node->data.string_literal);
      break;
    case AST_FUNCTION_CALL:
      print_indent(stream, depth + 1);
      fprintf(stream, "Name: %s\n", node->data.function_call.name);
      break;
    case AST_IF_STATEMENT:
    case AST_WHILE_STATEMENT:
    /* case AST_FOR_STATEMENT: */
    case AST_RETURN_STATEMENT:
      break;
    default:
      assert(false);
  }

  for (size_t i = 0; i < arrlenu(node->children); i++) {
    print_ast_node_internal(node->children[i], stream, depth + 1);
  }
}

void dump_ascii_ti_basic(ast_node_t* node, FILE* stream) {
  assert(node != NULL);
  assert(stream != NULL);

  switch (node->kind) {
    case AST_ROOT:
      for (size_t i = 0; i < arrlenu(node->children); i++) {
        dump_ascii_ti_basic(node->children[i], stream);
      }
      break;
    case AST_FUNCTION_DECL:
      // if main
      if (strncmp(node->data.function_decl.name, "main",
                  arrlenu(node->data.function_decl.name)) == 0) {
        dump_ascii_ti_basic(node->children[0], stream);
      }
      break;
    case AST_BLOCK:
      for (size_t i = 0; i < arrlenu(node->children); i++) {
        dump_ascii_ti_basic(node->children[i], stream);
      }
      break;
    case AST_VARIABLE_DECL:
      if (arrlenu(node->children) > 0) {
        dump_ascii_ti_basic(node->children[0], stream);
      }
      fprintf(stream, "→");

      if (node->data.variable_decl.has_variable) {
        switch (node->data.variable_decl.variable.type) {
          case VAR_NUMBER:
            fprintf(stream, "%c", node->data.variable_decl.variable.id);
            break;
          case VAR_STRING:
            fprintf(stream, "Str%c", node->data.variable_decl.variable.id);
            break;
          case VAR_MATRIX:
            fprintf(stream, "[%c]", node->data.variable_decl.variable.id);
            break;
          case VAR_LIST:
            fprintf(stream, "L%c", node->data.variable_decl.variable.id);
            break;
          case VAR_LIST_ELEMENT:
            fprintf(stream, "L₀(%d)", node->data.variable_decl.variable.id);
            break;
          default:
            break;
        }
        fprintf(stream, "\n");
      } else {
        fprintf(stream, " ");
      }
      break;
    case AST_STRING_LITERAL:
      fprintf(stream, "\"%.*s\"", (int)arrlen(node->data.string_literal),
              node->data.string_literal);
      break;
    case AST_NUMERIC_LITERAL:
      fprintf(stream, "%f", node->data.numeric_literal);
      break;
    case AST_FUNCTION_CALL:
      if (node->data.function_call.is_builtin) {
        print_builtin_function(
            node->data.function_call.matched_function.builtin, stream);
      } else {
        // NOT implemented
      }

      for (size_t i = 0; i < arrlenu(node->children); i++) {
        dump_ascii_ti_basic(node->children[i], stream);
      }
      fprintf(stream, "\n");
      break;
    case AST_IDENTIFIER:
      if (arrlenu(node->children) > 0) {
        if (node->children[0]->data.variable_decl.has_variable) {
          switch (node->children[0]->data.variable_decl.variable.type) {
            case VAR_NUMBER:
              fprintf(stream, "%c",
                      node->children[0]->data.variable_decl.variable.id);
              break;
            case VAR_STRING:
              fprintf(stream, "Str%c",
                      node->children[0]->data.variable_decl.variable.id);
              break;
            case VAR_MATRIX:
              fprintf(stream, "[%c]",
                      node->children[0]->data.variable_decl.variable.id);
              break;
            case VAR_LIST:
              fprintf(stream, "L%c",
                      node->children[0]->data.variable_decl.variable.id);
              break;
            case VAR_LIST_ELEMENT:
              fprintf(stream, "L₀(%d)",
                      node->children[0]->data.variable_decl.variable.id);
              break;
            default:
              break;
          }
        }
      }
      break;
    case AST_IF_STATEMENT:
      fprintf(stream, "If ");
      dump_ascii_ti_basic(node->children[0], stream);
      fprintf(stream, "\nThen\n");
      dump_ascii_ti_basic(node->children[1], stream);

      {
        size_t i = 2;
        while (i < arrlenu(node->children)) {
          ast_node_kind_t kind = node->children[i]->kind;
          if (kind != AST_BLOCK) {
            fprintf(stream, "Else\n");
            fprintf(stream, "If ");
            dump_ascii_ti_basic(node->children[i++], stream);
            fprintf(stream, "\nThen\n");
            dump_ascii_ti_basic(node->children[i++], stream);
          } else {
            fprintf(stream, "Else\n");
            dump_ascii_ti_basic(node->children[i], stream);
            break;
          }
        }

        for (size_t j = 1; j < i-1; j++) {
          fprintf(stream, "End\n");
        }
      }
      break;
    case AST_WHILE_STATEMENT:
      fprintf(stream, "While ");
      dump_ascii_ti_basic(node->children[0], stream);
      fprintf(stream, "\n");
      dump_ascii_ti_basic(node->children[1], stream);
      fprintf(stream, "End\n");
    default:
      break;
  }
}

