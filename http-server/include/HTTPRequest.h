#pragma once

#include <sstream>
#include <string>

namespace http {

class HTTPRequest {
public:
  static HTTPRequest parse(const std::string &payload);

  const std::string &getMehtod() const { return method; }
  const std::string &getEndpoint() const { return endpoint; }
  const std::string &getVersion() const { return version; }

private:
  std::string method;
  std::string endpoint;
  std::string version;

  static void parseRequestLine(const std::string &line, HTTPRequest &req);
};

}; // namespace http
