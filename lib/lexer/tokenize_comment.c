#include <ti-basic-plus-plus/lexer/lexer.h>

void tokenize_line_comment(input_file_iterator_t* it) {
  assert(it != NULL);

  assert(if_current(it) == '/');
  assert(if_peek_next(it) == '/');

  (void)if_next(it);  // '/'
  (void)if_next(it);  // '/'

  int c = if_current(it);
  while (c != EOF && c != '\n') {
    c = if_next(it);
  }
}

void tokenize_block_comment(input_file_iterator_t* it, diagnostics_t* d) {
  assert(it != NULL);
  assert(d != NULL);

  assert(if_current(it) == '/');
  assert(if_peek_next(it) == '*');

  source_range_t location = range_at_current(it);

  (void)if_next(it);  // '/'
  (void)if_next(it);  // '*'

  int c = if_current(it);
  while (c != EOF && (c != '*' || if_peek_next(it) != '/')) {
    c = if_next(it);
  }

  if (c == EOF) {
    diag_report_source(d, ERROR, &location, "unterminated /* comment");
    return;
  }

  (void)if_next(it);  // '*'
  (void)if_next(it);  // '/'
}
