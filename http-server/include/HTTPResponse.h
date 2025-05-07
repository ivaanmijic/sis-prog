#pragma once

#include "HTTPUtils.h"
#include <string>
namespace http {

class HTTPResponse {

public:
  HTTPResponse(int code = 200, const std::string &text = "OK");
  void setStatus(int code, const std::string &text);
  void setBody(const std::string &data);
  void addHeader(const std::string &name, const std::string &value);
  std::string toString() const;

private:
  int statusCode;
  std::string statusText;
  HeaderMap headers;
  std::string body;
};

}; // namespace http
