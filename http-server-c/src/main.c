#include "../include/logger.h"

int main(int argc, char *argv[]) {
  Logger_init();
  LOG_INFO("Application started");
  LOG_DEBUG("Some debug detail");
  LOG_ERROR("Something went wrong");
  Logger_close();
  return 0;
}
