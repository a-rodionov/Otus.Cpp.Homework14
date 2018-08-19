#pragma once

#include <list>
#include <string>

class ReduceFunctor
{
public:

  ReduceFunctor();

  void operator() (const std::string& data);

  operator std::list<std::string> ();

private:

  size_t prefix_size;
  std::string previousData;
};
