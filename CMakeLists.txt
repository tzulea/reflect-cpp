cmake_minimum_required(VERSION 3.4)

project(reflectcpp)

option(REFLECT_STATCI_LIB "Build static library" ON)
option(REFLECT_DYNAMIC_LIB "Build dynamic library" OFF)
option(REFLECT_BUILD_TESTS "Build testcases" OFF)


include_directories(
    "include"
)

if(REFLECT_BUILD_TESTS)
  add_subdirectory(tests)
endif(REFLECT_BUILD_TESTS)

if(REFLECT_DYNAMIC_LIB)
  add_library(reflectcpp SHARED "src/yyjson.c")
endif(REFLECT_DYNAMIC_LIB)
if(REFLECT_STATCI_LIB)
  add_library(reflectcpp_static STATIC "src/yyjson.c")
endif(REFLECT_STATCI_LIB)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
