#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inc/cmd_parser.h"
#include "inc/error.h"
#include "inc/shell_env.h"

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
  if (!isatty(STDIN_FILENO)) {
    fatal("STDIN is not a tty");
  }

  ShellEnv *env = new_env();
  char buff[MAX_LINE];

  while (1) {
    display_prompt(env);
    fgets(buff, MAX_LINE, stdin);

    int num_tokens = 0;
    char **tokens = tokenize(buff, &num_tokens);
    if (num_tokens == 0) {
      free(tokens);
      continue;
    }

    if (strcmp(tokens[0], "exit") == 0) {
      exit(EXIT_SUCCESS);
    } else if (strcmp(tokens[0], "mojcat") == 0) {
    } else {
      fprintf(stderr, "mojbash: command not found: %s\n", tokens[0]);
    }
  }

  free_env(env);

  return EXIT_SUCCESS;
}
