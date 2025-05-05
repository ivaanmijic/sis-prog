#include "../include/error.h"
#include "../include/port.h"
#include "../include/utils.h"
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  try {
    std::string portArg(validateArgs(argc, argv));
    Port port(portArg);
  } catch (const Error::Port &e) {
    fatal(e.what());
  } catch (const Error::Arg &e) {
    fatal(e.what());
  }
}
