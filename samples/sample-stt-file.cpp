/**
 * @file sample-stt-file.cpp
 * @brief Sample demonstrating Speech-to-Text batch transcription from files
 * 
 * This sample shows how to:
 * - Transcribe audio files using batch STT
 * - Extract word-level timestamps
 * - Handle various audio formats (MP3, WAV, etc.)
 */

#include <cartesiapp/cartesiapp.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>

/**
 * @brief Demonstrates STT batch transcription from an audio file
 */
bool testSTTWithFile(cartesiapp::Cartesia& client) {
    cartesiapp::request::stt::BatchRequest sttRequest;
    // you can omit optional parameters to use defaults from the actual file's header
    //sttRequest.language = "en";
    //sttRequest.sample_rate = 16000;
    //sttRequest.encoding = cartesiapp::request::stt_encoding::PCM_S16LE;

    cartesiapp::response::stt::TranscriptionResponse response = client.sttWithFile(
        "../data/tts_output.mp3",
         sttRequest
    );
    
    spdlog::info("STT With File Response:");
    spdlog::info("Type: {}", response.type);
    spdlog::info("Request ID: {}", response.request_id);
    spdlog::info("Transcribed Text: {}", response.text);
    spdlog::info("Language: {}", response.language.has_value() ? response.language.value() : "N/A");
    spdlog::info("Duration: {} seconds", response.duration);
    spdlog::info("Is Final: {}", response.is_final);

    spdlog::info("> Word Timings:");
    for (const auto& wordTiming : response.words) {
        spdlog::info("Word: '{}', Start: {} ms, End: {} ms",
            wordTiming.word, wordTiming.start, wordTiming.end);
    }
    return true;
}

int main(int ac, char** av) {

#ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    const char* apiKeyEnv = std::getenv("CARTESIA_API_KEY");
    std::string apiKey = apiKeyEnv ? apiKeyEnv : "";
    std::string apiVersion = cartesiapp::request::api_versions::LATEST;

    cartesiapp::Cartesia client(apiKey, apiVersion);

    // Retrieve and display API info
    cartesiapp::response::ApiInfo apiInfo = client.getApiInfo();
    spdlog::info("API Version: {}, Status OK: {}", apiInfo.version, apiInfo.ok);

    // Test STT with file
    if (!testSTTWithFile(client)) {
        return -1;
    }

    return 0;
}