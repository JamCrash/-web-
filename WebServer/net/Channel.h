
#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../base/noncopyable.h"

#include <functional>

namespace net
{

  class EventLoop;

  class Channel: public Base::noncopyable
  {
    using EventCallBack = std::function<void()>;

  public:
    Channel(EventLoop* loop, int infd);
    ~Channel();

    void setReadCallBack(const EventCallBack& cb) { readCallBack_ = cb; }
    void setWriteCallBack(const EventCallBack& cb) { writeCallBack_ = cb; }
    void setErrorCallBack(const EventCallBack& cb) { errorCallBack_ = cb; }
    void setCloseCallBack(const EventCallBack& cb) { closeCallBack_ = cb; }

    void enableReading() { events_ |= readEvents_; update(); }
    void enableWriting() { events_ |= writeEvents_; update(); }
    void setNonEvent() { events_ = nonEvent_; update(); }
    bool isNonEvent() const { return events_ == nonEvent_; }

    void handleEvent();

    const EventLoop* getLoop() const { return loop_; }

    // for EPoller
    static const int cConnected_ = 1;
    static const int cDisconnected_ = -1;

    void setStatus(int status);
    int getStatus() const { return status_; }
    int getFd() const { return fd_; }
    int getEvents() const { return events_; }
    void setRevents(int revents) { revents_ = revents; }

  private:
    void update();

  private:
    int fd_;
    int events_;
    int revents_;
    // for EPoller
    int status_;
    EventLoop* loop_;

    EventCallBack readCallBack_;
    EventCallBack writeCallBack_;
    EventCallBack errorCallBack_;
    EventCallBack closeCallBack_;

    static const int readEvents_;
    static const int writeEvents_;
    static const int nonEvent_;
  };

}

#endif  // _CHANNEL_H
