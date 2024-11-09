#include <ti-basic-plus-plus/basic/file_type.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

file_type_t get_file_type(const char* path) {
  struct stat s;
  if (stat(path, &s) < 0) {
    return FILE_TYPE_MISSING;
  }

  if (S_ISDIR(s.st_mode)) {
    return FILE_TYPE_DIRECTORY;
  }

  if (S_ISREG(s.st_mode)) {
    return FILE_TYPE_REGULAR;
  }

  return FILE_TYPE_UNKNOWN;
}

void validate_file_exists(const char* path, diagnostics_t* d) {
  file_type_t file_type = get_file_type(path);
  switch (file_type) {
    case FILE_TYPE_MISSING:
      diag_report_file(d, ERROR, path, "no such file");
      break;
    case FILE_TYPE_DIRECTORY:
      diag_report_file(d, ERROR, path, "is a directory");
      break;
    case FILE_TYPE_UNKNOWN:
      diag_report_file(d, ERROR, path, "not a regular file");
      break;
    case FILE_TYPE_REGULAR:
      break;
  }
}

file_extension_t get_file_extension(const char* path, const char** _extension) {
  const char* extension = strrchr(path, '.');
  if (_extension != NULL) {
    *_extension = extension;
  }
  if (extension == NULL) {
    return FILE_EXT_NONE;
  }

  if (strcmp(extension, ".8xp") == 0 || strcmp(extension, ".8Xp") == 0) {
    return FILE_EXT_8XP;
  }

  if (strcmp(extension, ".tibasic") == 0 ||
      strcmp(extension, ".TIBASIC") == 0) {
    return FILE_EXT_TIBASIC;
  }

  if (strcmp(extension, ".tibasic++") == 0 ||
      strcmp(extension, ".TIBASIC++") == 0) {
    return FILE_EXT_TIBASICPP;
  }

  return FILE_EXT_UNKNOWN;
}

void validate_8xp_filename(const char* filename,
                           const char* extension,
                           char program_name[8],
                           diagnostics_t* d) {
  assert(filename != NULL);
  assert(extension != NULL);
  assert(program_name != NULL);

  const char* file = strrchr(filename, '/');
  if (file == NULL) {
    file = filename;
  } else {
    file++;
  }
  assert(file != NULL);

  const size_t program_name_length = extension - file;
  if (program_name_length > 8) {
    diag_report_file(d, ERROR, file,
                     "output filename is longer than 8 characters");
    return;
  }
  if (!program_name_length) {
    diag_report_file(d, ERROR, file, "output filename cannot be empty");
    return;
  }

  for (size_t i = 0; i < program_name_length; ++i) {
    if (isalpha(file[i])) {
      if (islower(file[i])) {
        diag_report_file(d, ERROR, file, "output filename must be uppercase");
        return;
      }
      continue;
    }

    if (!isdigit(file[i])) {
      diag_report_file(d, ERROR, file, "output filename must be alphanumeric");
      return;
    }
  }

  (void)strncpy(program_name, file, program_name_length);
}

