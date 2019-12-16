
#include "Thread.h"

#include <unistd.h>
#include <sys/syscall.h>

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
} // Base