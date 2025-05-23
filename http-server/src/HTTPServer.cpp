#include "../include/HTTPServer.h"
#include "../include/Error.h"
#include "../include/Logger.h"
#include "../include/Utils.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include <wait.h>

namespace http {

static inline void throwStatusOnErrno() {
  switch (errno) {
  case ENOENT:
    throw Error::HTTPWithStatus(404, "Not Found");
  case EACCES:
    throw Error::HTTPWithStatus(403, "Forbidden");
  default:
    throw Error::HTTPWithStatus(400, "Bad Request");
  }
}

static inline bool isHTML(const std::string &path) {
  const std::string suffix = ".html";
  if (path.size() < suffix.size())
    return false;

  for (size_t i = 0; i < suffix.size(); ++i) {
    char a = std::tolower(
        static_cast<unsigned char>(path[path.size() - suffix.size() + i]));
    char b = suffix[i];
    if (a != b)
      return false;
  }
  return true;
}

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
      LOG_INFO("Serving client");
      close(serverSocket.get());
      handleConnection(clientSocket);
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

void HTTPServer::handleConnection(Socket &clientSocket) {
  constexpr size_t BUFSIZE = 4096;
  char buffer[BUFSIZE];
  std::string requestPayload;
  ssize_t bytesRead;

  while ((bytesRead = read(clientSocket.get(), buffer, BUFSIZE - 1)) > 0) {
    requestPayload.append(buffer, bytesRead);
    if (requestPayload.find("\r\n\r\n") != std::string::npos) {
      break;
    }
  }

  if (bytesRead < 0) {
    LOG_ERROR("Error reading");
    sendError(clientSocket.get(), 500, "Internal Server Error");
    return;
  }

  try {
    HTTPRequest req = HTTPRequest::parse(requestPayload);
    HTTPResponse resp{handleRequest(req)};
    sendResponse(clientSocket.get(), resp);
  }

  catch (const Error::HTTPWithStatus &e) {
    LOG_WARNING("Invalid HTTP Request: %d %s", e.status,
                std::string(e.what()).c_str());
    sendError(clientSocket.get(), e.status, e.what());
  }

  catch (const std::exception &e) {
    LOG_ERROR("Unexpected error: %s", std::string(e.what()).c_str());
    sendError(clientSocket.get(), 500, "Internal Server Error");
  }
}

HTTPResponse HTTPServer::handleRequest(const HTTPRequest &req) const {
  std::string path = req.getEndpoint();
  struct stat statBuffer;

  path.erase(0, 1);
  if (path.empty()) {
    path = ".";
  }

  std::vector<const char *> argv;
  std::string execOut;
  std::string contentType;

  if (stat(path.c_str(), &statBuffer) < 0) {
    throwStatusOnErrno();
  }

  if (S_ISREG(statBuffer.st_mode)) {
    argv = {"cat", path.c_str(), nullptr};
    execOut = execute(argv);
    contentType = isHTML(path) ? "text/html" : "application/octet-stream";
  }

  else if (S_ISDIR(statBuffer.st_mode)) {
    std::string indexPath = path + "/index.html";

    if (stat(indexPath.c_str(), &statBuffer) == 0 &&
        S_ISREG(statBuffer.st_mode)) {
      argv = {"cat", indexPath.c_str(), nullptr};
      execOut = execute(argv);
      contentType = "text/html";
    }

    else {
      argv = {"ls", "-l", path.c_str(), nullptr};
      execOut = execute(argv);
      contentType = "application/octet-stream";
    }
  }

  else {
    throw Error::HTTPWithStatus(403, "Forbidden");
  }

  HTTPResponse resp;
  resp.addHeader("Content-Type", contentType);
  resp.setBody(execOut);
  return resp;
}
void HTTPServer::sendError(int sockFd, int status, const std::string &reason,
                           const std::string &body) {
  HTTPResponse resp(status, reason);
  resp.addHeader("Content-Type", "text/plain");
  resp.setBody(body);
  sendResponse(sockFd, resp);
}

void HTTPServer::sendResponse(int sockFd, const HTTPResponse &resp) {
  std::string out = resp.toString();
  write(sockFd, out.c_str(), out.length());
}

void HTTPServer::throwErrno(const std::string &what) {
  throw Error::HTTP(what + ": " + std::string(std::strerror(errno)));
}

}; // namespace http
