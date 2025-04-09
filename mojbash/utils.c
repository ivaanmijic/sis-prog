#include "inc/utils.h"
#include "inc/constants.h"
#include "inc/error.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILESIZE 64
#define BUFFSIZE 512

Command commands[] = {{CMD_ECHO, mojecho},
                      {CMD_CAT, mojcat},
                      {CMD_TOUCH, mojtouch},
                      {CMD_LS, mojls}};

void execute(const char *cmd, char **args, int num_args) {
  int num_commands = sizeof(commands) / sizeof(commands[0]);
  for (int i = 0; i < num_commands; ++i) {
    if (strcmp(cmd, commands[i].name) == 0) {

      int fd_in, fd_out;
      char inputFile[FILESIZE] = {0}, outputFile[FILESIZE] = {0};
      int inputRedirect = 0, outputRedirect = 0;

      for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "<") == 0 && args[i + 1] != NULL) {
          strncpy(inputFile, args[i + 1], FILESIZE - 1);
          args[i] = args[i + 1] = "";
          inputRedirect = 1;
        }
        if (strcmp(args[i], ">") == 0 && args[i + 1] != NULL) {
          strncpy(outputFile, args[i + 1], FILESIZE - 1);
          args[i] = args[i + 1] = "";
          outputRedirect = 1;
        }
      }

      if (inputRedirect) {
        fd_in = open(inputFile, O_RDONLY);
        if (fd_in < 0 && errno == ENOENT) {
          fprintf(stderr, "mojbash: no such file or directory: %s\n",
                  inputFile);
          return;
        }
        if (dup2(fd_in, STDIN_FILENO) < 0)
          fatal("Failed to redirect input");
        close(fd_in);
      }

      if (outputRedirect) {
        fd_out = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0)
          fatal("Failed to open output file");
        if (dup2(fd_out, STDOUT_FILENO) < 0)
          fatal("Failet to redirect output");
        close(fd_out);
      }

      commands[i].func(args, num_args);
    }
  }
}

void mojecho(char **args, int num_args) {
  for (int i = 0; i < num_args; ++i) {
    if (strlen(args[i]) == 0)
      continue;
    write(STDOUT_FILENO, args[i], strlen(args[i]));
    write(STDOUT_FILENO, " ", 1);
  }
  write(STDOUT_FILENO, "\n", 1);
}

void mojcat(char **args, int num_args) {
  for (int i = 0; i < num_args; ++i) {
    if (strlen(args[i]) == 0)
      continue;
    int fd = open(args[i], O_RDONLY);
    if (fd < 0 && errno == ENOENT) {
      fprintf(stderr, "mojcat: %s: No such file or directory\n", args[0]);
    } else {
      int n;
      char read_buff[BUFFSIZE];
      while ((n = read(fd, read_buff, BUFFSIZE)) > 0) {
        write(STDOUT_FILENO, read_buff, n);
      }
      if (n < 0 && errno == EISDIR) {
        fprintf(stderr, "cat: %s: Is a directory\n", args[i]);
      } else if (n < 0) {
        fprintf(stderr, "cat error: %d", errno);
      }
    }
  }
}
void mojtouch(char **args, int num_args) {}
void mojls(char **args, int num_args) {}
