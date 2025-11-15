cmake_minimum_required(VERSION 3.16)

project(CartesiaPP_Lib VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

# add Boost
find_package(Boost REQUIRED COMPONENTS)

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

# fetch base64
set(BASE64_ENABLE_TESTING OFF CACHE BOOL "Disable base64 testing" FORCE)
FetchContent_Declare(
    base64
    GIT_REPOSITORY https://github.com/tobiaslocker/base64.git
)
FetchContent_MakeAvailable(base64)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

# define the library
add_library(cartesiapp STATIC
    src/cartesiapp.cpp
    src/cartesiapp_request.cpp
    src/cartesiapp_response.cpp
    include/cartesiapp.hpp
)

target_include_directories(cartesiapp PRIVATE
    ./include
)

target_link_libraries(cartesiapp
    PRIVATE
    spdlog::spdlog_header_only
    nlohmann_json::nlohmann_json
    ${OPENSSL_LIBRARIES}
    base64
)

# Generate export header
include(GenerateExportHeader)
generate_export_header(cartesiapp
    BASE_NAME cartesiapp
    EXPORT_FILE_NAME cartesiapp_export.hpp
)

target_include_directories(cartesiapp PUBLIC
    $<INSTALL_INTERFACE:include> # For installed package (optional but good practice)
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # For direct build
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # To export the symbols header
    #PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/gladiapp # to simplify the internal include files
)

# Installation rules
install(TARGETS cartesiapp
    EXPORT cartesiappTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include)

install(DIRECTORY include/
    DESTINATION include/cartesiapp
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "impl" EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cartesiapp_export.hpp
    DESTINATION include/cartesiapp)