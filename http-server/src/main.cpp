#include "../include/logger.h"
#include "../include/utils.h"
#include <iostream>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (!isValidPort(argv[1]))
    fatal("Invalid port number");
}
