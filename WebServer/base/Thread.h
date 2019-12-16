
#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>

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
} // Base

#endif  // _THREAD_H
