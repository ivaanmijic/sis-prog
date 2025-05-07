#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

class RelayServer {
public:
  RelayServer(uint16_t p, const std::string &loggerPath);

  void init();
  void run();

private:
  void setupSignalHandler();
  void createPipe();
  void createFifo();
  void forkReceiver();
  void forkLogger();
  void createListenerSocket();
  void configureSocketOptions();
  void bindSocket();

  void handleClientData(int clientFd, fd_set &masterSet);

  uint16_t port;
  int listenerFd;
  std::string fifoPath;
  int pipeFd[2];
  int opt = -1;
};
