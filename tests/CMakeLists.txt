cmake_minimum_required (VERSION 2.6)
project (tests)

set (BOOST_LIB_PATH /opt/boost/lib)
set (BOOST_LIBS ${BOOST_LIB_PATH}/libboost_unit_test_framework.a)
set (BOOST_INCLUDE_PATH /opt/boost/include)
set (EXECUTABLE_OUTPUT_PATH ".")

link_directories(${BOOST_LIB_PATH})
link_libraries(${BOOST_LIBS})

include_directories(. ../include ${BOOST_INCLUDE_PATH})

# Tests
add_executable(http11_parser_test http11_parser_test.cpp)
add_executable(uri_parser_test uri_parser_test.cpp)
