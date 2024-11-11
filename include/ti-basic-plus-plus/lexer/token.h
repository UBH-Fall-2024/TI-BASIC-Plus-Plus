#ifndef TOKEN_H
#define TOKEN_H

#include <ti-basic-plus-plus/basic/source_location.h>
#include <ti-basic-plus-plus/lexer/punctuator.h>
#include <ti-basic-plus-plus/lexer/keyword.h>

typedef enum {
  TOKEN_UNKNOWN = 0,
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_PUNCTUATOR,
  TOKEN_NUMBER_LITERAL,
  TOKEN_STRING_LITERAL,
  TOKEN_NEWLINE,
  TOKEN_EOF,
} token_kind_t;

const char* token_kind_to_string(token_kind_t kind);

typedef struct token {
  struct token* next;
  struct token* prev;

  source_range_t location;

  token_kind_t kind;

  union {
    char* string; // stb_ds allocated
    double number;
    keyword_kind_t keyword;
    punctuator_kind_t punctuator;
  } data;
} token_t;

// Creates a token with the given kind and location. The token's data is
// initialized to zero.
token_t* token_create(token_kind_t kind, source_range_t location);

// Frees an entire linked list of tokens.
void token_list_destroy(token_t* head);

// Prints a token with debug info to the given stream.
void print_token(token_t* token, FILE* stream);
void print_token_list(token_t* head, FILE* stream);

#endif // TOKEN_H
