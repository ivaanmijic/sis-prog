#pragma once

#include <linux/limits.h>

#define BUFFSIZE 512

typedef struct {
  char *cwd;
  char *home_dir;
  char *hostname;
  char *username;
} ShellEnv;

ShellEnv *new_env();
void free_env(ShellEnv *env);

char *retrieve_cwd();
char *retrieve_homeDir();
char *retrieve_username();
char *retrieve_hostname();

void display_prompt(ShellEnv *env);
void format_path(const char *cwd, const char *homeDir, char *path);
