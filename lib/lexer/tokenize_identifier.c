#include <ti-basic-plus-plus/lexer/lexer.h>

#include <stb_ds.h>
#include <ctype.h>

token_t* tokenize_identifier(input_file_iterator_t* it, diagnostics_t* d) {
  assert(it != NULL);
  assert(d != NULL);

  source_position_t start_position = if_get_position(it);

  char* text = NULL;
  int c = if_current(it);
  while (isalpha(c) || c == '_') {
    arrput(text, c);
    c = if_next(it);
  }

  source_range_t range =
      range_create(it->file, start_position, if_get_position(it));

  token_t* token = token_create(TOKEN_IDENTIFIER, range);
  token->data.string = text;

  return token;
}
