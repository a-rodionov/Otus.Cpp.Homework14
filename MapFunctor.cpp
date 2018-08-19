#include "MapFunctor.h"
#include <algorithm>

std::list<std::string> MapFunctor::operator() (const std::string& data)
{
  std::list<std::string> result{data.size()};
  std::generate(result.begin(),
                result.end(),
                [pos = 1, &data] () mutable {
                  return data.substr(0, pos++);
                });
  return result;
}
