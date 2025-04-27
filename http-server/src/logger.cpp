#include "../include/logger.h"
#include <chrono>
#include <ctime>
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>

namespace log {

Logger::Logger() {
  fd = open("app.log", O_WRONLY | O_CREAT | O_APPEND, 666);
  if (fd < 0) {
    fd = STDERR_FILENO;
  }
}

Logger::~Logger() {
  if (fd != STDERR_FILENO)
    close(fd);
}

void Logger::log(const std::string &msg, LogLevel level, const char *file,
                 int line) {
  auto now = std::chrono::system_clock::now();
  std::time_t ctime = std::chrono::system_clock::to_time_t(now);
  std::tm tm;
  localtime_r(&ctime, &tm);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " ["
      << logLevelToString(level) << "] " << file << ":" << line << " - " << msg
      << "\n";
  auto out = oss.str();

  write(fd, out.c_str(), out.size());
}

std::string Logger::logLevelToString(LogLevel level) {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}
} // namespace log
