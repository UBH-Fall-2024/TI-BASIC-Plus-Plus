#include <ti-basic-plus-plus/basic/diagnostics.h>

#include <ti-basic-plus-plus/basic/input_file.h>

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TYPE_NORMAL,
  TYPE_FILE,
  TYPE_LOCATION,
} diagnostic_type_t;

typedef enum {
  SEVERITY_NOTE = NOTE,
  SEVERITY_WARNING = WARNING,
  SEVERITY_ERROR = ERROR,
  SEVERITY_SUPPRESS,
} severity_internal_t;

static severity_internal_t get_severity(const diagnostics_t* d, severity_t severity);

static void emit_diagnostic_v(FILE* stream,
                              severity_internal_t severity,
                              const char* file,
                              source_range_t* range,
                              const char* fmt,
                              va_list args);

static void emit_location(FILE* stream,
                          const char* file,
                          const source_range_t* range);
static void emit_severity(FILE* stream, severity_internal_t severity);
static void emit_message(FILE* stream, const char* fmt, va_list args);
static void emit_code(FILE* stream, source_range_t* range);

void diagnostics_init(diagnostics_t* d) {
  assert(d != NULL);

  memset(d, 0, sizeof(diagnostics_t));
}

diagnostics_t diagnostics_create(void) {
  diagnostics_t d;
  diagnostics_init(&d);
  return d;
}

void diag_report(diagnostics_t* d, severity_t orig_severity, const char* fmt, ...) {
  assert(d != NULL);

  severity_internal_t severity = get_severity(d, orig_severity);
  if (severity == SEVERITY_SUPPRESS) {
    return;
  }

  va_list args;
  va_start(args, fmt);
  emit_diagnostic_v(stderr, severity, NULL, NULL, fmt, args);
  va_end(args);

  if (severity == SEVERITY_ERROR) {
    d->has_errors = true;
  }
}

void diag_report_file(diagnostics_t* d,
    severity_t orig_severity,
                      const char* file,
                      const char* fmt,
                      ...) {
  assert(d != NULL);
  assert(file != NULL);

  severity_internal_t severity = get_severity(d, orig_severity);
  if (severity == SEVERITY_SUPPRESS) {
    return;
  }

  va_list args;
  va_start(args, fmt);
  emit_diagnostic_v(stderr, severity, file, NULL, fmt, args);
  va_end(args);

  if (severity == SEVERITY_ERROR) {
    d->has_errors = true;
  }
}

void diag_report_source(diagnostics_t* d,
                        severity_t orig_severity,
                        source_range_t* range,
                        const char* fmt,
                        ...) {
  assert(d != NULL);
  assert(range_validate(range));

  severity_internal_t severity = get_severity(d, orig_severity);
  if (severity == SEVERITY_SUPPRESS) {
    return;
  }

  const char* file = range->file->path;

  va_list args;
  va_start(args, fmt);
  emit_diagnostic_v(stderr, severity, file, range, fmt, args);
  va_end(args);

  if (severity >= SEVERITY_ERROR) {
    d->has_errors = true;
  }
}

static severity_internal_t get_severity(const diagnostics_t* d, severity_t severity) {
  assert(d != NULL);

  if (severity == WARNING) {
    if (d->warnings_as_errors) {
      return SEVERITY_ERROR;
    }

    if (d->suppress_warnings) {
      return SEVERITY_SUPPRESS;
    }
  }

  return (severity_internal_t)severity;
}

static void emit_diagnostic_v(FILE* stream,
                              severity_internal_t severity,
                              const char* file,
                              source_range_t* range,
                              const char* fmt,
                              va_list args) {
  assert(stream != NULL);
  assert(severity != SEVERITY_SUPPRESS);
  assert(file != NULL || range == NULL);
  assert(fmt != NULL);

  if (file != NULL) {
    emit_location(stream, file, range);
  }
  emit_severity(stream, severity);
  emit_message(stream, fmt, args);

  if (range != NULL) {
    emit_code(stream, range);
  }
}

static void emit_location(FILE* stream,
                          const char* file,
                          const source_range_t* range) {
  assert(stream != NULL);
  assert(file != NULL);

  (void)fprintf(stream, "\033[0;1m%s:", file);

  if (range != NULL) {
    (void)fprintf(stream, "%zu:%zu:", range->begin.line, range->begin.column);
  }
  (void)fprintf(stream, "\033[0m ");
}

static void emit_severity(FILE* stream, severity_internal_t severity) {
  assert(stream != NULL);
  assert(severity != SEVERITY_SUPPRESS);

  const char* style = "";
  const char* text;

  switch (severity) {
    case SEVERITY_NOTE:
      style = "1;35";
      text = "note: ";
      break;
    case SEVERITY_WARNING:
      style = "1;33";
      text = "warning: ";
      break;
    case SEVERITY_ERROR:
      style = "1;31";
      text = "error: ";
      break;
    default:
      assert(false);
      return;
  }

  (void)fprintf(stream, "\033[0;%sm", style);
  (void)fprintf(stream, "%s", text);
  (void)fprintf(stream, "\033[0m");
}

static void emit_message(FILE* stream, const char* fmt, va_list args) {
  assert(stream != NULL);
  assert(fmt != NULL);

  (void)fprintf(stream, "\033[0;1m");
  (void)vfprintf(stream, fmt, args);
  (void)fprintf(stream, "\033[0m\n");
}

static size_t calculate_code_gutter_size(size_t line_number,
                                         size_t* line_digits);
static void emit_code_gutter_with_line_number(FILE* stream,
                                              size_t line,
                                              size_t line_digits,
                                              size_t gutter_size);
static void emit_code_gutter_empty(FILE* stream, size_t gutter_size);

static void emit_code(FILE* stream, source_range_t* range) {
  assert(stream != NULL);
  assert(range != NULL);

  if (range->begin.line != range->end.line) {
    // I'm not sure how to emit this... let's just not do it :D
    return;
  }

  char* line = if_get_line(range->file, range->begin.line);
  if (line == NULL) {
    assert(false);
    return;
  }
  const size_t line_length = strlen(line);
  if (!line_length) {
    assert(false);
    free(line);
    return;
  }

  size_t line_digits;
  size_t gutter_size =
      calculate_code_gutter_size(range->begin.line, &line_digits);

  // Print the code.

  emit_code_gutter_with_line_number(stream, range->begin.line, line_digits,
                                    gutter_size);
  (void)fprintf(stream, "%s\n", line);

  // Point to the problem.

  emit_code_gutter_empty(stream, gutter_size);

  (void)fputs("\033[0;1;32m", stream);
  if (range->begin.column > 1) {
    (void)fprintf(stream, "%*s", (int)(range->begin.column - 1), " ");
  }
  (void)fputc('^', stream);

  size_t problem_length = range->end.column - range->begin.column;
  for (size_t i = 0; i < problem_length; ++i) {
    (void)fputc('~', stream);
  }
  (void)fputs("\033[0m\n", stream);

  free(line);
}

static size_t calculate_code_gutter_size(size_t line_number,
                                         size_t* line_digits) {
  *line_digits = 1;
  while (line_number > 9) {
    line_number /= 10;
    (*line_digits)++;
  }

  if (*line_digits < 5) {
    return 6;
  }

  return *line_digits + 2;
}

static void emit_code_gutter_with_line_number(FILE* stream,
                                              size_t line,
                                              size_t line_digits,
                                              size_t gutter_size) {
  assert(line_digits < gutter_size);

  int num_leading_spaces = (int)(gutter_size - line_digits - 1);

  (void)fprintf(stream, "%*s%zu ", num_leading_spaces, " ", line);
  (void)fputs("| ", stream);
}

static void emit_code_gutter_empty(FILE* stream, size_t gutter_size) {
  (void)fprintf(stream, "%*s| ", (int)gutter_size, " ");
}

