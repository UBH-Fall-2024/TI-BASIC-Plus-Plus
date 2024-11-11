#ifndef LEXER_H
#define LEXER_H

#include <ti-basic-plus-plus/basic/diagnostics.h>
#include <ti-basic-plus-plus/basic/input_file.h>
#include <ti-basic-plus-plus/lexer/token.h>

token_t* tokenize_file(input_file_t* input_file, diagnostics_t* d);

token_t* next_token(input_file_iterator_t* it, diagnostics_t* d);

#endif  // LEXER_H

