
#ifndef _THREAD_H
#define _THREAD_H

#include "noncopyable.h"

#include <pthread.h>
#include <functional>

namespace Base 
{

namespace CurrentThread
{
  extern __thread pid_t cachedTid;
  void cacheTid();

  bool isMainThread();

  inline pid_t CurrentThreadId()
  {
    if(cachedTid == 0)
    {
      cacheTid();
    }
    return cachedTid;
  }
} // CurrentThread

namespace ThreadDef
{

  class Thread: public noncopyable 
  {
    using Func = std::function<void()>;

  public:
    Thread(const Func func);
    ~Thread();

    void start();

    void join();

  private:
    Func func_;
    bool joined_;
    bool started_;
    pid_t* threadId_;
    pthread_t tid_;
  };

} // Thread

} // Base

#endif  // _THREAD_H
