#ifndef UTILS_H
#define UTILS_H

#define BUFFSIZE 512

typedef void (*command_func)(char **);

typedef struct {
  const char *name;
  command_func func;
} Command;

void execute(const char *cmd, char **args, int num_args);

void mojecho(char **args);
void mojcat(char **args);
void mojtouch(char **args);
void mojls(char **args);

int remove_redirections(char **args, int num_args, char *inputFile,
                        int *inputRedirect, char *outputFile,
                        int *outputRedirect);

#endif
