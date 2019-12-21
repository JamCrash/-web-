
#include "HttpData.h"
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
    httpData_(new HttpData(this, sockFd)),
    channel_(new net::Channel(loop, sockFd))
  {
    channel_->setReadCallBack(
      std::bind(&HttpData::handleRead, httpData_));
    channel_->setWriteCallBack(
      std::bind(&HttpData::handleWrite, httpData_));
  }

  HttpConnection::~HttpConnection()
  {
    LOG << "ready to close socket = " << sockFd_;
    ::close(sockFd_);
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
  
  void HttpConnection::updateRead(bool flag)
  {
    if(flag) 
    {
      if(!(channel_->isEnableReading()))
        channel_->enableReading();
    }
    else 
    {
      if(channel_->isEnableReading())
        channel_->unableReading();
    }
  }

  void HttpConnection::updateWrite(bool flag)
  {
    if(flag)
    {
      if(!(channel_->isEnableWriting()))
        channel_->enableWriting();
    }
    else 
    {
      if(channel_->isEnableWriting())
        channel_->unableWriting();
    }
  }
}
