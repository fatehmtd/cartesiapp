cmake_minimum_required(VERSION 3.16)

project(CartesiaPP_Lib VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

# add Boost
find_package(Boost REQUIRED COMPONENTS system thread)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost or set BOOST_ROOT.")
endif()

# Debug: Print Boost information
message(STATUS "Boost_FOUND: ${Boost_FOUND}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

# add OpenSSL
find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL or set OPENSSL_ROOT_DIR.")
endif()

# add spdlog for logging
find_package(spdlog REQUIRED CONFIG)

# add nlohmann_json for JSON handling
find_package(nlohmann_json REQUIRED CONFIG)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

# define the library
add_library(cartesiapp STATIC
    src/cartesiapp.cpp
    include/cartesiapp.hpp
)

target_link_libraries(cartesiapp
    PRIVATE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${OPENSSL_LIBRARIES}
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(cartesiapp
    BASE_NAME cartesiapp
    EXPORT_FILE_NAME cartesiapp_export.h
)