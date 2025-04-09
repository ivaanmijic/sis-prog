#ifndef UTILS_H
#define UTILS_H

#define BUFFSIZE 512

typedef void (*command_func)(char **, int);

typedef struct {
  const char *name;
  command_func func;
} Command;

void execute(const char *cmd, char **args, int num_args);

void mojecho(char **args, int num_args);
void mojcat(char **args, int num_args);
void mojtouch(char **args, int num_args);
void mojls(char **args, int num_args);

#endif
