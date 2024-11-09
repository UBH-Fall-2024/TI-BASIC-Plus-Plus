#include <ti-basic-plus-plus/basic/source_location.h>

bool range_validate(const source_range_t* range) {
  assert(range != NULL);

  if (range->file == NULL) {
    return false;
  }

  if (range->begin.line < 1 || range->begin.column < 1) {
    return false;
  }

  if (range->end.line < 1 || range->end.column < 1) {
    return false;
  }

  if (range->begin.line > range->end.line) {
    return false;
  }

  if (range->begin.line == range->end.line &&
      range->begin.column > range->end.column) {
    return false;
  }

  return true;
}

source_range_t range_create(struct input_file* file,
                            source_position_t begin,
                            source_position_t end) {
  assert(file != NULL);
  assert(are_positions_equal(&begin, &end) || begin.line < end.line ||
         (begin.line == end.line && begin.column <= end.column));

  return (source_range_t){
      .file = file,
      .begin = begin,
      .end = end,
  };
}

source_range_t range_at_current(input_file_iterator_t* it) {
  assert(it != NULL);

  return range_at(it->file, if_get_position(it));
}

source_range_t range_cat(source_range_t* begin, source_range_t* end) {
  assert(range_validate(begin));
  assert(range_validate(end));

  assert(begin->file == end->file);
  assert(begin->end.line < end->begin.line ||
         (begin->end.line == end->begin.line &&
          begin->end.column <= end->begin.column));

  source_range_t result = *begin;
  result.end = end->end;

  return result;
}

