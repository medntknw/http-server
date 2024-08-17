#include "http_server.h"

int main(int argc, char **argv) {
  HttpServer server = HttpServer("4221");
  server.run();
  return 0;
}
