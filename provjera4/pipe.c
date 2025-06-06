#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE 1024

void fatal(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int pipeFd1[2];
  int pipeFd2[2];

  pid_t child_pid;

  if (pipe(pipeFd1) < 0 || pipe(pipeFd2) < 0)
    fatal("pipe() failure");

  child_pid = fork();
  if (child_pid < 0)
    fatal("fork() failure");

  if (child_pid == 0) {
    close(pipeFd1[1]);
    close(pipeFd2[0]);
    char *writeBuff = "to parent";
    char readBuff[BUFFERSIZE];
    write(pipeFd2[1], writeBuff, strlen(writeBuff));
    size_t nread = read(pipeFd1[0], readBuff, BUFFERSIZE);
    if (nread < 0)
      fatal("read error");
    close(pipeFd1[0]);
    close(pipeFd2[1]);
    printf("child: %s\n", readBuff);
    exit(EXIT_SUCCESS);
  }

  close(pipeFd1[0]);
  close(pipeFd2[1]);
  char *writeBuff = "to child";
  char readBuff[BUFFERSIZE];
  write(pipeFd1[1], writeBuff, strlen(writeBuff));
  size_t nread = read(pipeFd2[0], readBuff, BUFFERSIZE);
  if (nread < 0)
    fatal("read error");
  close(pipeFd1[0]);
  close(pipeFd2[1]);

  printf("parent: %s\n", readBuff);

  return EXIT_SUCCESS;
}
