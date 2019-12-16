
#include "HttpConnection.h"

#include "../net/EventLoop.h"
#include "../net/Channel.h"
#include "../base/Logging.h"
#include <stdio.h>

#include <unistd.h>

namespace Http
{

  HttpConnection::HttpConnection(net::EventLoop* loop, int sockFd)
  : loop_(loop),
    sockFd_(sockFd),
    channel_(new net::Channel(loop, sockFd))
  {
    channel_->setReadCallBack(
      std::bind(&HttpConnection::handleRead, this));
    channel_->setWriteCallBack(
      std::bind(&HttpConnection::handleWrite, this));
    channel_->setErrorCallBack(
      std::bind(&HttpConnection::handleError, this));
    channel_->setCloseCallBack(closeCallBack_);
  }

  HttpConnection::~HttpConnection()
  {
    LOG << "ready to close socket = " << sockFd_;
    ::close(sockFd_);
  }
 
  void HttpConnection::handleRead()
  {
    
  }

  void HttpConnection::handleWrite()
  {
    
  }

  void HttpConnection::handleError()
  {
    
  }

  void HttpConnection::connectionEstablish()
  {
    LOG << "New connection established with socket = " << sockFd_;
    channel_->enableReading();
  }

  void HttpConnection::connectionDestroy()
  {
    LOG << "Connection with socket = " << sockFd_ << " disconnected";
    channel_->setNonEvent();
  }
  
}
