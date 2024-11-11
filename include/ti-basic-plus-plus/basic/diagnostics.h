#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <ti-basic-plus-plus/basic/source_location.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
  bool has_errors;
  bool should_exit;
  bool suppress_warnings;
  bool warnings_as_errors;
} diagnostics_t;

void diagnostics_init(diagnostics_t* d);
diagnostics_t diagnostics_create(void);

typedef enum {
  NOTE,
  WARNING,
  ERROR,
} severity_t;

void diag_report(diagnostics_t*, severity_t, const char* fmt, ...);
void diag_report_file(diagnostics_t*,
                      severity_t,
                      const char* file,
                      const char* fmt,
                      ...);
void diag_report_source(diagnostics_t*,
                        severity_t,
                        source_range_t*,
                        const char* fmt,
                        ...);

static inline bool has_errors(const diagnostics_t* d) {
  assert(d != NULL);
  return d->has_errors;
}

static inline bool should_exit(const diagnostics_t* d) {
  assert(d != NULL);
  return d->should_exit || d->has_errors;
}

static inline int get_exit_status(const diagnostics_t* d) {
  assert(d != NULL);
  return has_errors(d) ? 1 : 0;
}

#endif  // DIAGNOSTICS_H

