#include "../include/Socket.h"
#include <unistd.h>

Socket::Socket(int fd) : _fd(fd) {}

Socket::Socket(Socket &&other) : _fd(other._fd) { other._fd = -1; }

Socket &Socket::operator=(Socket &&other) {
  if (this != &other) {
    if (_fd >= 0)
      close(_fd);
    _fd = other._fd;
    other._fd = -1;
  }
  return *this;
}

Socket::~Socket() {
  if (_fd >= 0)
    close(_fd);
}

int Socket::get() const { return _fd; }
