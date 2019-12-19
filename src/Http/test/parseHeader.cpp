
#include <string>
#include <cassert>
#include <map>
#include <iostream>

using namespace std;

enum HeaderState
{
  PARSE_HEADER_SUCCESS,
  PARSE_HEADER_ERROR,
  PARSE_HEADER_AGAIN
};

enum H_State
{
  H_START,
  H_KEY,
  H_COLON,
  H_VALUE_START,
  H_VALUE,

};

map<string, string> headers_;
H_State headerState_ = H_START;

  HeaderState parseHeader(string& readBuffer_)
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

int main()
{
  string test("Host: 127.0.0.1:3000\r\n\
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:71.0) Gecko/20100101 Firefox/71.0\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
Accept-Language: en-US,en;q=0.5\r\n\
Accept-Encoding: gzip, deflate\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\r\n1");
  HeaderState flag = parseHeader(test);
  switch (flag)
  {
    case PARSE_HEADER_AGAIN:
      cout << "again" << endl;
      break;
    case PARSE_HEADER_ERROR:
      cout << "error" << endl;
      break;
    case PARSE_HEADER_SUCCESS:
      cout << "success" << endl;
      break;
  }
  for(auto p: headers_)
    cout << p.first << "--->>>" << p.second << endl;
  
}
