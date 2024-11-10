#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>
#include <stdarg.h>

ast_node_t* parse_tokens(token_t* head_token, diagnostics_t* d) {
  assert(head_token != NULL);
  assert(d != NULL);

  return NULL;
}

token_kind_t comp_token_kind(const token_t* t, size_t n, ...) {
  assert(n > 0);

  if (t == NULL) {
    return TOKEN_UNKNOWN;
  }

  token_kind_t matched_kind = TOKEN_UNKNOWN;

  va_list args;
  va_start(args, n);

  for (size_t i = 0; i < n; i++) {
    token_kind_t kind = va_arg(args, token_kind_t);
    if (t->kind == kind) {
      matched_kind = kind;
      break;
    }
  }

  va_end(args);

  return matched_kind;
}

punctuator_kind_t comp_punctuator_kind(const token_t* t, size_t n, ...) {
  assert(n > 0);

  if (t == NULL || t->kind != TOKEN_PUNCTUATOR) {
    return PUNCT_UNKNOWN;
  }

  punctuator_kind_t matched_kind = PUNCT_UNKNOWN;

  va_list args;
  va_start(args, n);

  for (size_t i = 0; i < n; i++) {
    punctuator_kind_t kind = va_arg(args, punctuator_kind_t);
    if (t->data.punctuator == kind) {
      matched_kind = kind;
      break;
    }
  }

  va_end(args);

  return matched_kind;
}

keyword_kind_t comp_keyword_kind(const token_t* t, size_t n, ...) {
  assert(n > 0);

  if (t == NULL || t->kind != TOKEN_KEYWORD) {
    return KW_UNKNOWN;
  }

  keyword_kind_t matched_kind = KW_UNKNOWN;

  va_list args;
  va_start(args, n);

  for (size_t i = 0; i < n; i++) {
    keyword_kind_t kind = va_arg(args, keyword_kind_t);
    if (t->data.keyword == kind) {
      matched_kind = kind;
      break;
    }
  }

  va_end(args);

  return matched_kind;
}

