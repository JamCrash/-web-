
#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

namespace Base
{

  class noncopyable 
  {
  public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
  protected:
    noncopyable() = default;
    ~noncopyable() = default;
  };

}

#endif  // _NONCOPYABLE_H
