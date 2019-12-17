
#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>
#include <cassert>

namespace net
{

  const int Channel::readEvents_ = EPOLLIN | EPOLLPRI;
  const int Channel::writeEvents_ = EPOLLOUT;
  const int Channel::nonEvent_ = 0;

  Channel::Channel(EventLoop* loop, int infd)
  : fd_(infd),
    events_(0),
    revents_(0),
    status_(Channel::cDisconnected_),
    loop_(loop)
  {

  }

  Channel::~Channel()
  {

  }

  void Channel::update()
  {
    loop_->updateChannel(this);
  }

  void Channel::handleEvent()
  {
    if(revents_ & (EPOLLIN | EPOLLPRI))
      if(readCallBack_) readCallBack_();
    
    if(revents_ & EPOLLOUT)
      if(writeCallBack_) writeCallBack_();
    
    if(revents_ & EPOLLERR)
      if(errorCallBack_) errorCallBack_();

    if(revents_ & (EPOLLHUP | EPOLLRDHUP))
      if(closeCallBack_) closeCallBack_();
  }

  void Channel::setStatus(int status)
  {
    assert( (status == cDisconnected_) || (status == cConnected_) );
    status_ = status;
  }
}