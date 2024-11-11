#include <ti-basic-plus-plus/basic/stringref.h>

bool stringref_eq(const stringref_t* a, const stringref_t* b) {
  assert(a != NULL);
  assert(b != NULL);

  if (a->length != b->length) {
    return false;
  }

  if (a->data == NULL || b->data == NULL) {
    return false;
  }

  return memcmp(a->data, b->data, a->length) == 0;
}

bool stringref_eq_cstr(const stringref_t* a, const char* b_cstr) {
  assert(a != NULL);

  stringref_t b = stringref_create(b_cstr);

  return stringref_eq(a, &b);
}

bool stringref_starts_with(const stringref_t* s, const stringref_t* prefix) {
  assert(s != NULL);

  if (s->length < prefix->length) {
    return false;
  }

  if (s->data == NULL || prefix->data == NULL) {
    return false;
  }

  return memcmp(s->data, prefix->data, prefix->length) == 0;
}

bool stringref_starts_with_cstr(const stringref_t* s, const char* prefix) {
  assert(s != NULL);

  stringref_t prefix_string = stringref_create(prefix);

  return stringref_starts_with(s, &prefix_string);
}

bool stringref_ends_with(const stringref_t* s, const stringref_t* suffix) {
  assert(s != NULL);

  if (s->length < suffix->length) {
    return false;
  }

  if (s->data == NULL || suffix->data == NULL) {
    return false;
  }

  const char* suffix_start = s->data + s->length - suffix->length;

  return memcmp(suffix_start, suffix->data, suffix->length) == 0;
}

bool stringref_ends_with_cstr(const stringref_t* s, const char* suffix) {
  assert(s != NULL);

  stringref_t suffix_string = stringref_create(suffix);

  return stringref_ends_with(s, &suffix_string);
}

size_t stringref_increment(stringref_t* s) {
  assert(s != NULL);

  if (stringref_is_empty(s)) {
    return 0;
  }

  ++s->data;
  --s->length;

  return s->length;
}

size_t stringref_increment_n(stringref_t* s, size_t n) {
  assert(s != NULL);

  if (stringref_is_empty(s)) {
    return 0;
  }

  if (s->length < n) {
    return 0;
  }

  s->data += n;
  s->length -= n;

  return s->length;
}

