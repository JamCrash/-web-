
#include "HttpData.h"
#include "HttpConnection.h"
#include "Utility.h"

#include "../base/Logging.h"

#include <errno.h>
#include <cassert>

namespace Http
{
  HttpData::HttpData(HttpConnectionPtr& connection, int sockFd)
  : connection_(connection), 
    sockFd_(sockFd),
    processState_(ProcessState::STATE_PARSE_URI),
    //connectionState_(ConnectionState::H_CONNECTED),
    error_(false)
  {

  }

  HttpData::~HttpData()
  {
    // TODO
  }

  void HttpData::handleRead()
  {
    ssize_t readSum = 0;
    
    do 
    {
      bool readFinish_ = false;

      readSum = Utility::readn(sockFd_, readBuffer_, readFinish_);
      if(readSum < 0)
      {
        perror("read socket error");
        error_ = true;
        handleError("400", "Bad Request");
        break;
      }
      if(readSum == 0 && readFinish_)
      {
        // 连接成功建立但读不到数据,可能是网络拥塞或对端关闭了连接
        // 统一按照关闭连接处理
        connection_->handleClose();
        break;
      }
      assert(readFinish_);

      if(processState_ == ProcessState::STATE_PARSE_URI)
      {
        URIState flag = parseURI();
        if(flag == URIState::PARSE_URI_ERROR)
        {
          perror("parse URI error");
          error_ = true;
          handleError("400", "Bad Request");
          break;
        }
        processState_ = ProcessState::STATE_PARSE_HEADERS;
      }
      if(processState_ == ProcessState::STATE_PARSE_HEADERS)
      {
        HeaderState flag = parseHeader();
        if(flag == HeaderState::PARSE_HEADER_ERROR)
        {
          perror("parse Header error");
          error_ = true;
          handleError("400", "Bad Request");
          break;
        }
        if(method_ == Method::HTTP_POST)
          processState_ = ProcessState::STATE_RECV_BODY;
        else 
          processState_ = ProcessState::STATE_ANALYSIS;
      }
      if(processState_ == ProcessState::STATE_RECV_BODY)
      {
        
      }
      if(processState_ == ProcessState::STATE_ANALYSIS)
      {
        AnalysisState flag = analysis();
        if(flag )
      }
      
    } while(false);
  }

  void HttpData::handleWrite()
  {

  }

  void HttpData::handleError(char* errorCode, char* errorMsg)
  {

  }

  URIState HttpData::parseURI()
  {

  }

  HeaderState HttpData::parseHeader()
  {

  }

  AnalysisState HttpData::analysis()
  {

  }

}