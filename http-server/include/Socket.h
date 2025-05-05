#pragma once

class Socket {
public:
  explicit Socket(int fd);
  Socket(Socket &&);
  Socket(const Socket &) = delete;
  Socket &operator=(Socket &&);
  Socket &operator=(const Socket &) = delete;
  ~Socket();

  int get() const;

private:
  int _fd;
};
