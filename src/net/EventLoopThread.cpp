
#include "EventLoopThread.h"
#include "EventLoop.h"

#include "../base/Thread.h"

#include <memory>

namespace net
{

  EventLoopThread::EventLoopThread()
  : loop_(NULL),
    thread_(new Base::ThreadDef::Thread(std::bind(&EventLoopThread::start, this))),
    mutex_(),
    cond_(mutex_)
  {

  }

  EventLoopThread::~EventLoopThread()
  {
    loop_->quit();
    thread_->join();
  }

  EventLoop* EventLoopThread::startLoop()
  {
    thread_->start();

    {
      Base::MutexLockGuard guard(mutex_);
      while(loop_ == NULL)
        cond_.wait();
    }

    return loop_;
  }

  void EventLoopThread::start()
  {
    EventLoop loop;

    {
      Base::MutexLockGuard guard(mutex_);
      loop_ = &loop;
      cond_.signal();
    }

    loop.loop();
  }

}
