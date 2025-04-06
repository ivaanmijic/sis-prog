#include "inc/cmd_parser.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 10

char **tokenize(char *buffer, int *num_tokens) {
  const char *whitespace = " \t\n\f\r\v";

  int capacity = INITIAL_TOKEN_CAPACITY;
  char **tokens = malloc(capacity * sizeof(char *));

  *num_tokens = 0;

  char *token = strtok(buffer, whitespace);
  while (token != NULL) {
    if (*num_tokens >= capacity) {
      capacity *= 2;
      tokens = realloc(tokens, capacity * sizeof(char *));
    }

    tokens[*num_tokens] = token;
    (*num_tokens)++;
    token = strtok(NULL, whitespace);
  }

  return tokens;
}
