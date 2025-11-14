
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add spdlog for logging
find_package(spdlog REQUIRED CONFIG)

# Create a custom target to copy data folder (runs only once)
add_custom_target(copy_data_folder ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_CURRENT_BINARY_DIR}/data
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data
    COMMENT "Copying data folder to binary directory"
)

# Text-to-Speech with Bytes sample
add_executable(CartesiaPP_Sample_TTS_Bytes
    sample-tts-bytes.cpp
)

# Make executable depend on data being copied
add_dependencies(CartesiaPP_Sample_TTS_Bytes copy_data_folder)

target_link_libraries(CartesiaPP_Sample_TTS_Bytes
    PRIVATE
    cartesiapp
    spdlog::spdlog
)

# Speech-to-Text with File sample
add_executable(CartesiaPP_Sample_STT_File
    sample-stt-file.cpp
)

target_link_libraries(CartesiaPP_Sample_STT_File
    PRIVATE
    cartesiapp
    spdlog::spdlog
)

# Text to Speech with streaming sample
add_executable(CartesiaPP_Sample_TTS_Streaming
    sample-tts-streaming.cpp
)

target_link_libraries(CartesiaPP_Sample_TTS_Streaming
    PRIVATE
    cartesiapp
    spdlog::spdlog
)

# Speech to Text with streaming sample
add_executable(CartesiaPP_Sample_STT_Streaming
    sample-stt-streaming.cpp
)

target_link_libraries(CartesiaPP_Sample_STT_Streaming
    PRIVATE
    cartesiapp
    spdlog::spdlog
)


# Make executable depend on data being copied
add_dependencies(CartesiaPP_Sample_STT_File copy_data_folder)