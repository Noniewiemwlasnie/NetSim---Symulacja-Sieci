cmake_minimum_required(VERSION 3.14)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


set(SOURCES
    src/factory.cpp
    src/helpers.cpp
    src/nodes.cpp
    src/package.cpp
    src/storage_types.cpp
    src/simulation.cpp
    src/reports.cpp
    src/main.cpp
)

add_executable(netsim ${SOURCES})

target_link_libraries(netsim GTest::gtest_main)
