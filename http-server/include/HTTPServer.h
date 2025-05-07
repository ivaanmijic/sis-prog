#pragma once

#include "../include/HTTPRequest.h"
#include "../include/HTTPResponse.h"
#include "../include/Port.h"
#include "../include/Socket.h"

#include <cstdio>
#include <functional>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <utility>
#include <vector>

namespace http {

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

  void handleConnection(Socket &clinetSock);

  HTTPResponse handleRequest(const HTTPRequest &req) const;
  void sendError(int sockFd, int status, const std::string &reason,
                 const std::string &body = "");
  void sendResponse(int sockFd, const HTTPResponse &resp);

  static void reapZombie(int sig);
  static void throwErrno(const std::string &what);

  static int constexpr kBacklog = 10;

  Port port;
  int opt = 1;
  Socket serverSocket;
  struct sockaddr_in address;
};

} // namespace http
