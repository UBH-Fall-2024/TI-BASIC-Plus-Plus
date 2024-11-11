#ifndef INPUT_FILE_H
#define INPUT_FILE_H

#include <assert.h>
#include <ti-basic-plus-plus/basic/source_location.h>
#include <ti-basic-plus-plus/basic/stringref.h>
#include <stdio.h>

typedef struct line_data {
  long length;
  long start_position;
} line_data_t;

typedef struct input_file {
  const char* path;

  int* contents;  // EOF (-1) terminated string.
  size_t size;

  line_data_t* line_data;
} input_file_t;

typedef struct input_file_iterator {
  input_file_t* file;

  size_t line;
  size_t column;

  int* position;
} input_file_iterator_t;

// Opens a file at path, reads its contents, then closes it.
bool if_init(input_file_t* input_file, const char* path);

void if_destroy(input_file_t* input_file);

// Returns a the line at line_number in input_file. The line is allocated
// using malloc and must be freed by the caller. The line is NOT null
// terminated, instead the length of the line is stored in length.
char* if_get_line(input_file_t* input_file, size_t line_number);

void if_iterator_init(input_file_iterator_t* it, input_file_t* file);
input_file_iterator_t if_iterator_create(input_file_t* file);

static inline int if_current(const input_file_iterator_t* it) {
  assert(it != NULL);
  return *it->position;
}

// Returns the next character in the file without moving the iterator.
static inline int if_peek_next(const input_file_iterator_t* it) {
  assert(it != NULL);

  if (*it->position == EOF) {
    return EOF;
  }

  assert(it->position >= it->file->contents);
  assert((size_t)(it->position - it->file->contents) <= (it->file->size - 1));

  return *(it->position + 1);
}

static inline int if_peek_prev(const input_file_iterator_t* it) {
  assert(it != NULL);

  if (it->position == it->file->contents) {
    return EOF;
  }

  return *(it->position - 1);
}

int if_next(input_file_iterator_t* it);

static inline source_position_t if_get_position(
    const input_file_iterator_t* it) {
  return (source_position_t){.line = it->line, .column = it->column};
}

#endif  // INPUT_FILE_H

