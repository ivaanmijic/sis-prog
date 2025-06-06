#include "../include/RelayServer.h"
#include "../include/Error.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <experimental/filesystem>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

static inline std::string errnoString() { return std::string{strerror(errno)}; }

static inline void reapZombie(int sig) {
  while (waitpid(-1, nullptr, WNOHANG) > 0) {
  }
}

RelayServer::RelayServer(uint16_t p, const std::string &path)
    : port{p}, listenerFd{-1}, fifoPath{path} {}

// MARK: - Public interface

void RelayServer::init() {
  setupSignalHandler();
  createPipe();
  createFifo();
  forkReceiver();
  forkLogger();
  createListenerSocket();
  configureSocketOptions();
  bindSocket();
}

void RelayServer::run() {
  listen(listenerFd, 10);
  fd_set masterSet, readSet;
  FD_ZERO(&masterSet);
  FD_SET(listenerFd, &masterSet);
  int maxFd = listenerFd;

  while (true) {
    readSet = masterSet;
    if (select(maxFd + 1, &readSet, nullptr, nullptr, nullptr) < 0 &&
        errno != EINTR) {
      throw RelayError("select failure: " + errnoString());
    }

    for (int fd = 0; fd <= maxFd; ++fd) {
      if (!FD_ISSET(fd, &readSet))
        continue;

      if (fd == listenerFd) {
        int clientFd = accept(listenerFd, nullptr, nullptr);
        if (clientFd < 0)
          continue;
        FD_SET(clientFd, &masterSet);
        maxFd = std::max(maxFd, clientFd);
      } else {
        handleClientData(fd, masterSet);
      }
    }
  }
}

// MARK: - Private helpers

void RelayServer::setupSignalHandler() {
  struct sigaction sa;
  sa.sa_handler = reapZombie;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, nullptr) < 0)
    throw RelayError("sigaction failure: " + errnoString());
}

void RelayServer::createPipe() {
  if (pipe(pipeFd) < 0)
    throw RelayError{"pipe failure: " + errnoString()};
}

void RelayServer::createFifo() {
  unlink(fifoPath.c_str());
  if (mkfifo(fifoPath.c_str(), 0666) < 0 && errno != EEXIST) {
    close(pipeFd[0]);
    close(pipeFd[1]);
    throw RelayError{"mkfifo failure: " + errnoString()};
  }
}

void RelayServer::forkReceiver() {
  pid_t pid = fork();
  if (pid == 0) {
    close(pipeFd[1]);
    char buf[BUFSIZ];
    ssize_t nbytes;
    while ((nbytes = read(pipeFd[0], buf, BUFSIZ)) > 0) {
      std::cout.write(buf, nbytes);
    }
    _exit(0);
  }
  close(pipeFd[0]);
}

void RelayServer::forkLogger() {
  int loggerFd =
      open("logger.log", O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0666);
  if (loggerFd < 0) {
    close(pipeFd[1]);
    unlink(fifoPath.c_str());
    throw RelayError("logger open failure: " + errnoString());
  }

  pid_t pid = fork();
  if (pid == 0) {
    int fd = open(fifoPath.c_str(), O_RDONLY);
    char buf[BUFSIZ];
    ssize_t nbytes;
    while ((nbytes = read(fd, buf, BUFSIZ)) > 0) {
      write(loggerFd, buf, nbytes);
    }
    _exit(0);
  }
}

void RelayServer::createListenerSocket() {
  listenerFd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenerFd < 0)
    throw RelayError("socker() failure: " + errnoString());
}

void RelayServer::configureSocketOptions() {
  if (setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) < 0) {
    close(listenerFd);
    throw RelayError("setsockopt failure: " + errnoString());
  }
}

void RelayServer::bindSocket() {
  struct sockaddr_in address{};
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(listenerFd, reinterpret_cast<struct sockaddr *>(&address),
           sizeof(address)) < 0) {
    close(listenerFd);
    throw RelayError("bind failure: " + errnoString());
  }
}

void RelayServer::handleClientData(int clientFd, fd_set &masterSet) {
  char buf[BUFSIZ];
  ssize_t nbytes = read(clientFd, buf, BUFSIZ);
  if (nbytes <= 0) {
    close(clientFd);
    FD_CLR(clientFd, &masterSet);
  } else {
    write(pipeFd[1], buf, nbytes);
    int loggerFd = open(fifoPath.c_str(), O_WRONLY | O_NONBLOCK);
    if (loggerFd >= 0) {
      write(loggerFd, buf, nbytes);
      close(loggerFd);
    }
  }
}
