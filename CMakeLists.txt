cmake_minimum_required(VERSION 3.0)
project(megalo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-unused-function")

link_directories(/usr/local/lib)

set(LIB_SRC
  megalo/config.cc
  megalo/fiber.cc
  megalo/log.cc
  megalo/thread.cc
  megalo/util.cc
  )

set(LIB_LIB
  megalo
  yaml-cpp
  pthread
  )

add_library(megalo SHARED ${LIB_SRC})

add_executable(test tests/test.cc)
add_dependencies(test megalo)
target_link_libraries(test ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config megalo)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread megalo)
target_link_libraries(test_thread ${LIB_LIB})

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util megalo)
target_link_libraries(test_util ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber megalo)
target_link_libraries(test_fiber ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
