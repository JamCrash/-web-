
#ifndef _EVENTLOOP_THREAD_H
#define _EVENTLOOP_THREAD_H

#include "../base/noncopyable.h"

#include <memory>

namespace Base
{
namespace ThreadDef
{
  class Thread;
}
}

namespace net
{

  class EventLoop;
  
  class EventLoopThread: public Base::noncopyable
  {
  public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* getLoop() const { return &loop_; }
  private:
    EventLoop loop_;
  };

}

#endif  // _EVENTLOOP_THREAD_H
