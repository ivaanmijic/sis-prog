#include "../include/HttpServer.h"
#include "../include/Logger.h"
#include "../include/error.h"

#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <wait.h>

HTTPServer::HTTPServer(const Port &p) : port(p), serverSocket(-1) {}

HTTPServer::HTTPServer(Port &&p) : port(std::move(p)), serverSocket(-1) {}

// MARK: - Public interface

void HTTPServer::init() {
  setupSignalAction();
  createSocket();
  configureSocketOptions();
  bindSocket();
}

void HTTPServer::listenAndServe() {
  listen();
  serve();
}

// MARK: - Private helpers

void HTTPServer::setupSignalAction() {
  struct sigaction sa;
  sa.sa_handler = reapZombie;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, nullptr) < 0)
    throw Error::HTTP("sigaction failure: " + std::string(strerror(errno)));
}

void HTTPServer::reapZombie(int sig) {
  while (waitpid(-1, nullptr, WNOHANG) > 0) {
  }
}

void HTTPServer::createSocket() {
  int raw_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (raw_fd < 0)
    throwErrno("Failed to create socket");
  serverSocket = Socket{raw_fd};
}

void HTTPServer::configureSocketOptions() {
  if (setsockopt(serverSocket.get(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)) < 0)
    throwErrno("setsockopt failure");
}

void HTTPServer::bindSocket() {
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port.get());

  if (bind(serverSocket.get(), reinterpret_cast<struct sockaddr *>(&address),
           sizeof(address)) < 0)
    throwErrno("bind failure");
}

void HTTPServer::listen() {
  if (::listen(serverSocket.get(), kBacklog) < 0)
    throwErrno("listen failure");
  LOG_INFO("Listening on port %d...", port.get());
}

void HTTPServer::serve() {
  while (true) {
    Socket clientSocket = acceptConnection();
    if (clientSocket.get() == -1) {
      continue;
    }

    pid_t pid = fork();
    if (pid < 0) {
      LOG_ERROR("fork failure: %s", std::strerror(errno));
      continue;
    } else if (pid == 0) {
      close(serverSocket.get());
      LOG_INFO("Serving client");
      close(clientSocket.get());
      _exit(EXIT_SUCCESS);
    }
  }
}

Socket HTTPServer::acceptConnection() {
  struct sockaddr_in client_addr;
  socklen_t clientLen = sizeof(client_addr);

  int clientFd =
      accept(serverSocket.get(),
             reinterpret_cast<struct sockaddr *>(&client_addr), &clientLen);
  if (clientFd < 0) {
    LOG_ERROR("Accept failure: %s\n", std::strerror(errno));
    return Socket{-1};
  }

  char ipStr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(client_addr.sin_addr), ipStr, INET_ADDRSTRLEN);

  LOG_INFO("Accepted new connection from: %s", ipStr);
  return Socket{clientFd};
}

void HTTPServer::throwErrno(const std::string &what) {
  throw Error::HTTP(what + ": " + std::string(std::strerror(errno)));
}
