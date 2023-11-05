cmake_minimum_required(VERSION 2.8.3)

project(sync)

set(CMAKE_CXX_FLAGS "-std=c++14")

add_subdirectory(test)

install(
  DIRECTORY .
  DESTINATION sync/
  FILES_MATCHING
  PATTERN "*.h")
