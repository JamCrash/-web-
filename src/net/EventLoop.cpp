
#include "../base/Logging.h"

#include "EventLoop.h"
#include "Channel.h"
#include "EPoller.h"

#include <unistd.h>
#include <sys/eventfd.h>
#include <cassert>

namespace net
{

namespace 
{
  inline int createEventFd()
  {
    int evFd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(evFd <= 0)
    {
      LOG << "create event file descriptor failed";
      abort();
    }
    return evFd;
  }
}

  __thread EventLoop* currentEventLoop = 0;

  EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    threadId_(Base::CurrentThread::CurrentThreadId()),
    eventFd_(createEventFd()),
    wakeupChannel_(new Channel(this, eventFd_)),
    epoller_(new EPoller(this)),
    callingPendingFunctors_(false)
  {
    if(currentEventLoop != 0)
    {
      LOG << "already exit EventLoop in this thread";
      abort();
    }

    currentEventLoop = this;
    wakeupChannel_->setReadCallBack(
      std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
    LOG << "new EventLoop create in thread: " << threadId_;
  }

  EventLoop::~EventLoop()
  {
    assert(!looping_);
    ::close(eventFd_);
    currentEventLoop = 0;
  }

  void EventLoop::loop()
  {
    assertInLoopThread();
    looping_ = true;

    while(!quit_)
    {
      activeChannels_.clear();
      epoller_->wait(1000, &activeChannels_);
      for(auto it = activeChannels_.cbegin(); it != activeChannels_.cend(); ++it)
      {
        (*it)->handleEvent();
      }
      doPendingFunctors();
    }

    looping_ = false;
  }

  void EventLoop::quit()
  {
    quit_ = true;

    if(!isInLoopThread())
    {
      wakeup();
    }
  }

  void EventLoop::updateChannel(Channel* channel)
  {
    assert(channel->getLoop() == this);
    assertInLoopThread();
    epoller_->updateChannel(channel);
  }

  void EventLoop::runInLoop(const Functor& func)
  {
    if(isInLoopThread())
      func();
    else 
      queueInLoop(func);
  }

  void EventLoop::queueInLoop(const Functor& func)
  {
    {
      Base::MutexLockGuard guard(mutex_);
      pendingFunctors_.push_back(func);
    }

    if(!isInLoopThread() || callingPendingFunctors_)
      wakeup();
  }

  void EventLoop::wakeup()
  {
    uint64_t val = 1;
    ssize_t n = ::write(eventFd_, &val, sizeof val);
    if(n != sizeof val)
    {
      LOG << "wake up EventLoop failed";
    }
  }

  void EventLoop::handleRead()
  {
    uint64_t val;
    ssize_t n = ::read(eventFd_, &val, sizeof val);
    if(n != sizeof val)
    {
      LOG << "process wake up signal failed";
    }
  }
  
  void EventLoop::doPendingFunctors()
  {
    callingPendingFunctors_ = true;
    std::vector<Functor> Functors;

    {
      Base::MutexLockGuard guard(mutex_);
      Functors.swap(pendingFunctors_);
    }

    for(const Functor& func: Functors)
      func();
    
    callingPendingFunctors_ = false;
  }
}
