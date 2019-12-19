
#include "HttpData.h"
#include "HttpConnection.h"
#include "Utility.h"

#include "../base/Logging.h"

#include <errno.h>
#include <cassert>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace Http
{

  std::map<std::string, std::string> HttpData::mimeType_ =
  {
    {".html", "text/html"},
    {".c", "text/plain"},
    {".doc", "application/msword"},
    {".htm", "text/html"},
    {".png", "image/png"},
    {".txt", "text/plain"},
    {"default", "text/html"}
  };

  HttpData::HttpData(HttpConnection* connection, int sockFd)
  : connection_(connection), 
    sockFd_(sockFd),
    //connectionState_(ConnectionState::H_CONNECTED),
    error_(false),
    processState_(ProcessState::STATE_PARSE_URI),
    headerState_(Parse_Header_State::H_START)
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
      LOG << "in handle Read";
      readSum = Utility::readn(sockFd_, readBuffer_, readFinish_);
      
      LOG << "readSum = " << readSum;

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

      LOG << readBuffer_;

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
        // analysis函数自身会处理出错信息
        AnalysisState flag = analysis();
        if(flag == AnalysisState::ANALYSIS_ERROR)
        {
          perror("analysis request error");
          error_ = true;
          break;
        }
        assert(flag == AnalysisState::ANALYSIS_SUCCESS);
        processState_ = ProcessState::STATE_PARSE_FINISH;
      }
      assert(processState_ == ProcessState::STATE_PARSE_FINISH);
      
    } while(false);

    if(!error_ && writeBuffer_.size() > 0)
    {
      connection_->updateWrite();
    }
  }

  void HttpData::handleWrite()
  {
    ssize_t writeSum = Utility::writen(sockFd_, writeBuffer_);
    if(writeSum < 0)
    {
      perror("write socket error");
      error_ = true;
      connection_->handleClose();
    }
    else if(writeBuffer_.empty())
    {
      connection_->handleClose();
    }
    else 
    {
      connection_->updateWrite();
    }
  }

  void HttpData::handleError(std::string errorCode, std::string errorMsg)
  {
    /* writeBuffer_.clear(); */
    errorMsg = " " + errorMsg;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += errorCode + errorMsg;
    body_buff += "<hr><em> Hby's Linux Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + errorCode + errorMsg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: Linux/han\r\n";
    header_buff += "\r\n";

    Utility::writen(sockFd_, header_buff);
    Utility::writen(sockFd_, body_buff);

    connection_->handleClose();
  }

  URIState HttpData::parseURI()
  { 
    size_t pos = readBuffer_.find('\r');
    /* if(pos == std::string::npos)
      return URIState::PARSE_URI_AGIAN; */
    // 暂时假定每次都读满请求数据,则以下断言成立
    assert(pos != std::string::npos);

    size_t end = pos;

    std::string request_line(readBuffer_.substr(0, pos));
    readBuffer_ = readBuffer_.substr(pos + 2);

    size_t findGet = request_line.find("GET", 0);
    size_t findPost = request_line.find("POST", 0);
    size_t findHead = request_line.find("HEAD", 0);

    // BUG: 如果URI中存在GET,POST,HEAD字符串?
    if(findGet != std::string::npos) 
      method_ = Method::HTTP_GET, pos = findGet;
    else if(findHead != std::string::npos)
      method_ = Method::HTTP_HEAD, pos = findHead;
    else if(findPost != std::string::npos)
      method_ = Method::HTTP_POST, pos = findPost;
    else
      return URIState::PARSE_URI_ERROR;
    
    pos = request_line.find(' ', pos);
    while(request_line[pos] == ' ') ++pos; 

    // 寻找URI和Version的分界空格
    size_t findBlank = request_line.find(' ', pos);
    if(findBlank == std::string::npos)
      return URIState::PARSE_URI_ERROR;
    
    std::string&& uri = request_line.substr(pos, findBlank - pos);
    size_t findSlash = uri.find('/');
    if(findSlash == std::string::npos || findSlash == uri.size() - 1)
      filename_ = "default.txt";
    else 
      filename_ = uri.substr(findSlash + 1);
    
    pos = findBlank;
    while(request_line[pos] == ' ') ++pos;
    if(pos == end) 
      return URIState::PARSE_URI_ERROR;

    findSlash = request_line.find('/', pos);
    if(findSlash == std::string::npos)
      return URIState::PARSE_URI_ERROR;
    version_ = request_line.substr(findSlash+1, end - findSlash);
    if(version_ != "1.1" && version_ != "1.0")
      return URIState::PARSE_URI_ERROR;
    
    return URIState::PARSE_URI_SUCCESS;
  }

  HeaderState HttpData::parseHeader()
  {
    size_t end = readBuffer_.find("\r\n\r\n");
    /* if(end == std::string::npos)
      return HeaderState::PARSE_HEADER_AGAIN; */
    // 暂时假定每次都读满请求数据, 则以下断言成立
    assert(end != std::string::npos);

    end+=4;

    size_t key_start, value_start;
    std::string key, value;
    for(int i = 0; i < end; ++i)
    {
      switch(headerState_)
      {
        case H_START:
        {
          if(readBuffer_[i] == '\n' || readBuffer_[i] == '\r') break;
          key_start = i;
          headerState_ = H_KEY;
          break;
        }
        case H_KEY:
        {
          if(readBuffer_[i] == '\n' || readBuffer_[i] == '\r') 
            return PARSE_HEADER_ERROR;
          if(readBuffer_[i] != ':') break;
          if(i == key_start) 
            return PARSE_HEADER_ERROR;
          key = readBuffer_.substr(key_start, i-key_start);
          headerState_ = H_COLON;
          break;
        }
        case H_COLON:
        {
          if(readBuffer_[i] != ' ') 
            return PARSE_HEADER_ERROR;
          headerState_ = H_VALUE_START;
          break;
        }
        case H_VALUE_START:
        {
          if(readBuffer_[i] == ' ' || readBuffer_[i] == '\r' || readBuffer_[i] == '\n')
            return PARSE_HEADER_ERROR;
          value_start = i;
          headerState_ = H_VALUE;
          break;
        }
        case H_VALUE:
        {
          if(readBuffer_[i] == '\n')
            return PARSE_HEADER_ERROR;
          if(readBuffer_[i] == '\r')
          {
            value = readBuffer_.substr(value_start, i - value_start);
            headers_[key] = value;
            headerState_ = H_START;
          }
          break;
        }
        default:
          return PARSE_HEADER_ERROR;
      }
    }
    assert(headerState_ == H_START);
    return PARSE_HEADER_SUCCESS;    
  }

  AnalysisState HttpData::analysis()
  {
    std::string header("HTTP/1.1 200 OK\r\n");

    auto dot = filename_.find('.');
    std::string filetype;
    if(dot == std::string::npos)
    {
      filetype = mimeType_["defualt"];
      filename_ += ".html";
    }
    else
    {
      auto it = mimeType_.find(filename_.substr(dot));
      if(it == mimeType_.end())
      {
        handleError("404", "Not Found");
        return ANALYSIS_ERROR;
      }
      else
        filetype = it->second;
    }
    
    struct stat st;
    if(stat(filename_.c_str(), &st) < 0)
    {
      handleError("404", "Not Found");
      return ANALYSIS_ERROR;
    }
    header += ("Content-Type: " + filetype + "\r\n");
    header += ("Content-Length: " + std::to_string(st.st_size) + "\r\n");
    header += ("Server: Linux/Han\r\n");
    header += "\r\n";

    writeBuffer_ = header;
    if(method_ == HTTP_HEAD)
      return ANALYSIS_SUCCESS;

    int src_fd = ::open(filename_.c_str(), O_RDONLY);
    if(src_fd < 0)
    {
      handleError("403", "Forbbiden");
      return ANALYSIS_ERROR;
    }
    void* ptr = ::mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if(ptr == MAP_FAILED)
    {
      handleError("500", "Internal Server Error");
      return ANALYSIS_ERROR;
    }
    writeBuffer_ += std::string((char*)ptr, (char*)ptr+st.st_size);
    munmap(ptr, st.st_size);
    return ANALYSIS_SUCCESS;
  }

}