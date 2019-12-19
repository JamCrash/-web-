
#include "HttpServer.h"
#include "HttpConnection.h"
#include "Sockets.h"

#include "../net/EventLoop.h"
#include "../net/Channel.h"
#include "../base/Logging.h"

#include <unistd.h>
#include <memory>
#include <cassert>

namespace Http
{

  HttpServer::HttpServer(net::EventLoop* loop, uint16_t port)
  : loop_(loop),
    port_(port),
    acceptFd_(Sockets::create_and_bind(port, true)),
    acceptChannel_(new net::Channel(loop, acceptFd_)),
    start_(false)
  {
    if(acceptFd_ < 0)
    {
      LOG << "create listen socket failed";
      abort();
    }
    
    acceptChannel_->setReadCallBack(
      std::bind(&HttpServer::handleNewConnection, this));
  }

  HttpServer::~HttpServer()
  {
      // TODO
      // close the listen socket file descriptor
  }

  void HttpServer::start()
  {
    assert(!start_);

    if(Sockets::listen_socket(acceptFd_) < 0)
      abort();
    acceptChannel_->enableReading();

    start_ = true;
  }

  void HttpServer::handleNewConnection()
  {
    int infd;
    while((infd = Sockets::accept_sockets(acceptFd_)) > 0)
    {
      //LOG << "new connection referenced to " << infd;
    }
  }

  void HttpServer::handleDisconnecting(int socketFd)
  {
    
  }

}
