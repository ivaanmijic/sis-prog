#include "inc/shell_env.h"
#include "inc/error.h"
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ShellEnv *new_env() {
  ShellEnv *env = malloc(sizeof(ShellEnv));
  env->cwd = retrieve_cwd();
  env->home_dir = retrieve_homeDir();
  env->username = retrieve_username();
  env->hostname = retrieve_hostname();
  return env;
}

void free_env(ShellEnv *env) {
  free(env->cwd);
  free(env->hostname);
  free(env);
}

char *retrieve_cwd() {
  char *buff = malloc(PATH_MAX);
  if (!(getcwd(buff, PATH_MAX)))
    fatal("get_current_dir_name() failure");
  return buff;
}

char *retrieve_homeDir() {
  char *buff = getenv("HOME");
  return buff;
}

char *retrieve_username() {
  char *login = getlogin();
  if (!login)
    fatal("getlogin() failure");
  return login;
}

char *retrieve_hostname() {
  char *host = malloc(HOST_NAME_MAX);
  if (gethostname(host, HOST_NAME_MAX) < 0)
    fatal("gethostname() failure");
  return host;
}

void display_prompt(ShellEnv *env) {
  char *path = malloc(PATH_MAX);
  format_path(env->cwd, env->home_dir, path);

  char buff[BUFFSIZE];
  sprintf(buff, "\033[32m%s@%s:\033[36m%s\033[0m$", env->username,
          env->hostname, path);
  free(path);

  int nbytes = strlen(buff);

  if (write(STDOUT_FILENO, buff, nbytes) != nbytes) {
    fatal("write() failure");
  }
}

void format_path(const char *cwd, const char *home_dir, char *path) {
  if (home_dir && strncmp(cwd, home_dir, strlen(home_dir)) == 0) {
    sprintf(path, "~%s", cwd + strlen(home_dir));
  } else {
    strcpy(path, cwd);
  }
}
