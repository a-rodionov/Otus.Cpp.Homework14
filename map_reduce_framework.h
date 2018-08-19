#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <thread>
#include <mutex>

size_t GetFileSize(const std::string& filename);

std::vector<size_t> SplitFile(const std::string& filename, const size_t partsNum);

void Shuffle(std::list<std::string>& map_results,
             std::vector<std::list<std::string>>& reduceParts,
             std::vector<std::mutex>& parts_for_reduce_mutexes);

template <typename Callable>
void RunThreads(Callable&& callable, size_t count)
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

template<typename MapFunctor>
void MapCaller(const std::string& filename,
               const size_t begin,
               const size_t end,
               std::list<std::string>& result)
{
  try
  {
    std::ifstream file(filename, std::ios::in);
    if(!file) {
      throw std::invalid_argument("Error! Can't open file.");
    }
    file.seekg(begin);

    MapFunctor mapFunctor;
    std::string line;
    do {
      std::getline(file, line);
      result.splice(result.cend(), mapFunctor(line));
    }
    while( (file) && (file.tellg() < end) );
    result.sort();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

template<typename ReduceFunctor>
void ReduceCaller(const std::list<std::string>& part_for_reduce,
                  const std::string& filename)
{
  try
  {
    ReduceFunctor reduceFunctor;
    for(const auto& data : part_for_reduce) {
      reduceFunctor(data);
    }
    std::list<std::string> reduce_result = reduceFunctor;
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

template<typename MapFunctor, typename ReduceFunctor>
void MapReduceExecute(const std::string& inFilename,
                      const std::string& outFilenameTemplate,
                      const unsigned short map_threads_num,
                      const unsigned short reduce_threads_num)
{
  auto parts_for_map{SplitFile(inFilename, map_threads_num)};

  std::vector<std::list<std::string>> map_results(parts_for_map.size()-1);
  RunThreads([&inFilename, &parts_for_map, &map_results] (const size_t i) {
              MapCaller<MapFunctor>(inFilename,
                        parts_for_map[i],
                        parts_for_map[i+1],
                        map_results[i]);
            },
            parts_for_map.size()-1);
  parts_for_map.clear();

  std::vector<std::mutex> parts_for_reduce_mutexes(reduce_threads_num);
  std::vector<std::list<std::string>> parts_for_reduce(reduce_threads_num);
  RunThreads([&map_results, &parts_for_reduce, &parts_for_reduce_mutexes] (const size_t i) {
              Shuffle(map_results[i],
                      parts_for_reduce,
                      parts_for_reduce_mutexes);
            },
            map_results.size());
  map_results.clear();

  RunThreads([&parts_for_reduce, &outFilenameTemplate] (const size_t i) {
              std::string filename{outFilenameTemplate + '_' + std::to_string(i)};
              ReduceCaller<ReduceFunctor>(parts_for_reduce[i],
                           filename);
            },
            parts_for_reduce.size());
}
