#pragma once

#include <string>
#include <list>
#include <functional>

void MapReduceExecute(const std::string& inFilename,
                      const std::string& outFilenameTemplate,
                      const unsigned short map_threads_num,
                      const unsigned short reduce_threads_num,
                      std::function<std::list<std::string>(const std::string&)> mapFunction,
                      std::function<std::list<std::string>(const std::list<std::string>&)> reduceFunction);
