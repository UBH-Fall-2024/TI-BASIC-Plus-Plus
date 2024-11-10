#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  FILE* stream;
} subprocess_t;

// Runs a subprocess with the given arguments.
// argv must be a stb_ds dynamic array, and argv[0] must be the program name.
// Reutrns true if successful, false otherwise.
bool begin_subprocess(const char** argv, subprocess_t* subprocess);

// Waits until the subprocess finishes.
// If print_output is true, the subprocess output is printed to stdout.
// Returns true if the subprocess exited successfully, false otherwise.
bool end_subprocess(subprocess_t* subprocess, bool print_output);

#endif  // PROCESS_H

