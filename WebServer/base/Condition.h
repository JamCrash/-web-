
#ifndef _CONDITION_H
#define _CONDITION_H

#include "Mutex.h"

#include <pthread.h>
#include <cassert>

namespace Base
{

  class Condition: public noncopyable 
  {
  public:
    Condition(MutexLock& mutex): mutex_(mutex)
    {
      pthread_cond_init(&cond_, NULL);
    }

    ~Condition()
    {
      pthread_cond_destroy(&cond_);
    }

    void wait()
    {
      assert(mutex_.holder_ == CurrentThread::CurrentThreadId());
      MutexLock::holderAssignGuard guard(mutex_);
      pthread_cond_wait(&cond_, mutex_.getlock());
    }

    void signal()
    {
      pthread_cond_signal(&cond_);
    }

    void notifyAll()
    {
      pthread_cond_broadcast(&cond_);
    }

  private:
    pthread_cond_t cond_;
    MutexLock& mutex_;
  };

}

#endif  // _CONDITION_H
