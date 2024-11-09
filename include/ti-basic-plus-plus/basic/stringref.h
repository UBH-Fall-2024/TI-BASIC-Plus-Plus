#ifndef STRING_H
#define STRING_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
  char* data;
  size_t length;
} stringref_t;

static inline stringref_t stringref_create(const char* s) {
  return (stringref_t){.data = (char*)s, .length = s ? strlen(s) : 0};
}

static inline stringref_t stringref_create_len(const char* s, size_t n) {
  return (stringref_t){.data = (char*)s, .length = s ? n : 0};
}

static inline bool stringref_is_empty(const stringref_t* s) {
  assert(s != NULL);
  return s->data == NULL || s->length == 0;
}

static inline bool stringref_is_valid(const stringref_t* s) {
  return stringref_is_empty(s) == false;
}

bool stringref_eq(const stringref_t* a, const stringref_t* b);
bool stringref_eq_cstr(const stringref_t* s, const char* cstr);

bool stringref_starts_with(const stringref_t* s, const stringref_t* prefix);
bool stringref_starts_with_cstr(const stringref_t* s, const char* prefix);

bool stringref_ends_with(const stringref_t* s, const stringref_t* suffix);
bool stringref_ends_with_cstr(const stringref_t* s, const char* suffix);

// Increments the string data pointer and decrements the length.
// Calling this on an empty string is undefined behavior.
// Returns the length of the resulting string.
size_t stringref_increment(stringref_t* s);
size_t stringref_increment_n(stringref_t* s, size_t n);

#endif  // STRING_H

