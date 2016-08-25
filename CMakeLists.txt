cmake_minimum_required (VERSION 2.6)
project (spirit-parsers)

find_package(Boost 1.54.0 REQUIRED)

#link_directories(${Boost_LIBRARY_DIRS})

include_directories(. ./include ${Boost_INCLUDE_DIRS})

add_subdirectory(tests)
