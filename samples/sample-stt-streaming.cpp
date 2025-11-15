#include <stt_ws.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>

class STTResponseListener :
    public cartesiapp::STTResponseListener,
    public std::enable_shared_from_this<STTResponseListener> {

    public:

    void onConnected() override {
        spdlog::info("STT Listener: WebSocket connected.");
    }

    void onDisconnected(const std::string& reason) override {
        spdlog::info("STT Listener: WebSocket disconnected. Reason: {}", reason);
        _stopFlag.store(true);
    }

    void onNetworkError(const std::string& errorMessage) override {
        spdlog::error("STT Listener: Network error: {}", errorMessage);
        _stopFlag.store(true);
    }

    void onTranscriptionReceived(const cartesiapp::response::stt::TranscriptionResponse& response) override {
        spdlog::info("STT Listener: Received transcription response. Request ID: {}, Text: {}, Is Final: {}, Language: {}",
            response.request_id, response.text, response.is_final, response.language);

        spdlog::info("Duration: {} seconds", response.duration);
        spdlog::info("> Word Timings:");
        for (const auto& wordTiming : response.words) {
            spdlog::info("Word: '{}', Start: {} ms, End: {} ms",
                wordTiming.word, wordTiming.start, wordTiming.end);
        }
    }

    void onDoneReceived(const cartesiapp::response::stt::DoneResponse& response) override {
        spdlog::info("STT Listener: STT done. Request ID: {}", response.request_id);
        _stopFlag.store(true);
    }

    void onFlushDoneReceived(const cartesiapp::response::stt::FlushDoneResponse& response) override {
        spdlog::info("STT Listener: STT flush done. Request ID: {}", response.request_id);
        _stopFlag.store(true);
    }

    bool stopRequested() const {
        return _stopFlag.load();
    }

    private:
    std::atomic_bool _stopFlag;
};


/**
 * @brief Test Speech-to-Text functionality with a local audio file.
 */
bool testTTSWithStreaming(cartesiapp::Cartesia& client) {
    cartesiapp::request::stt::BatchRequest sttRequest;
    // you can omit optional parameters to use defaults from the actual file's header
    //sttRequest.language = "en";
    //sttRequest.sample_rate = 16000;
    //sttRequest.encoding = cartesiapp::request::stt_encoding::PCM_S16LE;

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

    // uncomment to test STT with file
    if (!testTTSWithStreaming(client)) {
        return -1;
    }

    return 0;
}