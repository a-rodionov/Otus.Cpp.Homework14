#include "map_reduce_framework.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <thread>
#include <mutex>


static size_t GetFileSize(const std::string& filename) {
  std::ifstream file(filename, std::ios::in);
  if(!file) {
    throw std::invalid_argument("Error! Can't open file.");
  }
  file.seekg(0, std::ios_base::end);
  auto position = file.tellg();
  if(std::iostream::pos_type(-1) == position) {
    throw std::runtime_error("Error! Can't get filesize.");
  }
  return position;
}


static std::vector<size_t> SplitFile(const std::string& filename, const size_t partsNum)
{
  std::vector<size_t> fileParts;
  fileParts.reserve(partsNum);
  const auto filesize = GetFileSize(filename);
  const auto partSize = filesize / partsNum;
  std::iostream::pos_type currentPosition{0};

  std::ifstream file(filename, std::ios::in);
  if(!file) {
    throw std::invalid_argument("Error! Can't open file.");
  }

  fileParts.push_back(0);
  std::string line;
  do {
    file.seekg(currentPosition + partSize);
    std::getline(file, line);
    auto position = file.tellg();
    if(std::iostream::pos_type(-1) == position) {
      fileParts.push_back(filesize);
    }
    else {
      fileParts.push_back(position);
    }
    currentPosition = position;
  }
  while(file);
  return fileParts;
}


template <typename Callable>
static void RunThreads(Callable&& callable, size_t count)
{
  std::vector<std::thread> threads(count);
  std::generate(threads.begin(),
                threads.end(),
                [i = 0, &callable] () mutable {
                  return std::thread(callable, i++);
                });
  for(auto& thread : threads) {
    if(thread.joinable())
      thread.join();
  }
}


static void MapCaller(const std::string& filename,
                      const size_t begin,
                      const size_t end,
                      std::list<std::string>& result,
                      std::function<std::list<std::string>(const std::string&)> mapFunction)
{
  try
  {
    std::ifstream file(filename, std::ios::in);
    if(!file) {
      throw std::invalid_argument("Error! Can't open file.");
    }
    file.seekg(begin);

    std::string line;
    do {
      std::getline(file, line);
      result.splice(result.cend(), mapFunction(line));
    }
    while( (file) && (file.tellg() < end) );
    result.sort();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}


static void Shuffle(std::list<std::string>& map_results,
                    std::vector<std::list<std::string>>& reduceParts,
                    std::mutex& parts_for_reduce_mutex)
{
  auto begin = map_results.cbegin();
  auto end = map_results.cend();
  while(begin != end) {
    if(begin->empty()) {
      ++begin;
      continue;
    }
    auto reduceIdx = (*begin)[0] % reduceParts.size();
    auto range = std::equal_range(begin, end, *begin);
    std::lock_guard<std::mutex> lock(parts_for_reduce_mutex);
    auto pos = std::lower_bound(reduceParts[reduceIdx].cbegin(), reduceParts[reduceIdx].cend(), *begin);
    reduceParts[reduceIdx].splice(pos, map_results, range.first, range.second);
    begin = range.second;
  }
}


static void ReduceCaller(const std::list<std::string>& part_for_reduce,
                         const std::string& filename,
                         std::function<std::list<std::string>(const std::list<std::string>&)> reduceFunction)
{
  try
  {
    auto reduce_result = reduceFunction(part_for_reduce);
    std::ofstream file(filename, std::ios::out);
    if(!file) {
      throw std::invalid_argument("Error! Can't open file for writing result.");
    }
    std::copy(reduce_result.cbegin(), reduce_result.cend(), std::ostream_iterator<std::string>(file, "\n"));
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}


void MapReduceExecute(const std::string& inFilename,
                      const std::string& outFilenameTemplate,
                      const unsigned short map_threads_num,
                      const unsigned short reduce_threads_num,
                      std::function<std::list<std::string>(const std::string&)> mapFunction,
                      std::function<std::list<std::string>(const std::list<std::string>&)> reduceFunction)
{
  auto parts_for_map{SplitFile(inFilename, map_threads_num)};

  std::vector<std::list<std::string>> map_results(parts_for_map.size());
  RunThreads([&inFilename, &parts_for_map, &map_results, &mapFunction] (const size_t i) {
              MapCaller(inFilename,
                        parts_for_map[i],
                        parts_for_map[i+1],
                        map_results[i],
                        mapFunction);
            },
            parts_for_map.size());
  parts_for_map.clear();

  std::mutex parts_for_reduce_mutex;
  std::vector<std::list<std::string>> parts_for_reduce(reduce_threads_num);
  RunThreads([&map_results, &parts_for_reduce, &parts_for_reduce_mutex] (const size_t i) {
              Shuffle(map_results[i],
                      parts_for_reduce,
                      parts_for_reduce_mutex);
            },
            map_results.size());
  map_results.clear();

  RunThreads([&parts_for_reduce, &outFilenameTemplate, &reduceFunction] (const size_t i) {
              std::string filename{outFilenameTemplate + '_' + std::to_string(i)};
              ReduceCaller(parts_for_reduce[i],
                           filename,
                           reduceFunction);
            },
            parts_for_reduce.size());
}
