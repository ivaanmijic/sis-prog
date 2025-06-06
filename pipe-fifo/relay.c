#include <asm-generic/errno-base.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

#define FIFO "logger_fifo"
#define BACKLOG 10

void sigchld_handler(int sig) {
  while (waitpid(-1, NULL, WNOHANG)) {
    ;
  }
}

void fatal(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(void) {
  int pipeFd[2];
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, 0) < 0)
    fatal("sigaction failure");

  if (pipe(pipeFd) < 0) {
    fatal("pipe failure");
  }

  unlink(FIFO);
  if (mkfifo(FIFO, 0666) < 0 && errno != EEXIST)
    fatal("mkfifo failure");

  pid_t rec_pid = fork();
  if (rec_pid == 0) {
    close(pipeFd[1]);
    char buf[BUFSIZ];
    size_t nread;
    while ((nread = read(pipeFd[0], buf, BUFSIZ)) > 0)
      write(STDOUT_FILENO, buf, nread);
    close(pipeFd[0]);
    exit(EXIT_SUCCESS);
  }
  close(pipeFd[0]);

  int loggerFd =
      open("looger.log", O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0666);
  if (loggerFd < 0)
    fatal("open logger failure");

  pid_t logger_pid = fork();
  if (logger_pid == 0) {
    int fd = open(FIFO, O_RDONLY);
    char buf[BUFSIZ];
    size_t nread;
    while ((nread = read(fd, buf, BUFSIZ)) > 0)
      write(loggerFd, buf, nread);
    close(fd);
    exit(EXIT_SUCCESS);
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    fatal("socket error");

  int opt = -1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) < 0)
    fatal("setsockopt() failure");

  struct sockaddr_in addr_in;
  memset(&addr_in, '\0', sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_addr.s_addr = INADDR_ANY;
  addr_in.sin_port = htons(9000);

  if (bind(server_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0)
    fatal("bind() failure");

  listen(server_fd, BACKLOG);

  int fifoFd = open(FIFO, O_RDWR | O_NONBLOCK);
  if (fifoFd < 0)
    fatal("open() fifo failure");

  fd_set masterSet, readSet;
  FD_ZERO(&masterSet);
  FD_SET(server_fd, &masterSet);
  int max_fd = server_fd;

  while (1) {
    readSet = masterSet;
    if (select(max_fd + 1, &readSet, NULL, NULL, NULL) < 0 && errno != EINTR)
      fatal("select() failure");

    for (int fd = 0; fd <= max_fd; ++fd) {
      if (!FD_ISSET(fd, &readSet))
        continue;

      if (fd == server_fd) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
          continue;
        FD_SET(client_fd, &masterSet);
        if (client_fd > max_fd)
          max_fd = client_fd;
      }

      else {
        char buf[BUFSIZ];
        size_t nread = read(fd, buf, BUFSIZ);
        if (nread < 0) {
          close(fd);
          FD_CLR(fd, &masterSet);
        } else {
          write(pipeFd[1], buf, nread);
          int loggerFd = open(FIFO, O_RDWR | O_NONBLOCK);
          if (loggerFd >= 0) {
            write(loggerFd, buf, nread);
            close(loggerFd);
          }
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
