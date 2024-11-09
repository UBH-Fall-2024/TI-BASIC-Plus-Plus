#ifndef FILE_TYPE_H
#define FILE_TYPE_H

#include <stdio.h>
#include <ti-basic-plus-plus/basic/diagnostics.h>

typedef enum {
  FILE_TYPE_MISSING,
  FILE_TYPE_DIRECTORY,
  FILE_TYPE_REGULAR,
  FILE_TYPE_UNKNOWN,
} file_type_t;

file_type_t get_file_type(const char* path);

void validate_file_exists(const char* path, diagnostics_t* d);

typedef enum {
  FILE_EXT_NONE,
  FILE_EXT_8XP,
  FILE_EXT_TIBASIC,
  FILE_EXT_TIBASICPP,
  FILE_EXT_UNKNOWN,
} file_extension_t;

file_extension_t get_file_extension(const char* path, const char** extension);

void validate_8xp_filename(const char* filename,
                           const char* extension,
                           char program_name[8],
                           diagnostics_t* d);

#endif  // FILE_TYPE_H

