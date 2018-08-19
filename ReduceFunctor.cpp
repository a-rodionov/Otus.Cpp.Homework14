#include "ReduceFunctor.h"

ReduceFunctor::ReduceFunctor()
  : prefix_size{1}
{
}

void ReduceFunctor::operator() (const std::string& data)
{
  if(previousData == data) {
    prefix_size = std::max(prefix_size, data.size()+1);
  }
  else {
    previousData = data;
  }
}

ReduceFunctor::operator std::list<std::string> ()
{
  return std::list<std::string>(1, std::to_string(prefix_size));
}
