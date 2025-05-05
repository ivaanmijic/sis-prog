#include "../include/logger.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define BUFSIZE 1024

static int fd = -1;

static const char *level_to_string(LogLevel level) {
  switch (level) {
  case DEBUG:
    return "DEBUG";
  case INFO:
    return "INFO";
  case WARNING:
    return "WARNING";
  case ERROR:
    return "ERROR";
  }
}

void Logger_init() {
  fd = open("app.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
  if (fd < 0) {
    fd = STDERR_FILENO;
  }
}

void Logger_close() {
  if (fd != STDERR_FILENO)
    close(fd);
}

void Logger_log(LogLevel level, const char *fmt, ...) {
  char msgBuffer[BUFSIZE];
  time_t t = time(NULL);
  struct tm tm_info;

  localtime_r(&t, &tm_info);

  char timebuf[20];
  if (strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm_info) == 0) {
    snprintf(timebuf, sizeof(timebuf), "0000-00-00 00:00:00");
  }

  int offset = snprintf(msgBuffer, BUFSIZE, "[%s] ", timebuf);
  if (offset < 0)
    offset = 0;

  va_list args;
  va_start(args, fmt);
  int n = vsnprintf(msgBuffer + offset, BUFSIZE - offset, fmt, args);
  va_end(args);

  size_t total_len = offset + (n >= 0 ? (size_t)n : 0);
  if (total_len >= BUFSIZE) {
    total_len = BUFSIZE - 1;
  }

  if (total_len < BUFSIZE - 1) {
    msgBuffer[total_len++] = '\n';
  }

  write(fd, msgBuffer, total_len);
}
