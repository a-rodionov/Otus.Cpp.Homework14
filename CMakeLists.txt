cmake_minimum_required(VERSION 3.2)

# Определение PATCH_VERSION для локальной сборки и сборки на travis
set(PATCH_VERSION 1)
if($ENV{TRAVIS_BUILD_NUMBER})
  set(PATCH_VERSION $ENV{TRAVIS_BUILD_NUMBER})
endif()

project(yamr VERSION 1.0.${PATCH_VERSION})

find_package(Threads)
find_package(Boost COMPONENTS unit_test_framework system thread REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
elseif ((CMAKE_BUILD_TYPE STREQUAL "Release")
        OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Создание целей
add_executable(yamr main.cpp)

add_library(yamr_lib STATIC functions.cpp map_reduce_framework.cpp)

# Настройка для всех целей
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wpedantic -Wall -Wextra)

target_compile_definitions(yamr PRIVATE)
target_link_libraries(yamr
  yamr_lib
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)

install(TARGETS yamr RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT camelot58@gmail.com)

include (CPack)

enable_testing()

add_subdirectory(tests)
