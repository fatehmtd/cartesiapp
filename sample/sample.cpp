#include <cartesiapp.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>

int main(int ac, char** av) {

#ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    std::string apiKey = std::getenv("CARTESIA_API_KEY");
    std::string apiVersion = cartesiapp::request::api_versions::LATEST;

    cartesiapp::Cartesia client(apiKey, apiVersion);
    try {
        cartesiapp::response::ApiInfo apiInfo = client.getApiInfo();
        spdlog::info("API Version: {}, Status OK: {}", apiInfo.version, apiInfo.ok);

        cartesiapp::request::VoiceListRequest voiceListRequest;
        voiceListRequest.gender = cartesiapp::request::voice_gender::MASCULINE;
        cartesiapp::response::VoiceListPage voiceListResponse = client.getVoiceList(voiceListRequest);

        spdlog::info("Retrieved {} voices. Has more: {}", voiceListResponse.voices.size(), voiceListResponse.has_more);

        // Fetch and display details for each voice
        for (const auto& voice : voiceListResponse.voices) {
            auto detailedVoice = client.getVoice(voice.id);
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
        ttsRequest.output_format.sample_rate = cartesiapp::request::sample_rate::SR_16000;
        ttsRequest.output_format.encoding = cartesiapp::request::tts_encoding::PCM_S16LE;
        ttsRequest.output_format.container = cartesiapp::request::container::RAW;

        ttsRequest.voice.id = voiceListResponse.voices[0].id; // Use the first voice from the list

        ttsRequest.transcript = "Hey Siri, tell me how beautiful I am today :)";

        cartesiapp::request::GenerationConfig generationConfig;
        generationConfig.emotion = cartesiapp::request::emotion::HAPPY;
        generationConfig.speed = 1.0f;
        generationConfig.volume = 1.0f;
        ttsRequest.generation_config = generationConfig;

        std::string response = client.ttsBytes(ttsRequest);

        std::ofstream outFile("tts_output.raw", std::ios::binary);
        outFile.write(response.data(), response.size());
    }
    catch (const std::exception& e) {
        std::cerr << "Error retrieving API info: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}