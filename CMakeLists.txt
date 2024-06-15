cmake_minimum_required(VERSION 3.5)

project(sync)

set(CMAKE_CXX_FLAGS "-std=c++17")

add_subdirectory(test)

install(
  DIRECTORY .
  DESTINATION sync/
  FILES_MATCHING
  PATTERN "*.h")
