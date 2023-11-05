
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR})

add_executable(sync_test sync_test.cpp)
target_link_libraries(sync_test ${GTEST_BOTH_LIBRARIES} pthread)

install(TARGETS sync_test DESTINATION bin)
