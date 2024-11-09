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

static void emit_code(FILE* stream, source_range_t* range) {
  assert(stream != NULL);
  assert(range != NULL);
}

