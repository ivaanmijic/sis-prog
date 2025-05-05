#pragma once

#include "../include/Port.h"
#include "../include/Socket.h"

#include <cstdio>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

class HTTPServer {
public:
  explicit HTTPServer(const Port &p);
  explicit HTTPServer(Port &&p);

  void init();
  void listenAndServe();

private:
  void setupSignalAction();
  void createSocket();
  void configureSocketOptions();
  void bindSocket();
  void listen();
  void serve();

  Socket acceptConnection();

  static void reapZombie(int sig);
  static void throwErrno(const std::string &what);

  static int constexpr kBacklog = 10;

  Port port;
  int opt = 1;
  Socket serverSocket;
  struct sockaddr_in address;
};
