#include "inc/utils.h"
#include "inc/constants.h"
#include "inc/error.h"
#include <asm-generic/errno-base.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <langinfo.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define FILESIZE 64
#define BUFFSIZE 512
#define DATESIZE 256

static char get_inode_type(mode_t mode) {
  if (S_ISREG(mode))
    return '-';
  else if (S_ISDIR(mode))
    return 'd';
  else if (S_ISLNK(mode))
    return 'l';
  else if (S_ISCHR(mode))
    return 'c';
  else if (S_ISBLK(mode))
    return 'b';
  else if (S_ISFIFO(mode))
    return 'p';
  else if (S_ISSOCK(mode))
    return 's';
  else
    return '?';
}

Command commands[] = {{CMD_ECHO, mojecho},
                      {CMD_CAT, mojcat},
                      {CMD_TOUCH, mojtouch},
                      {CMD_LS, mojls}};

void parse_redirection(char *args[], int *p_num_args, char inputFile[FILESIZE],
                       int *p_inputRedirect, char outputFile[FILESIZE],
                       int *p_outputRedirect) {

  int i = 0;
  int num_args = *p_num_args;
  *p_inputRedirect = *p_outputRedirect = 0;
  inputFile[0] = outputFile[0] = '\0';

  while (i < num_args) {
    if (i + 1 < num_args && strcmp(args[i], "<") == 0) {
      strncpy(inputFile, args[i + 1], FILESIZE - 1);
      inputFile[FILESIZE - 1] = '\0';
      *p_inputRedirect = 1;

      memmove(&args[i], &args[i + 2], (num_args - (i + 2)) * sizeof(char *));
      num_args -= 2;
    }

    else if (i + 1 < num_args && strcmp(args[i], ">") == 0) {
      strncpy(outputFile, args[i + 1], FILESIZE - 1);
      outputFile[FILESIZE - 1] = '\0';
      *p_outputRedirect = 1;

      memmove(&args[i], &args[i + 2], (num_args - (i + 2)) * sizeof(char *));
      num_args -= 2;
    }

    else
      i++;
  }

  args[num_args] = NULL;
  *p_num_args = num_args;
}

void execute(const char *programe, char **args, int num_args) {
  int num_commands = sizeof(commands) / sizeof(commands[0]);

  for (int i = 0; i < num_commands; ++i) {
    if (strcmp(programe, commands[i].name) == 0) {
      int fd_in, fd_out;
      char inputFile[FILESIZE] = {0};
      char outputFile[FILESIZE] = {0};
      int inputRedirect = 0;
      int outputRedirect = 0;

      parse_redirection(args, &num_args, inputFile, &inputRedirect, outputFile,
                        &outputRedirect);

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
        fd_out = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd_out < 0)
          fatal("Failed to open output file");
        if (dup2(fd_out, STDOUT_FILENO) < 0)
          fatal("Failet to redirect output");
        close(fd_out);
      }

      commands[i].func(args);
      break;
    }
  }
}

void mojecho(char **strings) {
  while (*strings) {
    write(STDOUT_FILENO, *strings, strlen(*strings));
    write(STDOUT_FILENO, " ", 1);
    strings++;
  }
  write(STDOUT_FILENO, "\n", 1);
}

void mojcat(char **fnames) {
  char buffer[BUFFSIZE];
  const char *progname = "mojcat";
  ssize_t nread;

  if (*fnames == NULL) {
    while ((nread = read(STDIN_FILENO, buffer, BUFFSIZE)) > 0) {
      if (write(STDOUT_FILENO, buffer, nread) < 0) {
        print_error(progname, "write", strerror(errno));
        break;
      }
    }
  }

  while (*fnames) {
    const char *fname = *fnames;
    fnames++;

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
      if (errno == ENOENT)
        print_error(progname, fname, "No such file or directory");
      else if (errno == EISDIR)
        print_error(progname, fname, "Is a directory");
      else
        print_error(progname, fname, strerror(errno));
      continue;
    }

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

void mojtouch(char **args) {
  while (*args) {
    const char *path = *args;
    args++;

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

static void read_dir_info(const char *dirpath) {
  struct dirent *direntry;
  struct stat statbuf;
  struct tm *tm;
  char datestring[DATESIZE];
  DIR *dp = opendir(dirpath);
  if (dp == NULL) {
    print_error("mojls", "opendir", strerror(errno));
  }

  while ((direntry = readdir(dp)) != NULL) {
    char fullpath[256];
    snprintf(fullpath, sizeof(fullpath), dirpath, direntry->d_name);

    if (stat(fullpath, &statbuf) < 0) {
      continue;
    }

    fprintf(stdout, "%lu\t%c\t%lu\t%d\t%d\t%lu\t", statbuf.st_ino,
            get_inode_type(statbuf.st_mode), statbuf.st_nlink, statbuf.st_uid,
            statbuf.st_gid, statbuf.st_size);

    tm = localtime(&statbuf.st_mtime);
    strftime(datestring, sizeof(datestring), "%Y-%m-%d %H:%M", tm);

    fprintf(stdout, "%s\t%s\n", datestring, direntry->d_name);
  }

  closedir(dp);
};

void mojls(char **args) {
  if (*args == NULL) {
    char *dirname = ".";
    read_dir_info(dirname);
  }

  while (*args) {
    char *dirpath = *args;
    args++;
    read_dir_info(dirpath);
  }
}
