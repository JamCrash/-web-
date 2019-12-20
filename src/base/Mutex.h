
#ifndef _MUTEX_H
#define _MUTEX_H

#include "noncopyable.h"
#include "Thread.h"

#include <pthread.h>
#include <cassert>

namespace Base
{

  class MutexLock: public noncopyable 
  {
    friend class Condition;
  public:
    MutexLock(): holder_(0)
    {
      pthread_mutex_init(&lock_, NULL);
    } 

    ~MutexLock()
    {
      assert(holder_ == 0);
      pthread_mutex_destroy(&lock_);
    }

    void lock()
    {
      pthread_mutex_lock(&lock_);
      assert(holder_ == 0);
      assignHolder();
    }

    void unlock()
    {
      unAssignHolder();
      pthread_mutex_unlock(&lock_);
    }

  private:
    pthread_mutex_t lock_;
    pid_t holder_;

    // for condition variable
    pthread_mutex_t* getlock() { return &lock_; }
    void assignHolder() { holder_ = CurrentThread::CurrentThreadId(); }
    void unAssignHolder() { holder_ = 0; }

    class holderAssignGuard
    {
    public:
      holderAssignGuard(MutexLock& mutex): mutex_(mutex)
      {
        mutex_.unAssignHolder();
      }
      ~holderAssignGuard()
      {
        mutex_.assignHolder();
      }
    private: 
      MutexLock& mutex_;
    };
  };

  class MutexLockGuard: public noncopyable 
  {
  public:
    MutexLockGuard(MutexLock& mutex): mutex_(mutex)
    {
      mutex_.lock();
    }

    ~MutexLockGuard()
    {
      mutex_.unlock();
    }
  
  private:
    MutexLock& mutex_;
  };

#define MutexLockGuard(x) error "MutexLockGuard can't be temporary variable"
}

#endif  // _MUTEX_H
