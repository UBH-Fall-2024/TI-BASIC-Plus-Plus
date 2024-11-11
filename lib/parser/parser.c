#include <ti-basic-plus-plus/parser/parser.h>

#include <assert.h>
#include <stdarg.h>

#include "parser_internal.h"

ast_node_t* parse_tokens(token_t* head_token, diagnostics_t* d) {
  assert(head_token != NULL);
  assert(d != NULL);

  ast_node_t* root = ast_node_create(AST_ROOT);

  while (head_token != NULL && head_token->kind != TOKEN_EOF &&
         !should_exit(d)) {
    ast_node_t* decl = parse_global_decl(&head_token, d);
    if (decl == NULL) {
      return root;
    }

    arrput(root->children, decl);
  }

  // Should be EOF...

  return root;
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

void unexpected_token(token_t* t, token_kind_t kind, diagnostics_t* d) {
  assert(t != NULL);
  assert(d != NULL);

  if (t->kind != kind || kind == TOKEN_UNKNOWN) {
    diag_report_source(d, ERROR, &t->location, "unexpected %s",
                       token_kind_to_string(t->kind));
  } else if (t->kind == TOKEN_PUNCTUATOR) {
    diag_report_source(d, ERROR, &t->location, "unexpected punctuator '%s'",
                       punct_to_string(t->data.punctuator));
  } else if (t->kind == TOKEN_KEYWORD) {
    diag_report_source(d, ERROR, &t->location, "unexpected keyword '%s'",
                       keyword_to_string(t->data.keyword));
  } else {
    diag_report_source(d, ERROR, &t->location, "unexpected token");
  }
}

variable_type_t parse_variable_type(token_t** t, bool void_allowed, diagnostics_t* d) {
  keyword_kind_t keyword_kind = comp_keyword_kind(
      *t, 5, KW_VOID, KW_NUMBER, KW_STRING, KW_MATRIX, KW_LIST);
  if (keyword_kind == KW_UNKNOWN) {
    unexpected_token(*t, TOKEN_KEYWORD, d);
    return VAR_UNKNOWN;
  }

  if (!void_allowed && keyword_kind == KW_VOID) {
    diag_report_source(d, ERROR, &(*t)->location, "variables of type 'void' are not allowed");
    return VAR_UNKNOWN;
  }

  TOK_ITER(t);

  return keyword_to_variable_type(keyword_kind);
}

