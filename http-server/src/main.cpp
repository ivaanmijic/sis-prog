#include "../include/HttpServer.h"
#include "../include/Port.h"
#include "../include/error.h"
#include "../include/utils.h"
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  try {

    std::string portArg(validateArgs(argc, argv));
    Port port(portArg);

    HTTPServer server{port};
    server.init();
    server.listenAndServe();

  } catch (const Error::Port &e) {
    fatal("bad port: ", e.what());
  } catch (const Error::Arg &e) {
    fatal("usage: ", e.what());
  } catch (const Error::HTTP &e) {
    fatal("server: ", e.what());
  } catch (const std::exception &e) {
    fatal("unknown: ", e.what());
  }
}
