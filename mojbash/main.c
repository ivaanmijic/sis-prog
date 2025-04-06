#include "inc/shell_env.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (isatty(STDIN_FILENO)) {
    ShellEnv *env = new_env();
    display_prompt(env);

    free_env(env);
  }

  return EXIT_SUCCESS;
}
