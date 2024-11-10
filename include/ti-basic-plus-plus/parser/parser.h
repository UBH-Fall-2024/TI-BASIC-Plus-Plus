#ifndef PARSER_H
#define PARSER_H

#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <ti-basic-plus-plus/lexer/token.h>
#include <ti-basic-plus-plus/parser/ast.h>

ast_node_t* parse_tokens(token_t* head_token, diagnostics_t* d);

// Parser helper functions

token_kind_t comp_token_kind(const token_t* t, size_t n, ...);
punctuator_kind_t comp_punctuator_kind(const token_t* t, size_t n, ...);
keyword_kind_t comp_keyword_kind(const token_t* t, size_t n, ...);

#endif  // PARSER_H

