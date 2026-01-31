cmake_minimum_required(VERSION 3.14)

project(NetSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB LIB_SOURCES "src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*src/main.cpp$")

add_library(netsim_lib STATIC ${LIB_SOURCES})
target_include_directories(netsim_lib PUBLIC include)

if(EXISTS "${CMAKE_SOURCE_DIR}/src/main.cpp")
    add_executable(netsim src/main.cpp)
    target_link_libraries(netsim PRIVATE netsim_lib)
endif()

file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(netsim_test ${TEST_SOURCES})
target_link_libraries(netsim_test PRIVATE netsim_lib GTest::gtest_main)
