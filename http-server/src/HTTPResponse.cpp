#include "../include/HTTPResponse.h"
#include <sstream>
#include <string>

namespace http {
HTTPResponse::HTTPResponse(int code, const std::string &text)
    : statusCode(code), statusText(text) {}

void HTTPResponse::setStatus(int code, const std::string &text) {
  statusCode = code;
  statusText = text;
}

void HTTPResponse::setBody(const std::string &data) {
  body = data;
  headers["Content-Length"] = std::to_string(data.size());
}

void HTTPResponse::addHeader(const std::string &name,
                             const std::string &value) {
  headers[name] = value;
}

std::string HTTPResponse::toString() const {
  std::ostringstream out;
  out << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
  for (const auto &header : headers) {
    out << header.first << ": " << header.second << "\r\n";
  }
  out << "\r\n" << body;
  return out.str();
}

}; // namespace http
