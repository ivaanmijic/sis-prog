#include "inc/scanner.h"
#include "inc/constants.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 10

char **tokenize(char *buffer, int *num_tokens) {
  const char *whitespace = " \t\n\f\r\v";

  int capacity = INITIAL_TOKEN_CAPACITY;

  char **tokens = malloc(capacity * sizeof(char *));
  if (tokens == NULL) {
    return NULL;
  }

  *num_tokens = 0;
  char *token = strtok(buffer, whitespace);
  while (token != NULL) {
    if (*num_tokens >= capacity - 1) {
      capacity *= 2;
      char **temp = realloc(tokens, capacity * sizeof(char *));
      if (temp == NULL) {
        free(tokens);
        return NULL;
      }
      tokens = temp;
    }

    tokens[*num_tokens] = token;
    (*num_tokens)++;
    token = strtok(NULL, whitespace);
  }
  tokens[*num_tokens] = NULL;
  return tokens;
}

int is_valid(const char *cmd) {
  if (strcmp(cmd, CMD_ECHO) == 0 || strcmp(cmd, CMD_CAT) == 0 ||
      strcmp(cmd, CMD_TOUCH) == 0 || strcmp(cmd, CMD_LS) == 0) {
    return 1;
  }
  return 0;
}
