#include "inc/error.h"
#include <stdio.h>
#include <stdlib.h>

void fatal(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void print_error(const char *cmd, const char *arg, const char *msg) {
  fprintf(stderr, "%s: %s: %s\n", cmd, arg, msg);
}
