
#include "../HttpServer.h"
#include "../../net/EventLoop.h"

using namespace net;
using namespace Http;

int main()
{
  EventLoop loop;
  HttpServer server(&loop, 3000, 10);
  server.start();
  loop.loop();
}
