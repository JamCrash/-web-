
#include "../Thread.h"

#include <iostream>

using namespace std;
using namespace Base;
using namespace Base::ThreadDef;

void f()
{
  cout << "in new thread" << endl;
}

int main()
{
  Thread thread(f);
  thread.start();
  thread.join();
}
