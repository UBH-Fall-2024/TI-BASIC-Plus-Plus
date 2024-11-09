#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct input_file;
struct input_file_iterator;

typedef struct source_position {
  size_t line;
  size_t column;
} source_position_t;

static inline bool are_positions_equal(const source_position_t* a,
                                       const source_position_t* b) {
  assert(a != NULL);
  assert(b != NULL);
  return a->line == b->line && a->column == b->column;
}

typedef struct source_range {
  struct input_file* file;

  source_position_t begin;
  source_position_t end;
} source_range_t;

bool range_validate(const source_range_t* range);

source_range_t range_create(struct input_file* file,
                            source_position_t begin,
                            source_position_t end);

static inline source_range_t range_at(struct input_file* file, source_position_t position) {
  assert(file != NULL);
  return range_create(file, position, position);
}

// Creates a source range pointing to the current position in the file.
source_range_t range_at_current(struct input_file_iterator* file);

// Concatenate two source ranges.
source_range_t range_cat(source_range_t* begin, source_range_t* end);

#endif  // SOURCE_LOCATION_H

