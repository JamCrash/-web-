
#ifndef _HTTPDATA_H
#define _HTTPDATA_H

#include "../base/noncopyable.h"

#include <memory>
#include <string>
#include <map>
#include <functional>

namespace Http
{

  enum ProcessState
  {
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_PARSE_FINISH
  };

  enum URIState
  {
    PARSE_URI_SUCCESS = 1,
    //PARSE_URI_AGAIN,
    PARSE_URI_ERROR
  };

  enum HeaderState
  {
    PARSE_HEADER_SUCCESS = 1,
    //PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
  };

/*   enum ConnectionState
  {
    H_CONNECTED, H_DISCONNECTED
  };
 */
  enum AnalysisState
  {
    H_ANALYSIS_SUCCESS, H_ANALYSIS_ERROR
  };

  // 支持的请求方法
  enum Method
  {
    HTTP_GET, HTTP_POST, HTTP_HEAD
  };

  class HttpConnection;

  class HttpData: public Base::noncopyable
  {
    using HttpConnectionPtr = std::shared_ptr<HttpConnection>;
    using CallBack = std::function<void()>;

  public:
    HttpData(HttpConnectionPtr& connection, int sockFd);
    ~HttpData();

    void handleRead();
    void handleWrite();

    /* void setCloseCallBack(const CallBack& cb) { closeCallBack_ = cb; } */

  private:
    URIState parseURI();
    HeaderState parseHeader();

    // analysis向writeBuffer_中写入正确响应后的数据
    AnalysisState analysis();

    void handleError(char* errorCode, char* errorMsg);

    /* // 调用handleClose使服务端清除本次连接信息,并断开本次连接
    void handleClose()
    {
      if(closeCallBack_)
        closeCallBack_();
    } */

  private:
    int sockFd_;
    HttpConnectionPtr connection_;
    bool error_;

    //ConnectionState connectionState_;
    ProcessState processState_;

    std::string readBuffer_;
    std::string writeBuffer_;
    
    // 存储请求头数据
    std::map<std::string, std::string> headers_;
    // HTTP版本
    std::string version_;
    // 请求的文件
    std::string filename_;
    // 请求方法
    Method method_;

    /* // closeCallBack;
    // closeCallBack使Server清除本次连接信息
    CallBack closeCallBack_; */
  };

}

#endif  // _HTTPDATA_H
