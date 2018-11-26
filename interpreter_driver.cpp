#include <iostream>
#include "interpreter.h"
using namespace std;

int main()
{
  cout << "running interpreter..." << endl;
  Interpreter story("test-example.html");
  story.start();
  // story.start();
  return 0;
}
