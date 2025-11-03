#include <cartesiapp.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>


/**
 * @brief Test Speech-to-Text functionality with a local audio file.
 */
bool testSttWithFile(cartesiapp::Cartesia& client) {
    cartesiapp::request::STTBatchRequest sttRequest;
    // you can omit optional parameters to use defaults from the actual file's header
    //sttRequest.language = "en";
    //sttRequest.sample_rate = 16000;
    //sttRequest.encoding = cartesiapp::request::stt_encoding::PCM_S16LE;

    cartesiapp::response::stt::BatchResponse response = client.sttWithFile(
        "../data/tts_output.mp3",
         sttRequest
    );
    
    spdlog::info("STT With File Response:");
    spdlog::info("Type: {}", response.type);
    spdlog::info("Request ID: {}", response.request_id);
    spdlog::info("Transcribed Text: {}", response.text);
    spdlog::info("Language: {}", response.language);
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

    std::string apiKey = std::getenv("CARTESIA_API_KEY");
    std::string apiVersion = cartesiapp::request::api_versions::LATEST;

    cartesiapp::Cartesia client(apiKey, apiVersion);

    // Retrieve and display API info
    cartesiapp::response::ApiInfo apiInfo = client.getApiInfo();
    spdlog::info("API Version: {}, Status OK: {}", apiInfo.version, apiInfo.ok);

    // uncomment to test STT with file
    if (!testSttWithFile(client)) {
        return -1;
    }

    return 0;
}