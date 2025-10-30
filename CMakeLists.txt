cmake_minimum_required(VERSION 3.16)

project(CartesiaPP VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_SAMPLE "Build sample application" ON)

add_subdirectory(lib/cartesiapp)

if(BUILD_SAMPLE)
  add_subdirectory(sample)
endif()