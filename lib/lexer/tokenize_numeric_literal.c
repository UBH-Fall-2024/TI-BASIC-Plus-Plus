#include <ti-basic-plus-plus/lexer/lexer.h>

#include <ctype.h>
#include <string.h>
#include <stb_ds.h>

token_t* tokenize_numeric_literal(input_file_iterator_t* it, diagnostics_t* d) {
  assert(it != NULL);
  assert(d != NULL);

  source_position_t start_position = if_get_position(it);
  source_position_t end_position = start_position;

  int c = if_current(it);
  int next_c = if_peek_next(it);
  assert(isdigit(c) || c == '.');

  double value = 0.0;

  // Decimal number
  if (c - '0' > 0 || c == '.' || next_c == '.') {
    const bool dot_seen = (c == '.');

    char* text = NULL;
    arrput(text, c);
    for (c = if_next(it); isdigit(c); c = if_next(it)) {
      arrput(text, c);
      end_position = if_get_position(it);
    }

    if (c == '.' && !dot_seen) {
      end_position = if_get_position(it);
      arrput(text, c);
      for (c = if_next(it); isdigit(c); c = if_next(it)) {
        arrput(text, c);
        end_position = if_get_position(it);
      }
    }

    arrput(text, '\0');

    value = atof(text);

    arrfree(text);
  }
  // Hexadecimal number
  else if (next_c == 'x' || next_c == 'X') {
    (void)if_next(it); // '0'
    (void)if_next(it); // 'x'

    int i_value = 0;

    c = if_current(it);
    while (isalnum(c)) {
      end_position = if_get_position(it);
      int c_value;

      if (isdigit(c)) {
        c_value = c - '0';
      }
      else {
        if (c >= 'a' && c <= 'f') {
          c_value = 10 + (c - 'a');
        }
        else if (c >= 'A' && c <= 'F') {
          c_value = 10 + (c - 'A');
        }
        else {
          source_range_t range = range_at(it->file, end_position);
          diag_report_source(d, ERROR, &range, "invalid suffix on hexadecimal literal", c);
          return NULL;
        }
      }

      i_value *= 0x10;
      i_value += c_value;

      c = if_next(it);
    }

    value = (double)i_value;
  }
  // Binary number
  else if (next_c == 'b' || next_c == 'B') {
    (void)if_next(it); // '0'
    (void)if_next(it); // 'b'

    int i_value = 0;

    c = if_current(it);
    while (isalnum(c)) {
      end_position = if_get_position(it);

      if (c != '0' && c != '1') {
        source_range_t range = range_at(it->file, end_position);
        diag_report_source(d, ERROR, &range, "invalid suffix on binary literal", c);
        return NULL;
      }

      i_value *= 2;
      i_value += c - '0';
      
      c = if_next(it);
    }

    value = (double)i_value;
  }
  // Octal number.
  else {
    (void)if_next(it); // '0'

    int i_value = 0;

    c = if_current(it);
    while (isalnum(c)) {
      end_position = if_get_position(it);

      if (c < '0' || c > '7') {
        source_range_t range = range_at(it->file, end_position);
        diag_report_source(d, ERROR, &range, "invalid suffix on octal literal", c);
        return NULL;
      }

      i_value *= 010;
      i_value += c - '0';
      
      c = if_next(it);
    }

    value = (double)i_value;
  }

  if (isalpha(c)) {
    source_range_t range = range_at(it->file, if_get_position(it));
    diag_report_source(d, ERROR, &range, "invalid suffix on numeric literal", c);
    return NULL;
  }

  source_range_t range =
      range_create(it->file, start_position, end_position);

  token_t* token = token_create(TOKEN_NUMBER_LITERAL, range);
  token->data.number = value;

  return token;
}
