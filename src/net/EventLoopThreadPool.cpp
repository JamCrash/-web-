
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

#include <cassert>

namespace net
{

  EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, int threadNum)
  : start_(false),
    baseLoop_(loop),
    threadNum_(threadNum),
    idx_(0)
  {

  }

  EventLoopThreadPool::~EventLoopThreadPool()
  {
    for(int i = 0; i < threadNum_; ++i)
    {
      delete threads_[i];
      threads_[i] = NULL;
    }
  }

  void EventLoopThreadPool::start()
  {
    assert(!start_);
    baseLoop_->assertInLoopThread();
    
    for(int i = 0; i < threadNum_; ++i)
    {
      EventLoopThread* loopThread = new EventLoopThread();
      threads_.push_back(loopThread);
      loops_.push_back(loopThread->startLoop());
    }

    start_ = true;
  }

  EventLoop* EventLoopThreadPool::getNext()
  {
    assert(start_);
    baseLoop_->assertInLoopThread();

    EventLoop* retLoop = baseLoop_;
    if(!loops_.empty())
    {
      retLoop = loops_[idx_++];
      if(idx_ == threadNum_)
        idx_ = 0;
    }

    return retLoop;
  }

}
