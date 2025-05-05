#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

typedef enum { DEBUG, INFO, WARNING, ERROR } LogLevel;

void Logger_init(void);
void Logger_close(void);

void Logger_log(LogLevel level, const char *fmt, ...);

#define LOG_DEBUG(fmt, ...)                                                    \
  Logger_log(DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
  Logger_log(INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)                                                  \
  Logger_log(WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
  Logger_log(ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif // !LOGGER_H
