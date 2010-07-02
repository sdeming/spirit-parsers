cmake_minimum_required (VERSION 2.6)
project (spirit-parsers)

set (BOOST_LIB_PATH /opt/boost/lib)
set (BOOST_LIBS boost_unit_test_framework)
set (BOOST_INCLUDE_PATH /opt/boost/include)

link_directories(${BOOST_LIB_PATH})
link_libraries(${BOOST_LIBS})

include_directories(. ./include ${BOOST_INCLUDE_PATH})

add_subdirectory(tests)
