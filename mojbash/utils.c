#include "inc/utils.h"
#include "inc/constants.h"
#include "inc/error.h"
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define FILESIZE 64
#define BUFFSIZE 512

Command commands[] = {{programe_ECHO, mojecho},
                      {programe_CAT, mojcat},
                      {programe_TOUCH, mojtouch},
                      {programe_LS, mojls}};

void execute(const char *programe, char **args, int num_args) {
  int num_commands = sizeof(commands) / sizeof(commands[0]);
  for (int i = 0; i < num_commands; ++i) {
    if (strcmp(programe, commands[i].name) == 0) {

      int fd_in, fd_out;
      char inputFile[FILESIZE] = {0}, outputFile[FILESIZE] = {0};
      int inputRedirect = 0, outputRedirect = 0;

      for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "<") == 0 && i + 1 < num_args && args[i + 1]) {
          strncpy(inputFile, args[i + 1], FILESIZE - 1);
          args[i] = args[i + 1] = "";
          inputRedirect = 1;
        }
        if (strcmp(args[i], ">") == 0 && i + 1 < num_args && args[i + 1]) {
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
  char buffer[BUFFSIZE];
  const char *progname = "mojcat";

  for (int i = 0; i < num_args; ++i) {
    char *fname = args[i];
    if (fname[0] == '\0')
      continue;

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
      if (errno == ENOENT)
        print_error(progname, fname, "No such file or directoru");
      else if (errno == EISDIR)
        print_error(progname, fname, "Is a directory");
      else
        print_error(progname, fname, strerror(errno));
      continue;
    }

    ssize_t nread;
    while ((nread = read(fd, buffer, BUFFSIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, nread) < 0) {
        print_error(progname, "write", strerror(errno));
        break;
      }
    }
    if (nread < 0) {
      print_error(progname, fname, strerror(errno));
    }

    close(fd);
  }
}

void mojtouch(char **args, int num_args) {
  for (int i = 0; i < num_args; ++i) {
    const char *path = args[i];
    if (path[0] == '\0')
      continue;

    int fd = open(path, O_CREAT | O_EXCL, 0666);
    if (fd >= 0) {
      close(fd);
      continue;
    }

    if (errno == EEXIST) {
      fd = open(path, O_WRONLY);
      if (fd < 0) {
        fprintf(stderr, "touch: cannot open '%s' for writing: %s\n", path,
                strerror(errno));
        continue;
      }
      struct timespec times[2];
      times[0].tv_nsec = UTIME_NOW;
      times[1].tv_nsec = UTIME_OMIT;

      if (futimens(fd, times) < 0) {
        fprintf(stderr, "touch: cannot update times on '%s': %s\n", path,
                strerror(errno));
      }
      close(fd);
    } else {
      fprintf(stderr, "touch: cannot create '%s': %s\n", path, strerror(errno));
    }
  }
}

void mojls(char **args, int num_args) {}
