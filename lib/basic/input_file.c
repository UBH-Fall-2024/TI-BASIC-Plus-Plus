#include <ti-basic-plus-plus/basic/input_file.h>

#include <stb_ds.h>
#include <stdlib.h>

bool if_init(input_file_t* f, const char* path) {
  assert(f != NULL);
  assert(path != NULL);

  f->path = path;

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    return false;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_size < 0) {
    fclose(file);
    return false;
  }

  f->size = (size_t)file_size;
  f->contents = (int*)malloc((file_size + 1) * sizeof(int));

  {
    f->line_data = NULL;
    line_data_t line_data = {0, 0};
    arrput(f->line_data, line_data);
  }

  size_t i = 0, line_start_index = 0;
  long position = ftell(file);
  int c = fgetc(file);
  bool omit_newline = false;
  while (i < f->size && c != EOF && !ferror(file)) {
    int next_c;
    if (feof(file) || (next_c = fgetc(file)) == EOF) {
      next_c = EOF;
    } else {
      (void)ungetc(next_c, file);
    }

    if (c == '\\' && next_c == '\n') { // Line splices
      omit_newline = true;
      goto NEXT_CHAR;
    }

    if (c == '\n') {
      arrlast(f->line_data).length = i - line_start_index + !omit_newline;
      line_start_index = i + !omit_newline;

      // Next line.
      {
        line_data_t line_data = {0, position + 1};
        arrput(f->line_data, line_data);
      }

      if (omit_newline) {
        omit_newline = false;
        goto NEXT_CHAR;
      }

      c = '\n';
    }

    assert(omit_newline == false);

    f->contents[i++] = c;

  NEXT_CHAR:
    position = ftell(file);
    c = fgetc(file);
  }

  (void)fclose(file);

  f->contents[i] = EOF;
  f->size = i + 1;
  arrlast(f->line_data).length = i - line_start_index;

  return true;
}

void if_destroy(input_file_t* f) {
  assert(f != NULL);

  if (f->contents != NULL) {
    free(f->contents);
  }

  arrfree(f->line_data);
}

char* if_get_line(input_file_t* f, size_t line) {
  assert(f != NULL);
  assert(line > 0);

  const size_t num_lines = arrlenu(f->line_data);

  if (line > num_lines) {
    return NULL;
  }

  FILE* file = fopen(f->path, "r");
  if (!file) {
    assert(false);
    return NULL;
  }

  size_t length = f->line_data[line - 1].length;
  if (!length) {
    (void)fclose(file);
    return NULL;
  }

  (void)fseek(file, f->line_data[line - 1].start_position, SEEK_SET);

  char* line_contents = (char*)malloc(length + 2);

  int c;
  size_t i = 0;
  while (!ferror(file) && !feof(file) && (c = fgetc(file)) != EOF &&
         c != '\r' && c != '\n' && i < length + 1) {
    line_contents[i] = (char)c;
    i++;
  }
  line_contents[i] = '\0';

  (void)fclose(file);

  return line_contents;
}

void if_iterator_init(input_file_iterator_t* it, input_file_t* file) {
  assert(it != NULL);
  assert(file != NULL);

  it->file = file;
  it->line = 1;
  it->column = 1;
  it->position = file->contents;
}

input_file_iterator_t if_iterator_create(input_file_t* file) {
  input_file_iterator_t it;
  if_iterator_init(&it, file);
  return it;
}

int if_next(input_file_iterator_t* it) {
  assert(it != NULL);

  const size_t num_lines = arrlenu(it->file->line_data);
  assert(it->line <= num_lines);

  if (*it->position == EOF ||
      (size_t)(it->position - it->file->contents) >= it->file->size) {
    return EOF;
  }

  const size_t line_length = it->file->line_data[it->line - 1].length;

  if ((it->column == line_length &&
       (*it->position == '\n' || *(it->position + 1) != '\n')) ||
      (it->column == line_length + 1)) {
    it->column = 1;
    while (it->line < num_lines) {
      ++it->line;

      // Skip empty lines spliced together with '\'
      if (it->file->line_data[it->line - 1].length) {
        break;
      }
    }
  } else {
    assert(it->column <= line_length);
    ++it->column;
  }

  return *++it->position;
}

