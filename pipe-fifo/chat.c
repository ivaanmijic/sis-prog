#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 9034

void fatal(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(void) {
  fd_set master;
  fd_set read_fds;
  struct sockaddr_in myaddr;
  struct sockaddr_in remoteaddr;
  int fd_max;
  int listener;
  int new_fd;
  char buf[256];
  int nbytes;
  int yes = 1;
  socklen_t addrlen;
  int i, j;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  if ((listener = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    fatal("socket");

  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes,
                 sizeof(int)) < 0)
    fatal("setsockopt");

  memset(&myaddr, '\0', sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = INADDR_ANY;
  myaddr.sin_port = htons(PORT);

  if (bind(listener, (struct sockaddr *)&myaddr, sizeof myaddr) < 0) {
    fatal("bind");
  }

  if (listen(listener, 10) < 0)
    fatal("listen");

  FD_SET(listener, &master);
  fd_max = listener;

  while (1) {
    read_fds = master;
    if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
      fatal("select");
    }

    for (i = 0; i < fd_max; ++i) {
      if (FD_ISSET(i, &read_fds)) {
        if (i == listener) {
          addrlen = sizeof(remoteaddr);
          if ((new_fd = accept(listener, (struct sockaddr *)&remoteaddr,
                               &addrlen)) < 0) {
            perror("accept");
          } else {
            FD_SET(new_fd, &master);
            if (new_fd > fd_max)
              fd_max = new_fd;
            printf("new connection from %s on socket %d\n",
                   inet_ntoa(remoteaddr.sin_addr), new_fd);
          }
        } else {
          if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
            if (nbytes == 0) {
              printf("selectserver: socket %d hang up\n", i);
            } else {
              perror("recv");
            }
            close(i);
            FD_CLR(i, &master);
          } else {
            for (j = 0; j <= fd_max; j++) {
              if (FD_ISSET(j, &master)) {
                if (j != listener && j != i) {
                  if (send(j, buf, nbytes, 0) == -1)
                    perror("send");
                }
              }
            }
          }
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
