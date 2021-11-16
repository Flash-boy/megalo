cmake_minimum_required(VERSION 3.0)
project(megalo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-unused-function")

set(LIB_SRC
  megalo/log.cc
  megalo/util.cc
  megalo/config.cc
  )

add_library(megalo SHARED ${LIB_SRC})

add_executable(test tests/test.cc)
add_dependencies(test megalo)
target_link_libraries(test megalo)

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config megalo)
target_link_libraries(test_config megalo)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
