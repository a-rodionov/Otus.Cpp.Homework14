#include "functions.h"
#include <iostream>

int main(int argc, char const* argv[])
{
  try
  {
    std::string filename;
    unsigned short mnum, rnum;
    CheckInputParameters(argc, argv, filename, mnum, rnum);
    Process(filename, mnum, rnum);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
