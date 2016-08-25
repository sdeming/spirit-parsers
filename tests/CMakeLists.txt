cmake_minimum_required (VERSION 2.6)
project (tests)

find_package(Boost 1.54.0 REQUIRED)

set (EXECUTABLE_OUTPUT_PATH ".")

link_directories(${Boost_LIBRARY_DIRS})
link_libraries(boost_unit_test_framework)

include_directories(. ../include ${Boost_INCLUDE_DIRS})

# Tests
add_executable(ipv4_address_test ipv4_address_test.cpp)
add_executable(ipv6_address_test ipv6_address_test.cpp)
add_executable(uri_parser_test uri_parser_test.cpp)
add_executable(http11_parser_test http11_parser_test.cpp)
