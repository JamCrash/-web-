
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

map<string, string> m;
H_State state = H_START;

HeaderState parseHeader(string& parseStr)
{
  size_t end = parseStr.find("\r\n\r\n");
  if(end == string::npos)
    return HeaderState::PARSE_HEADER_AGAIN;

  end += 2;
  string&& str = parseStr.substr(0, end);
  size_t key_start, value_start;
  string key, value;
  for(int i=0;i<str.size();++i)
  {
    switch(state)
    {
      case H_START:
      {
        if(str[i] == '\n' || str[i] == '\r') break;
        key_start = i;
        state = H_KEY;
        break;
      }
      case H_KEY:
      {
        if(str[i] == '\n' || str[i] == '\r') 
          return PARSE_HEADER_ERROR;
        if(str[i] != ':') break;
        if(i == key_start) 
          return PARSE_HEADER_ERROR;
        key = str.substr(key_start, i-key_start);
        state = H_COLON;
        break;
      }
      case H_COLON:
      {
        if(str[i] != ' ') 
          return PARSE_HEADER_ERROR;
        state = H_VALUE_START;
        break;
      }
      case H_VALUE_START:
      {
        if(str[i] == ' ' || str[i] == '\r' || str[i] == '\n')
          return PARSE_HEADER_ERROR;
        value_start = i;
        state = H_VALUE;
        break;
      }
      case H_VALUE:
      {
        if(str[i] == ' ' || str[i] == '\n')
          return PARSE_HEADER_ERROR;
        if(str[i] == '\r')
        {
          value = str.substr(value_start, i - value_start);
          m[key] = value;
          state = H_START;
        }
        break;
      }
      default:
        return PARSE_HEADER_ERROR;
    }
  }
  assert(state == H_START);
  return PARSE_HEADER_SUCCESS;
}

int main()
{
  string test = "key1: han\r\nkey2: bao\r\nkey3: yu\r\n\r\n";
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
  assert(m["key1"] == "han");
  assert(m["key2"] == "bao");
  assert(m["key3"] == "yu");
}