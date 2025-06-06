#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void fatal(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(void) {
  int server_fd;
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    fatal("socket() failure");

  int opt = -1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) < 0)
    fatal("setsockopt() failure");

  struct sockaddr_in addr_in;
  memset(&addr_in, '\0', sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_addr.s_addr = INADDR_ANY;
  addr_in.sin_port = htons(7070);

  if (bind(server_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0)
    fatal("bind failure");

  listen(server_fd, 1);

  while (1) {
    int clinetFd = accept(server_fd, NULL, 0);
    if (clinetFd < 0)
      fatal("accept() failure\n");

    sleep(25);

    char buffer[BUFSIZ];
    int nread = recv(clinetFd, buffer, BUFSIZ, 0);
    close(clinetFd);
    if (nread <= 0) {
      break;
    } else {
      printf("timeout je istekao");
    }
  }
  close(server_fd);

  return EXIT_SUCCESS;
}
