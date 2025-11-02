
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add spdlog for logging
find_package(spdlog REQUIRED CONFIG)

add_executable(CartesiaPP_Sample
    sample.cpp
)

target_link_libraries(CartesiaPP_Sample
    PRIVATE
    cartesiapp
    spdlog::spdlog
)