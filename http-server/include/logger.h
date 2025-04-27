#pragma once

#include <mutex>
#include <string>

namespace log {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logger {
public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  Logger(Logger &&) = delete;
  Logger &operator=(Logger &&) = delete;

  void log(const std::string &messsage, LogLevel level, const char *file,
           int line);

private:
  std::string logLevelToString(LogLevel level);

  Logger();
  ~Logger();

  int fd;
  std::mutex logMutex;
};

#define LOG(level, message)                                                    \
  log::Logger::getInstance().log(message, level, __FILE__, __LINE__)
#define LOG_DEBUG(message) LOG(log::LogLevel::DEBUG, message)
#define LOG_INFO(message) LOG(log::LogLevel::INFO, message)
#define LOG_WARNING(message) LOG(log::LogLevel::WARNING, message)
#define LOG_ERROR(message) LOG(log::LogLevel::ERROR, message)

} // namespace log
