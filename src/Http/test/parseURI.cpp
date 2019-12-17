
#include <string>
#include <stdio.h>
#include <assert.h>
#include <iostream>

using namespace std;

enum URIState
{
  PARSE_URI_SUCCESS,
  PARSE_URI_AGIAN,
  PARSE_URI_ERROR
};

enum Method
{
  H_GET, 
  H_POST,
  H_HEAD
};

string version;
Method method;
string filename;

URIState parseURI(string& str)
{
  size_t pos = str.find('\r', 0);
  if(pos == string::npos)
    return URIState::PARSE_URI_AGIAN;

  size_t end = pos;

  string request_line(str.substr(0, pos));

  size_t findGet = request_line.find("GET", 0);
  size_t findPost = request_line.find("POST", 0);
  size_t findHead = request_line.find("HEAD", 0);

  // BUG: 如果URI中存在GET,POST,HEAD字符串?
  if(findGet != string::npos) 
    method = Method::H_GET, pos = findGet;
  else if(findHead != string::npos)
    method = Method::H_HEAD, pos = findHead;
  else if(findPost != string::npos)
    method = Method::H_POST, pos = findPost;
  else
    return URIState::PARSE_URI_ERROR;
  
  pos = request_line.find(' ', pos);
  while(request_line[pos] == ' ') ++pos; 

  // 寻找URI和Version的分界空格
  size_t findBlank = request_line.find(' ', pos);
  if(findBlank == string::npos)
    return URIState::PARSE_URI_ERROR;
  
  string&& uri = request_line.substr(pos, findBlank - pos);
  size_t findSlash = uri.find('/');
  if(findSlash == string::npos || findSlash == uri.size() - 1)
    filename = "default.txt";
  else 
    filename = uri.substr(findSlash + 1);
  
  pos = findBlank;
  while(request_line[pos] == ' ') ++pos;
  if(pos == end) 
    return URIState::PARSE_URI_ERROR;

  findSlash = request_line.find('/', pos);
  if(findSlash == string::npos)
    return URIState::PARSE_URI_ERROR;
  version = request_line.substr(findSlash+1, end - findSlash);
  if(version != "1.1" && version != "1.0")
    return URIState::PARSE_URI_ERROR;
  
  return URIState::PARSE_URI_SUCCESS;
}

int main()
{
  string test = "GETs 127.0.0.1/adf HTTP/1.1\r\n";
  URIState flag = parseURI(test);
  assert(flag == URIState::PARSE_URI_SUCCESS);
  cout << version << endl;
  cout << filename << endl;
  if(method == Method::H_GET)
    cout << "GET" << endl;
  if(method == Method::H_HEAD)
    cout << "HEAD" << endl;
  if(method == Method::H_POST)
    cout << "POST" << endl;  

  
}
