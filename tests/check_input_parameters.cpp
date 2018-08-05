#define BOOST_TEST_IGNORE_NON_ZERO_CHILD_CODE
#define BOOST_TEST_MODULE test_async

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

#include "functions.h"
#include <fstream>

static std::string EMAIL_FILENAME{"emails"};

struct initialized_test
{
  initialized_test()
  {
    std::system(cmdRemoveEmails.c_str());
    std::ofstream file(EMAIL_FILENAME, std::ios::out);
    file << "some emails";
  }

  ~initialized_test() {
    std::system(cmdRemoveEmails.c_str());
  }

  const std::string cmdRemoveEmails{"rm -f " + EMAIL_FILENAME};
};

BOOST_FIXTURE_TEST_SUITE(test_suite_main, initialized_test)

BOOST_AUTO_TEST_CASE(correct_parameters)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "7", "9"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_NO_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum));
  BOOST_CHECK_EQUAL("emails", filename);
  BOOST_CHECK_EQUAL(7, mnum);
  BOOST_CHECK_EQUAL(9, rnum);
}

BOOST_AUTO_TEST_CASE(incorrect_number_of_parameters_1)
{
  int argc = 3;
  const char* argv[3] = {"application", "emails", "7"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_number_of_parameters_2)
{
  int argc = 2;
  const char* argv[2] = {"application", "emails"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_number_of_parameters_3)
{
  int argc = 1;
  const char* argv[1] = {"application"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_number_of_parameters_4)
{
  int argc = 5;
  const char* argv[5] = {"application", "emails", "7", "9", "extra_parameter"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_filename)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails_not_exist", "7", "9"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_map_number_1)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "0", "9"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_map_number_2)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "-1", "9"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_map_number_3)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "mnum", "9"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_reduce_number_1)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "7", "0"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_reduce_number_2)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "7", "-1"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(incorrect_reduce_number_3)
{
  int argc = 4;
  const char* argv[4] = {"application", "emails", "7", "rnum"};
  std::string filename;
  unsigned short mnum{0}, rnum{0};
  
  BOOST_CHECK_THROW(CheckInputParameters(argc, argv, filename, mnum, rnum), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
