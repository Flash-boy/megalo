cmake_minimum_required(VERSION 3.0)
project(megalo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-unused-function")

link_directories(/usr/local/lib)

set(LIB_SRC
  megalo/config.cc
  megalo/fiber.cc
  megalo/fd_manager.cc
  megalo/hook.cc
  megalo/iomanager.cc
  megalo/log.cc
  megalo/scheduler.cc
  megalo/thread.cc
  megalo/timer.cc
  megalo/util.cc
  )

set(LIB_LIB
  megalo
  yaml-cpp
  pthread
  dl
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

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler megalo)
target_link_libraries(test_scheduler ${LIB_LIB})

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager megalo)
target_link_libraries(test_iomanager ${LIB_LIB})

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook megalo)
target_link_libraries(test_hook ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
