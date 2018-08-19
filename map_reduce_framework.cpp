#include "map_reduce_framework.h"

size_t GetFileSize(const std::string& filename)
{
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

std::vector<size_t> SplitFile(const std::string& filename, const size_t partsNum)
{
  std::vector<size_t> fileParts;
  fileParts.reserve(partsNum);
  const auto filesize = GetFileSize(filename);
  const auto partSize = filesize % partsNum
                        ? filesize / partsNum + 1
                        : filesize / partsNum;
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

void Shuffle(std::list<std::string>& map_results,
             std::vector<std::list<std::string>>& reduceParts,
             std::vector<std::mutex>& parts_for_reduce_mutexes)
{
  auto begin = map_results.cbegin();
  auto end = map_results.cend();
  while(begin != end) {
    if(begin->empty()) {
      ++begin;
      continue;
    }
    auto reduceIdx = (*begin)[0] % reduceParts.size();
    auto rangeEnd = std::upper_bound(begin,
                                    end,
                                    *begin,
                                    [] (const auto& lhs, const auto& rhs)
                                    {
                                      if(rhs.empty())
                                        return true;
                                      return lhs[0] < rhs[0];
                                    });
    std::lock_guard<std::mutex> lock(parts_for_reduce_mutexes[reduceIdx]);
    auto reduceItems = reduceParts[reduceIdx].size();
    reduceParts[reduceIdx].splice(reduceParts[reduceIdx].cend(), map_results, begin, rangeEnd);
    auto middleItr = reduceParts[reduceIdx].begin();
    std::advance(middleItr, reduceItems);
    std::inplace_merge(reduceParts[reduceIdx].begin(), middleItr, reduceParts[reduceIdx].end());
    begin = rangeEnd;
  }
}
