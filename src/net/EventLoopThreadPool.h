
#ifndef _EVENTLOOP_THREADPOOL_H
#define _EVENTLOOP_THREADPOOL_H

#include "../base/noncopyable.h"

#include <vector>
#include <memory>

namespace net
{

  class EventLoop;
  class EventLoopThread;

  class EventLoopThreadPool: public Base::noncopyable
  {
  public:
    EventLoopThreadPool(EventLoop* loop, int threadNum = 0);
    ~EventLoopThreadPool();

    void start();

    EventLoop* getNext();

  private:
    bool start_;
    EventLoop* baseLoop_;
    int threadNum_;
    int idx_;

    std::vector<EventLoop*> loops_;
    std::vector<EventLoopThread*> threads_;
  };

}

#endif  // _EVENTLOOP_THREADPOOL_H
