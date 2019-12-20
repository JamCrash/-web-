
#ifndef _LOGGING_H
#define _LOGGING_H

#include "noncopyable.h"

#include <iostream>
#include <sstream>

struct Logger: public Base::noncopyable 
{
  std::ostringstream oss;

  Logger(){};
  
  ~Logger()
  {
    std::cout << oss.str() << std::endl;
  }
};

#define LOG Logger().oss

#endif  // _LOGGING_H
