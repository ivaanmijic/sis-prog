#include "../include/Error.h"
#include "../include/RelayServer.h"
#include "../include/Utils.h"
#include <exception>
#include <string>

int main() {
  try {
    RelayServer server{9000, "receiver-fifo"};
    server.init();
    server.run();
  } catch (const RelayError &e) {
    fatal("relay server error: ", e.what());
  } catch (const std::exception &e) {
    fatal("unexpected error", e.what());
  }
}
