
#include "../base/Logging.h"

#include "EPoller.h"
#include "EventLoop.h"
#include "Channel.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <strings.h>

namespace net
{

namespace 
{
  inline int createEpollFd()
  {
    int epFd = epoll_create1(EPOLL_CLOEXEC);
    if(epFd <= 0)
    {
      LOG << "create epoll file descriptor failed";
      abort();
    }
    return epFd;
  }
} // static namespace

  const int EPoller::eventsSizeInitializer_ = 16;

  EPoller::EPoller(EventLoop* loop)
  : epFd_(createEpollFd()),
    loop_(loop),
    events_(eventsSizeInitializer_)
  {
    
  }

  EPoller::~EPoller()
  {
    ::close(epFd_);
  }

  void EPoller::assertInLoopThread()
  {
    loop_->assertInLoopThread();
  }

  void EPoller::updateChannel(Channel* channel)
  {
    assertInLoopThread();
    int status = channel->getStatus();
    if(status == Channel::cDisconnected_)
    {
      if( !(channel->isNonEvent()) )
      {
        // add new channel
        int channelFd = channel->getFd();
        assert(channels_.find(channelFd) == channels_.end());
        if(epollUpdate(EPOLL_CTL_ADD, channel) == 0)
        {
          channel->setStatus(Channel::cConnected_);
          channels_[channelFd] = channel;
          LOG << "new channel added";
        }
        else
          LOG << "add new channel failed";
      }
    }
    else
    {
      // update or delete channel
      assert(status == Channel::cConnected_);
      int channelFd = channel->getFd();
      if(channel->isNonEvent())
      {
        removeChannel(channel);
      }
      else 
      {
        assert(channels_.find(channelFd) != channels_.end());
        assert(channels_[channelFd] == channel);
        if(epollUpdate(EPOLL_CTL_MOD, channel) == 0)
          LOG << "channel update successfully";
        else 
          LOG << "channel update failed";
      }
    }
  }

  void EPoller::removeChannel(Channel* channel)
  {
    assertInLoopThread();
    int channelFd = channel->getFd();
    if(channels_.find(channelFd) != channels_.end())
    {
      assert(channels_[channelFd] == channel);
      if(epollUpdate(EPOLL_CTL_DEL, channel) == 0)
      {
        channel->setStatus(Channel::cDisconnected_);
        channels_.erase(channelFd);
        LOG << "remove channel successfully";
      }
      else 
        LOG << "remove channel failed";
    }
  }

  int EPoller::epollUpdate(int operation, Channel* channel)
  {
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.data.ptr = static_cast<void*>(channel);
    event.events = static_cast<uint32_t>(channel->getEvents());
    LOG << "ready to modify fd = " << channel->getFd();
    return epoll_ctl(epFd_, operation, channel->getFd(), &event);
  }

  void EPoller::wait(int timeoutMs, ChannelList* activeChannels)
  {
    int activeNums = epoll_wait(epFd_, events_.data(), events_.size(), timeoutMs);
    if(activeNums > 0)
    {
      LOG << activeNums << ((activeNums == 1) ? " event" : " events") << " happenned";
      fillActiveChannelList(activeNums, activeChannels);
      if(activeNums == events_.size())
      {
        events_.resize(events_.size() + events_.size() / 4);
      }
    }
    else if(activeNums == 0)
    {
      LOG << "nothing happen";
    }
    else 
    {
      LOG << "some error occurred when calling epoll_wait";
    }
  }

  void EPoller::fillActiveChannelList(int activeNums, ChannelList* activeChannels)
  {
    for(int i = 0; i < activeNums; ++i)
    {
      struct epoll_event& event = events_[i];
      Channel* channel = static_cast<Channel*>(event.data.ptr);
      channel->setRevents(event.events);
      activeChannels->push_back(channel);
    }
  }
}
