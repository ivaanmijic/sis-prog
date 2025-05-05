#pragma once

#include <cstdarg>
#include <string>

#define LOG(level, fmt, ...)                                                   \
  Logger::getInstance().log(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG(LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG(LogLevel::INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) LOG(LogLevel::WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG(LogLevel::ERROR, fmt, ##__VA_ARGS__)

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
public:
  static Logger &getInstance();

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  void log(LogLevel level, const char *file, int line, const char *fmt, ...);

private:
  std::string logLevelToString(LogLevel level);

  Logger();
  ~Logger();

  int fd;
};
