#ifndef AST_H
#define AST_H

#include <stb_ds.h>
#include <stdio.h>
#include <ti-basic-plus-plus/basic/source_location.h>
#include <ti-basic-plus-plus/calculator/variable.h>
#include <ti-basic-plus-plus/calculator/builtin_functions.h>
#include <ti-basic-plus-plus/parser/operator.h>

typedef enum {
  AST_ROOT,
  AST_FUNCTION_DECL,
  AST_VARIABLE_DECL,
  AST_STATEMENT,
  AST_BLOCK,
  AST_VARIABLE_RESERVATION,
  AST_PARAMETER,
  AST_PARAMETER_LIST,

  AST_BINARY_EXPRESSION,
  AST_UNARY_EXPRESSION,
  AST_IDENTIFIER,
  AST_NUMERIC_LITERAL,
  AST_STRING_LITERAL,

  AST_FUNCTION_CALL,

  AST_IF_STATEMENT,
  AST_WHILE_STATEMENT,
  /* AST_FOR_STATEMENT, */
  AST_RETURN_STATEMENT,
} ast_node_kind_t;

typedef struct ast_variable_decl {
  variable_type_t type;
  const char* name;
  bool has_variable;
  variable_t variable;
} ast_variable_decl_t;

typedef struct ast_function_decl {
  variable_type_t return_type;
  const char* name;
} ast_function_decl_t;

typedef struct ast_function_call {
  const char* name;
  union {
    struct ast_node* decl;
    builtin_function_t builtin;
  } matched_function;
  bool is_builtin;
} ast_function_call_t;

typedef struct ast_node {
  ast_node_kind_t kind;

  source_range_t location;
  source_range_t error_location;
  bool has_error_location;

  struct ast_node** children;
  bool dont_free_children;

  union {
    variable_t reserved_variable;
    ast_variable_decl_t variable_decl;
    ast_function_decl_t function_decl;
    const char* identifier;
    double numeric_literal;
    const char* string_literal;
    operator_kind_t operator;
    ast_function_call_t function_call;
  } data;
} ast_node_t;

ast_node_t* ast_node_create(ast_node_kind_t kind);
void ast_node_destroy(ast_node_t* node);

ast_node_t* ast_node_create_reservation(variable_t variable,
                                        source_range_t location);

ast_node_t* ast_node_create_variable_decl(variable_type_t type,
                                          const char* name,
                                          bool has_variable,
                                          variable_t variable,
                                          ast_node_t* expr,
                                          source_range_t location);

ast_node_t* ast_node_create_param(variable_type_t type,
                                  const char* name,
                                  source_range_t param_location);

ast_node_t* ast_node_create_function_decl(variable_type_t return_type,
                                          const char* name,
                                          ast_node_t* parameters,
                                          ast_node_t* body,
                                          source_range_t location);

ast_node_t* ast_node_create_unary_expr(ast_node_t* operand,
                                       operator_kind_t op,
                                       source_range_t location);

ast_node_t* ast_node_create_binary_expr(ast_node_t* left,
                                        operator_kind_t op,
                                        ast_node_t* right,
                                        source_range_t location);

ast_node_t* ast_node_create_function_call(const char* name,
                                          ast_node_t** arguments,
                                          source_range_t location);

void print_ast(ast_node_t* root, FILE* stream);

void dump_ascii_ti_basic(ast_node_t* root, FILE* stream);

#endif  // AST_H

