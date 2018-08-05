#pragma once

#include <string>

void CheckInputParameters(int argc,
                          char const* argv[],
                          std::string& filename,
                          unsigned short& mnum,
                          unsigned short& rnum);

void Process(const std::string& filename,
             const unsigned short mnum,
             const unsigned short rnum);
