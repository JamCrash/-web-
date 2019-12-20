
#include "Logging.h"
#include "Thread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <cassert>
#include <functional>

namespace Base
{
namespace CurrentThread
{

  __thread pid_t cachedTid = 0;

namespace detail
{

  pid_t gettid()
  {
    return static_cast<pid_t>(::syscall(SYS_gettid));
  }
  
} // detail

  void cacheTid()
  {
    if(cachedTid == 0)
    {
      cachedTid = detail::gettid();
    }
  }

  bool isMainThread()
  {
    return CurrentThreadId() == getpid();
  }

} // CurrentThread

namespace ThreadDef
{

namespace 
{
  class ThreadData: public Base::noncopyable
  {
    using Func = void(*)(void);

  public:
    ThreadData(pid_t* threadId, const Func& func)
    : threadId_(threadId),
      func_(func)
    {
      
    }

    ~ThreadData() = default;

    void startThread()
    {
      assert(threadId_);
      *threadId_ = CurrentThread::CurrentThreadId();
      func_();
    }

  private:
    pid_t* threadId_;
    const Func& func_;
  };

  void* entrance(void* arg)
  {
    ThreadData* t = static_cast<ThreadData*>(arg);
    t->startThread();
    delete t;
    
    return NULL;
  }
} // static namespace 

  Thread::Thread(const Func& func)
  : func_(func),
    joined_(false),
    started_(false),
    threadId_(new pid_t(0))
  {

  }

  Thread::~Thread()
  {
    if(started_ && !joined_)
    {
      pthread_detach(tid_);
      //joined_ = true;
    }
  }

  void Thread::start()
  {
    assert(!started_);

    ThreadData* data_ = new ThreadData(threadId_, func_);
    if(pthread_create(&tid_, NULL, entrance, static_cast<void*>(data_)) < 0)
    {
      LOG << "create new thread failed";
      abort();
    }

    started_ = true;
  }

  void Thread::join()
  {
    if(started_ && !joined_)
    {
      pthread_join(tid_, NULL);
      joined_ = true;
    }
  }

} // Thread
} // Base
