#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include <ti-basic-plus-plus/parser/parser.h>

#include <ti-basic-plus-plus/calculator/variable.h>

ast_node_t* parse_global_decl(token_t** head_token, diagnostics_t* d);

// Start the token after the variable's name.
ast_node_t* parse_variable_decl(token_t** t,
                                variable_type_t type,
                                const char* name,
                                source_range_t start_location,
                                diagnostics_t* d);

// Start the token after (
ast_node_t* parse_function_decl(token_t** t,
                                variable_type_t return_type,
                                const char* name,
                                source_range_t start_location,
                                diagnostics_t* d);

variable_type_t parse_variable_type(token_t** t,
                                    bool void_allowed,
                                    diagnostics_t* d);

// Start the token after {
ast_node_t* parse_block(token_t** t,
                        source_range_t start_location,
                        diagnostics_t* d);

ast_node_t* parse_if_statement(token_t** t,
                               source_range_t start_location,
                               diagnostics_t* d);

ast_node_t* parse_while_statement(token_t** t,
                                  source_range_t start_location,
                                  diagnostics_t* d);

ast_node_t* parse_return_statement(token_t** t,
                                   source_range_t start_location,
                                   diagnostics_t* d);

ast_node_t* parse_for_statement(token_t** t,
                                source_range_t start_location,
                                diagnostics_t* d);

ast_node_t* parse_expression(token_t** t,
                             diagnostics_t* d);

#endif  // PARSER_INTERNAL_H

