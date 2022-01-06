cmake_minimum_required(VERSION 3.0)
project(megalo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -std=c++11 -Wall -Wno-unused-function")

link_directories(/usr/local/lib)

set(LIB_SRC
  megalo/address.cc
  megalo/bytearray.cc
  megalo/config.cc
  megalo/fiber.cc
  megalo/fd_manager.cc
  megalo/hook.cc
  megalo/http/http.cc
  megalo/http/http11_parser.rl.cc
  megalo/http/httpclient_parser.rl.cc
  megalo/http/http_parser.cc
  megalo/http/http_session.cc
  megalo/http/http_server.cc
  megalo/http/servlet.cc
  megalo/iomanager.cc
  megalo/log.cc
  megalo/scheduler.cc
  megalo/socket.cc
  megalo/stream.cc
  megalo/socket_stream.cc
  megalo/tcp_server.cc
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

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address megalo)
target_link_libraries(test_address ${LIB_LIB})

add_executable(test_socket tests/test_socket.cc)
add_dependencies(test_socket megalo)
target_link_libraries(test_socket ${LIB_LIB})

add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies(test_bytearray megalo)
target_link_libraries(test_bytearray ${LIB_LIB})

add_executable(test_http tests/test_http.cc)
add_dependencies(test_http megalo)
target_link_libraries(test_http ${LIB_LIB})

add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies(test_http_parser megalo)
target_link_libraries(test_http_parser ${LIB_LIB})

add_executable(test_tcp_server tests/test_tcp_server.cc)
add_dependencies(test_tcp_server megalo)
target_link_libraries(test_tcp_server ${LIB_LIB})

add_executable(echo_server examples/echo_server.cc)
add_dependencies(echo_server megalo)
target_link_libraries(echo_server ${LIB_LIB})

add_executable(test_http_server tests/test_http_server.cc)
add_dependencies(test_http_server megalo)
target_link_libraries(test_http_server ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
