#include <ti-basic-plus-plus/lexer/token.h>

#include <assert.h>
#include <stb_ds.h>
#include <stdlib.h>
#include <string.h>

token_t* token_create(token_kind_t kind, source_range_t location) {
  assert(kind != TOKEN_UNKNOWN);

  token_t* token = malloc(sizeof(token_t));
  if (token == NULL) {
    assert(false);
    return NULL;
  }

  (void)memset(token, 0, sizeof(token_t));

  token->kind = kind;
  token->location = location;

  return token;
}

static void token_destroy(token_t* token) {
  if (token->kind == TOKEN_STRING_LITERAL || token->kind == TOKEN_IDENTIFIER) {
    arrfree(token->data.string);
  }

  free(token);
}

void token_list_destroy(token_t* head) {
  assert(head != NULL);

  token_t* current = head;
  while (current != NULL) {
    head = current;
    current = current->next;

    token_destroy(head);
  }
}

static void print_token_unknown(token_t* token, FILE* stream);
static void print_token_identifier(token_t* token, FILE* stream);
static void print_token_keyword(token_t* token, FILE* stream);
static void print_token_punctuator(token_t* token, FILE* stream);
static void print_token_number_literal(token_t* token, FILE* stream);
static void print_token_string_literal(token_t* token, FILE* stream);
static void print_token_newline(token_t* token, FILE* stream);
static void print_token_eof(token_t* token, FILE* stream);

void print_token(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);

  void (*print_function)(token_t*, FILE*) = NULL;

  switch (token->kind) {
    case TOKEN_UNKNOWN:
      print_function = print_token_unknown;
      break;
    case TOKEN_IDENTIFIER:
      print_function = print_token_identifier;
      break;
    case TOKEN_KEYWORD:
      print_function = print_token_keyword;
      break;
    case TOKEN_PUNCTUATOR:
      print_function = print_token_punctuator;
      break;
    case TOKEN_NUMBER_LITERAL:
      print_function = print_token_number_literal;
      break;
    case TOKEN_STRING_LITERAL:
      print_function = print_token_string_literal;
      break;
    case TOKEN_NEWLINE:
      print_function = print_token_newline;
      break;
    case TOKEN_EOF:
      print_function = print_token_eof;
      break;
    default:
      assert(false);
      break;
  }

  if (print_function != NULL) {
    print_function(token, stream);
  }
}

void print_token_list(token_t* head, FILE* stream) {
  assert(head != NULL);
  assert(stream != NULL);

  while (head != NULL) {
    print_token(head, stream);
    head = head->next;
  }
}

static void print_token_unknown(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_UNKNOWN);

  fputs("Token: Unknown\n", stream);
  print_range(&token->location, 1, stream);
}

static void print_token_identifier(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_IDENTIFIER);

  fputs("Token: Identifier\n", stream);
  print_range(&token->location, 1, stream);

  fprintf(stream, "\tText: '%.*s'\n", (int)arrlen(token->data.string),
          token->data.string);
}

static void print_token_keyword(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_KEYWORD);

  fputs("Token: Keyword\n", stream);
  print_range(&token->location, 1, stream);

  /* fprintf(stream, "\tValue: '%s'\n", ...); */
}

static void print_token_punctuator(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_PUNCTUATOR);

  fputs("Token: Punctuator\n", stream);
  print_range(&token->location, 1, stream);

  /* fprintf(stream, "\tValue: '%s'\n", ...); */
}

static void print_token_number_literal(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_NUMBER_LITERAL);

  fputs("Token: Number Literal\n", stream);
  print_range(&token->location, 1, stream);

  fprintf(stream, "\tValue: '%f'\n", token->data.number);
}

static void print_token_string_literal(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_STRING_LITERAL);

  fputs("Token: String Literal\n", stream);
  print_range(&token->location, 1, stream);

  fprintf(stream, "\tText: '%.*s'\n", (int)arrlen(token->data.string),
          token->data.string);
}

static void print_token_newline(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_NEWLINE);

  fputs("Token: Newline\n", stream);
  print_range(&token->location, 1, stream);
}

static void print_token_eof(token_t* token, FILE* stream) {
  assert(token != NULL);
  assert(stream != NULL);
  assert(token->kind == TOKEN_EOF);

  fputs("Token: EOF\n", stream);
  print_range(&token->location, 1, stream);
}

