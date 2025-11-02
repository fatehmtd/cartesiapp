cmake_minimum_required(VERSION 3.16)

project(CartesiaPP VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_SAMPLES "Build sample applications" ON)

add_subdirectory(lib/cartesiapp)

if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif()