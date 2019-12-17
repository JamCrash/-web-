
#ifndef _EPOLLER_H
#define _EPOLLER_H

#include "../base/noncopyable.h"

#include <sys/epoll.h>
#include <vector>
#include <map>

struct epoll_event;

namespace net
{
  
  class EventLoop;
  class Channel;

  class EPoller: public Base::noncopyable 
  {
    using ChannelList = std::vector<Channel*>;

  public:
    EPoller(EventLoop* loop);
    ~EPoller();

    void assertInLoopThread();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void wait(int timeoutMs, ChannelList* activeChannels);

  private:
    int epollUpdate(int operation, Channel* channel);

    void fillActiveChannelList(int activeNums, ChannelList* activeChannels);

  private:
    int epFd_;
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    static const int eventsSizeInitializer_;
    std::map<int, Channel*> channels_;
  };

}

#endif  // _EPOLLER_H
