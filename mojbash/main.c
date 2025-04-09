#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "inc/constants.h"
#include "inc/error.h"
#include "inc/scanner.h"
#include "inc/shell_env.h"
#include "inc/utils.h"

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
  if (!isatty(STDIN_FILENO)) {
    fatal("STDIN is not a tty");
  }

  ShellEnv *env = new_env();
  char buff[MAX_LINE];
  char **tokens = NULL;

  while (1) {
    display_prompt(env);
    fgets(buff, MAX_LINE, stdin);

    int num_tokens = 0;
    tokens = tokenize(buff, &num_tokens);
    if (num_tokens == 0) {
      free(tokens);
      continue;
    }

    char *cmd = tokens[0];

    if (strcmp(cmd, CMD_EXIT) == 0) {
      free(tokens);
      if (num_tokens > 2) {
        fprintf(stderr, "exit: too many arguments\n");
      } else
        break;
    }

    if (!is_valid(cmd)) {
      fprintf(stderr, "mojbash: command not found: %s\n", cmd);
      free(tokens);
      continue;
    }

    char **args = tokens + 1;
    int num_args = num_tokens - 1;

    pid_t cpid = fork();
    if (cpid == 0) {
      execute(cmd, args, num_args);
      exit(EXIT_SUCCESS);
    } else {
      int status;
      waitpid(cpid, &status, 0);
      free(tokens);
    }
  }

  free_env(env);

  return EXIT_SUCCESS;
}
