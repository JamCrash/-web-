
#ifndef _EVENTLOOP_THREAD_H
#define _EVENTLOOP_THREAD_H

#include "../base/noncopyable.h"
#include "../base/Mutex.h"
#include "../base/Condition.h"

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

    EventLoop* startLoop();

  private:
    void start();

  private:
    EventLoop* loop_;
    std::unique_ptr<Base::ThreadDef::Thread> thread_;
    Base::MutexLock mutex_;
    Base::Condition cond_;
  };

}

#endif  // _EVENTLOOP_THREAD_H
