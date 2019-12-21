
#include "HttpServer.h"
#include "HttpConnection.h"
#include "Sockets.h"

#include "../net/EventLoop.h"
#include "../net/Channel.h"
#include "../net/EventLoopThreadPool.h"
#include "../base/Logging.h"

#include <unistd.h>
#include <memory>
#include <cassert>
#include <functional>

namespace Http
{

  using std::placeholders::_1;

  HttpServer::HttpServer(net::EventLoop* loop, uint16_t port, int threadNum)
  : baseLoop_(loop),
    port_(port),
    acceptFd_(Sockets::create_and_bind(port, true)),
    acceptChannel_(new net::Channel(loop, acceptFd_)),
    start_(false),
    pool_(new net::EventLoopThreadPool(loop, threadNum))
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
      //TODO
  }

  void HttpServer::start()
  {
    assert(!start_);

    if(Sockets::listen_socket(acceptFd_) < 0)
      abort();
    acceptChannel_->enableReading();
    pool_->start();

    start_ = true;
  }

  void HttpServer::handleNewConnection()
  {
    int infd;
    while((infd = Sockets::accept_sockets(acceptFd_)) > 0)
    {
      LOG << "new connection referenced to " << infd;

      Sockets::set_nonblock(infd);

      net::EventLoop* ioLoop = pool_->getNext();
      HttpConnectionPtr conn(new HttpConnection(ioLoop, infd));
      connections_[infd] = conn;
      conn->setCloseCallBack(
        std::bind(&HttpServer::handleDisconnecting, this, _1));
      ioLoop->queueInLoop(
        std::bind(&HttpConnection::connectionEstablish, conn));
    }
  }

  void HttpServer::handleDisconnecting(int sockFd)
  {
    baseLoop_->runInLoop(
      std::bind(&HttpServer::removeConnection, this, sockFd));
  }

  void HttpServer::removeConnection(int sockFd)
  {
    auto it = connections_.find(sockFd);
    assert(it != connections_.end());

    HttpConnectionPtr conn = it->second;
    LOG << "ready to remove connection with socket " << sockFd;
    connections_.erase(it);
    conn->getLoop()->queueInLoop(
      std::bind(&HttpConnection::connectionDestroy, conn));
  }

}
