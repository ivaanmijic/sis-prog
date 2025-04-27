#include "../include/logger.h"

int main(int argc, char *argv[]) {
  LOG_INFO("Application started");
  LOG_DEBUG("Some debug detail");
  LOG_ERROR("Something went wrong");
  return 0;
}
