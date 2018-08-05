#define BOOST_TEST_IGNORE_NON_ZERO_CHILD_CODE
#define BOOST_TEST_MODULE test_async

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include "functions.h"
#include <fstream>

static std::string EMAIL_FILENAME{"emails"};
extern std::string RESULT_FILENAME_TEMPLATE;
static std::string RESULT_FILENAME_ALL{"result_*"};
static std::string RESULT_FILENAME_CONCATENATED{"result_total"};
static std::string RESULT_FILENAME_SORTED{"result_sorted"};

struct initialized_test
{
  initialized_test()
  {
    std::system(cmdRemoveResults.c_str());
    std::system(cmdRemoveEmails.c_str());

    const std::string emails = 
      "bob.dylan@gmail.com\n"
      "bob.fisher@gmail.com\n"
      "aleksey@mail.ru\n"
      "dmitryi@mail.ru\n"
      "vladislav@mail.ru\n"
      "marla@gmail.com\n"
      "aleksandr@mail.ru\n"
      "maxim@mail.ru\n"
      "stanislav@mail.ru\n"
      "bob.marley@gmail.com\n"
      "eugene@mail.ru\n"
      "denis@mail.ru\n";

    std::ofstream file(EMAIL_FILENAME, std::ios::out);
    file << emails;
  }

  ~initialized_test() {
    std::system(cmdRemoveResults.c_str());
    std::system(cmdRemoveEmails.c_str());
  }

  const std::string ethalon = 
    "aleksa\n"
    "alekse\n"
    "bob.d\n"
    "bob.f\n"
    "bob.m\n"
    "de\n"
    "dm\n"
    "e\n"
    "mar\n"
    "max\n"
    "s\n"
    "v\n";
  const std::string cmdRemoveResults{"rm -f " + RESULT_FILENAME_ALL};
  const std::string cmdRemoveEmails{"rm -f " + EMAIL_FILENAME};
};

BOOST_FIXTURE_TEST_SUITE(test_suite_main, initialized_test)

void CheckResult(const std::string& ethalon)
{
  const std::string cmdConcatResults{"cat " + RESULT_FILENAME_ALL + " >> " + RESULT_FILENAME_CONCATENATED};
  const std::string cmdSortResults{"sort -o " + RESULT_FILENAME_SORTED + ' ' + RESULT_FILENAME_CONCATENATED};

  std::system(cmdConcatResults.c_str());  
  std::system(cmdSortResults.c_str());

  std::ifstream file(RESULT_FILENAME_SORTED, std::ios::in);
  std::stringstream ss;
  ss << file.rdbuf();
  BOOST_REQUIRE_EQUAL(ss.str(), ethalon);
}

BOOST_AUTO_TEST_CASE(check_one_reduce_file_exist)
{
  Process(EMAIL_FILENAME, 1, 1);
  std::ifstream file(RESULT_FILENAME_TEMPLATE + "_0", std::ios::in);
  BOOST_REQUIRE(true == file.is_open());
  std::ifstream file2(RESULT_FILENAME_TEMPLATE + "_1", std::ios::in);
  BOOST_REQUIRE(false == file2.is_open());
}

BOOST_AUTO_TEST_CASE(check_two_reduce_file_exist)
{
  Process(EMAIL_FILENAME, 1, 2);
  std::ifstream file(RESULT_FILENAME_TEMPLATE + "_0", std::ios::in);
  BOOST_REQUIRE(true == file.is_open());
  std::ifstream file2(RESULT_FILENAME_TEMPLATE + "_1", std::ios::in);
  BOOST_REQUIRE(true == file2.is_open());
  std::ifstream file3(RESULT_FILENAME_TEMPLATE + "_2", std::ios::in);
  BOOST_REQUIRE(false == file3.is_open());
}

BOOST_AUTO_TEST_CASE(one_map_one_reduce)
{
  Process(EMAIL_FILENAME, 1, 1);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(two_map_one_reduce)
{
  Process(EMAIL_FILENAME, 2, 1);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(three_map_one_reduce)
{
  Process(EMAIL_FILENAME, 3, 1);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(one_map_two_reduce)
{
  Process(EMAIL_FILENAME, 1, 2);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(two_map_two_reduce)
{
  Process(EMAIL_FILENAME, 2, 2);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(three_map_two_reduce)
{
  Process(EMAIL_FILENAME, 3, 2);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(one_map_three_reduce)
{
  Process(EMAIL_FILENAME, 1, 3);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(two_map_three_reduce)
{
  Process(EMAIL_FILENAME, 2, 3);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_CASE(three_map_three_reduce)
{
  Process(EMAIL_FILENAME, 3, 3);
  CheckResult(ethalon);
}

BOOST_AUTO_TEST_SUITE_END()
