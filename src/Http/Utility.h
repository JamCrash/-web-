
#ifndef _UTILITY_H
#define _UTILITY_H

#include <unistd.h>
#include <errno.h>
#include <string>

namespace Utility 
{
  
  #define MAXBUF 1024

  ssize_t readn(int fd, std::string& buffer, bool& readFinish)
  {
    ssize_t readSum = 0;
    ssize_t n;

    while(true)
    {
      char buf[MAXBUF];

      if((n = ::read(fd, buf, MAXBUF)) < 0)
      {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
          // socket为非阻塞时,将读错误为EAGAIN或EWOULDBLOCK作为退出条件之一
          // 数据是否读全由数据解析函数handleRead判断
          readFinish = true;
          break;
        }
        else if(errno == EINTR)
          continue;
        else 
          return -1;
      }
      else if(n == 0)
      {
        readFinish = true;
        break;
      }
      else 
      {
        readSum += n;
        buffer += std::string(buf, buf+n);
      }
    }

    return readSum;
  }

  ssize_t writen(int fd, std::string& buffer)
  {
    ssize_t writeSum = 0;
    ssize_t n;
    ssize_t nLeft = buffer.size();
    const char* ptr = buffer.data();

    while(nLeft > 0)
    {
      n = ::write(fd, ptr, nLeft);
      if(n < 0)
      {
        if(errno == EAGAIN || errno == EWOULDBLOCK)
          break;
        else if(errno == EINTR)
          continue;
        else 
          return -1;
      }
      nLeft -= n;
      ptr += n;
      writeSum += n;
    }

    if(nLeft == 0)
      buffer.clear();
    else 
      buffer = buffer.substr(writeSum);

    return writeSum;
  }

}

#endif  // _UTILITY_H
