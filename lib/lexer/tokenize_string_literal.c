#include <ti-basic-plus-plus/lexer/lexer.h>

#include <stb_ds.h>

token_t* tokenize_string_literal(input_file_iterator_t* it, diagnostics_t* d) {
  assert(it != NULL);
  assert(d != NULL);

  const int quote = if_current(it);
  assert(quote == '"' || quote == '\'');

  source_position_t start_position = if_get_position(it);

  (void)if_next(it);  // quote

  char* text = NULL;
  int c = if_current(it);
  while (c != quote) {
    if (c == EOF || c == '\n') {
      source_range_t location = range_at(it->file, start_position);
      diag_report_source(d, ERROR, &location, "unterminated string literal");
      arrfree(text);
      return NULL;
    }

    arrput(text, c);
    c = if_next(it);
  }

  source_position_t end_position = if_get_position(it);

  (void)if_next(it);  // quote

  source_range_t range = range_create(it->file, start_position, end_position);

  token_t* token = token_create(TOKEN_STRING_LITERAL, range);
  token->data.string = text;

  return token;
}

