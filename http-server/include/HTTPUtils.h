#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

namespace http {

using HeaderMap = std::unordered_map<std::string, std::string>;

// static void trim(std::string &s);

} // namespace http
