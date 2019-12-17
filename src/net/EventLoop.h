
#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include "../base/noncopyable.h"
#include "../base/Thread.h"
#include "../base/Mutex.h"

#include <memory>
#include <vector>
#include <functional>
#include <cassert>

namespace net
{

  class EPoller;
  class Channel;

  class EventLoop: public Base::noncopyable
  {
    using Functor = std::function<void()>;

  public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    bool isInLoopThread() { return threadId_ == Base::CurrentThread::CurrentThreadId(); }
    void assertInLoopThread() { assert(isInLoopThread()); }

    pid_t getThreadId() const { return threadId_; }

    void updateChannel(Channel* channel);

    void runInLoop(const Functor& func);
    void queueInLoop(const Functor& func);

  private:
    void wakeup();

    // process the wake up signal
    void handleRead();

    void doPendingFunctors();

  private:
    bool looping_;
    bool quit_;
    pid_t threadId_;
    int eventFd_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::unique_ptr<EPoller> epoller_;
    std::vector<Channel*> activeChannels_;

    bool callingPendingFunctors_;
    std::vector<Functor> pendingFunctors_;
    // Mutex for pengingFunctors_
    Base::MutexLock mutex_;
  };

}

#endif  // _EVENTLOOP_H
