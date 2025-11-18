/**
 * @file sample-tts-bytes.cpp
 * @brief Sample demonstrating Text-to-Speech byte synthesis
 * 
 * This sample shows how to:
 * - List and filter available voices
 * - Configure TTS request with emotions and output formats
 * - Generate audio as bytes and save to file
 */

#include <cartesiapp/cartesiapp.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>
#include <filesystem>

/**
 * @brief Demonstrates TTS byte synthesis with voice selection and audio generation
 */
bool testBytesTTS(cartesiapp::Cartesia& client) {
    try {
        // prepare voice list request
        cartesiapp::request::VoiceListRequest voiceListRequest;
        voiceListRequest.gender = cartesiapp::request::voice_gender::MASCULINE;

        // fetch voice list
        cartesiapp::response::VoiceListPage voiceListResponse = client.getVoiceList(voiceListRequest);

        spdlog::info("Retrieved {} voices. Has more: {}", voiceListResponse.voices.size(), voiceListResponse.has_more);

        // Fetch and display details for each voice
        for (const auto& voice : voiceListResponse.voices) {
            // Fetch detailed voice information
            cartesiapp::response::Voice detailedVoice = client.getVoice(voice.id);
            spdlog::info("Voice ID: {}", detailedVoice.id);
            spdlog::info("Name: {}", detailedVoice.name);
            spdlog::info("Language: {}", detailedVoice.language);
            spdlog::info("Gender: {}", detailedVoice.gender);
            spdlog::info("Is starred: {}", detailedVoice.is_starred ? (detailedVoice.is_starred.value() ? "true" : "false") : "N/A");
            spdlog::info("Is owner: {}", detailedVoice.is_owner);
            spdlog::info("Is public: {}", detailedVoice.is_public);
            spdlog::info("Description: {}", detailedVoice.description);
            spdlog::info("-----");
        }

        cartesiapp::request::TTSBytesRequest ttsRequest;
        ttsRequest.output_format.sample_rate = cartesiapp::request::sample_rate::SR_44100;
        ttsRequest.output_format.encoding = cartesiapp::request::tts_encoding::PCM_S16LE;
        ttsRequest.output_format.container = cartesiapp::request::container::WAV;

        ttsRequest.voice.id = voiceListResponse.voices[0].id; // Use the first voice from the list

        ttsRequest.transcript = "Hello world, this is a test of the Cartesia Text to Speech byte synthesis feature.";

        cartesiapp::request::GenerationConfig generationConfig;
        generationConfig.emotion = cartesiapp::request::emotion::HAPPY;
        generationConfig.speed = 1.0f;
        generationConfig.volume = 1.0f;
        ttsRequest.generation_config = generationConfig;

        std::string response = client.ttsBytes(ttsRequest);

        std::string output_path = "tts_output." + ttsRequest.output_format.container;
        // print absolute path with filesystem
        std::filesystem::path absPath = std::filesystem::absolute(output_path);
        spdlog::info("Received TTS byte synthesis response of size: {} bytes to {}", response.size(), absPath.string());
        std::ofstream outFile(absPath, std::ios::binary);
        outFile.write(response.data(), response.size());
    }
    catch (const std::exception& e) {
        std::cerr << "Error retrieving API info: " << e.what() << std::endl;
        return false;
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

    // Test TTS byte synthesis
    if (!testBytesTTS(client)) {
        return -1;
    }

    return 0;
}