#pragma once

#include <list>
#include <string>

struct MapFunctor
{
  std::list<std::string> operator() (const std::string& data);
};
