#include "functions.h"
#include "map_reduce_framework.h"
#include <fstream>
#include <algorithm>
#include <list>

std::string RESULT_FILENAME_TEMPLATE{"result"};

static unsigned short TryParseUnsignedShort(char const* parameter)
{
  unsigned long long value;
  std::string digit_str{parameter};
  if(!std::all_of(std::cbegin(digit_str),
                  std::cend(digit_str),
                  [](unsigned char symbol) { return std::isdigit(symbol); } )) {
    throw std::invalid_argument("");
  }
  value = std::stoull(digit_str);
  if(value > std::numeric_limits<unsigned short>::max()) {
    throw std::invalid_argument("");
  }
  return value;
}

void CheckInputParameters(int argc,
                          char const* argv[],
                          std::string& filename,
                          unsigned short& mnum,
                          unsigned short& rnum)
{
  try
  {
    if(4 != argc) {
      throw std::invalid_argument("");
    }

    mnum = TryParseUnsignedShort(argv[2]);
    if( 0 == mnum ) {
      throw std::invalid_argument("Number of threads for MAP can't be equal to zero.");
    }
    rnum = TryParseUnsignedShort(argv[3]);
    if( 0 == rnum ) {
      throw std::invalid_argument("Number of threads for REDUCE can't be equal to zero.");
    }

    filename = std::string{argv[1]};
    std::ifstream file(filename, std::ios::in);
    if(!file) {
      std::string error_msg{"Error! Can't open file "};
      error_msg += filename;
      throw std::runtime_error(error_msg);
    }
  }
  catch(const std::exception& exc)
  {
    std::string error_msg{exc.what()};
    error_msg += "\nThe programm must be started with 3 parameters.\n"
                "1st - absolute path to input file.\n"
                "2nd - number of threads that will MAP input.\n"
                "3rd - number of threads that will REDUCE result.\n"
                "2nd and 3rd parameters must be in range 1 - "
                + std::to_string(std::numeric_limits<unsigned short>::max());
    throw std::invalid_argument(error_msg);
  }
}

auto MapFunction(const std::string& line) {
  std::list<std::string> result{line.size()};
  std::generate(result.begin(),
                result.end(),
                [pos = 1, &line] () mutable {
                  return line.substr(0, pos++);
                });
  return result;
}

auto ReduceFunction(const std::list<std::string>& lines) {
  std::list<std::string> result;
  size_t prefix_size{0};
  auto currentPosition = lines.cbegin();
  auto end = lines.cend();
  while(currentPosition != end) {
    currentPosition = std::adjacent_find(currentPosition, lines.cend());
    if(end == currentPosition) {
      break;
    }
    prefix_size = std::max(prefix_size, currentPosition->size()+1);
    auto previousCurrentPosition = currentPosition;
    do {
      ++currentPosition;
    }
    while((currentPosition != end)
          && (*currentPosition == *previousCurrentPosition));
  }
  result.push_back(std::to_string(prefix_size));
  return result;
};

void Process(const std::string& filename,
             const unsigned short mnum,
             const unsigned short rnum)
{
  MapReduceExecute(filename, RESULT_FILENAME_TEMPLATE, mnum, rnum, MapFunction, ReduceFunction);
}
