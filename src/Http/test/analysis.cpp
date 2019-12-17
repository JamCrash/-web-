
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

using namespace std;

enum AnalysisState
{
  ANALYSIS_SUCCESS,
  ANALYSIS_ERROR
};
enum Method
{
  H_GET,
  H_HEAD
};

string writeBuffer;
map<string, string> m;
Method method;

// 返回错误码或者成功
AnalysisState analysis(string& filename)
{
  string header="HTTP/1.1 200 OK\r\n";

  auto dot = filename.find('.');
  string filetype;
  if(dot == string::npos)
  {
    filetype = m["defualt"];
    filename += ".txt";
  }
  else 
  {
    auto it = m.find(filename.substr(dot));
    if(it == m.end())
    {
      // handleError("404", "Not Found")
      cout << "404 Not Found" << endl;
      return ANALYSIS_ERROR;
    }
    else
      filetype = it->second;
  }
  
  struct stat st;
  if(stat(filename.c_str(), &st) < 0)
  {
    cout << "404 Not Found" << endl;
    return ANALYSIS_ERROR;
  }
  header += ("Content-Type: " + filetype + "\r\n");
  header += ("Content-Length: " + to_string(st.st_size) + "\r\n");
  header += ("Server: Linux/Han\r\n");
  header += "\r\n";

  writeBuffer = header;
  if(method == H_HEAD)
    return ANALYSIS_SUCCESS;

  int src_fd = ::open(filename.c_str(), O_RDONLY);
  if(src_fd < 0)
  {
    writeBuffer.clear();
    cout << "403 Forbbiden" << endl;
    return ANALYSIS_ERROR;
  }
  void* ptr = ::mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
  if(ptr == MAP_FAILED)
  {
    writeBuffer.clear();
    cout << "500 Internal Server Error" << endl;
    return ANALYSIS_ERROR;
  }
  writeBuffer += string((char*)ptr, (char*)ptr+st.st_size);
  munmap(ptr, st.st_size);
  return ANALYSIS_SUCCESS;
}

int main()
{
  m[".txt"]="txt/plain";
  m["default"]="txt/plain";
  string filename("test");
  AnalysisState flag = analysis(filename);
  assert(flag==ANALYSIS_SUCCESS);
  cout << writeBuffer << endl;
}