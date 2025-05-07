#include "../include/HTTPRequest.h"
#include "../include/Error.h"
#include <cctype>
#include <cstddef>
#include <regex>
#include <sstream>
#include <string>

namespace http {

HTTPRequest HTTPRequest::parse(const std::string &raw) {
  HTTPRequest req;
  size_t pos = raw.find("\r\n\r\n");
  std::string head =
      raw.substr(0, pos == std::string::npos ? raw.size() : pos + 2);
  std::istringstream in(head);
  std::string line;

  if (!std::getline(in, line) || line.empty())
    throw Error::HTTPWithStatus(400, "Bad Request");
  parseRequestLine(line, req);
  return HTTPRequest(req);
}

void HTTPRequest::parseRequestLine(const std::string &line, HTTPRequest &req) {
  std::istringstream rl{line};
  rl >> req.method >> req.endpoint >> req.version;

  if (req.method.empty() || req.endpoint.empty() || req.version.empty())
    throw Error::HTTPWithStatus(400, "Bad Request");

  static const std::regex abs_url_re{R"(^(?:https?:)?\/\/)"};
  if (std::regex_search(req.endpoint, abs_url_re)) {
    throw Error::HTTPWithStatus(400, "Bad Request");
  }

  static const std::regex up_re{R"(\.\.)"};
  if (std::regex_search(req.endpoint, up_re)) {
    throw Error::HTTPWithStatus(403, "Forbidden");
  }
}

} // namespace http
