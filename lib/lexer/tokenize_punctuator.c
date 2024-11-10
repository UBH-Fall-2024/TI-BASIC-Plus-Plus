#include <ti-basic-plus-plus/lexer/lexer.h>

#include <ti-basic-plus-plus/lexer/punctuator.h>

token_t* tokenize_punctuator(input_file_iterator_t* it, diagnostics_t* d) {
  assert(it != NULL);
  assert(d != NULL);

  source_position_t start_position = if_get_position(it);

  punctuator_kind_t kind = PUNCT_UNKNOWN;

  int c = if_current(it);
  int next_c = if_peek_next(it);
  switch (c) {
    case '(':
      kind = PUNCT_LPAREN;
      break;
    case ')':
      kind = PUNCT_RPAREN;
      break;
    case '{':
      kind = PUNCT_LBRACE;
      break;
    case '}':
      kind = PUNCT_RBRACE;
      break;
    case '[':
      kind = PUNCT_LBRACKET;
      break;
    case ']':
      kind = PUNCT_RBRACKET;
      break;
    case ':':
      kind = PUNCT_COLON;
      break;
    case '.':
      kind = PUNCT_DOT;
      break;
    case ',':
      kind = PUNCT_COMMA;
      break;
    case '+':
      kind = PUNCT_ADD;
      if (next_c == '=') {
        kind = PUNCT_ASGN_ADD;
      }
      break;
    case '-':
      kind = PUNCT_SUB;
      if (next_c == '=') {
        kind = PUNCT_ASGN_SUB;
      }
      break;
    case '*':
      kind = PUNCT_MUL;
      if (next_c == '=') {
        kind = PUNCT_ASGN_MUL;
      }
      break;
    case '/':
      kind = PUNCT_DIV;
      if (next_c == '=') {
        kind = PUNCT_ASGN_DIV;
      }
      break;
    case '%':
      kind = PUNCT_MOD;
      if (next_c == '=') {
        kind = PUNCT_ASGN_MOD;
      }
      break;
    case '=':
      kind = PUNCT_ASGN;
      if (next_c == '=') {
        kind = PUNCT_CMP_EQ;
      }
      break;
    case '!':
      if (next_c == '=') {
        kind = PUNCT_CMP_NE;
      }
      break;
    case '<':
      kind = PUNCT_CMP_LT;
      if (next_c == '=') {
        kind = PUNCT_CMP_LE;
      }
      break;
    case '>':
      kind = PUNCT_CMP_GT;
      if (next_c == '=') {
        kind = PUNCT_CMP_GE;
      }
      break;
    default:
      break;
  }

  if (kind == PUNCT_UNKNOWN) {
    return NULL;
  }

  if (kind >= PUNCT_ASGN_ADD) {
    (void)if_next(it);
  }

  source_range_t range =
      range_create(it->file, start_position, if_get_position(it));

  (void)if_next(it);

  token_t* token = token_create(TOKEN_PUNCTUATOR, range);
  token->data.punctuator = kind;

  return token;
}

