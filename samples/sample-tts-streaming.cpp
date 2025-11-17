#include <streaming_tts.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>
#include <chrono>
#include <atomic>

std::string generateSimpleID() {
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return "context-id-" + std::to_string(timestamp);
}

class TTSResponseListener : public cartesiapp::TTSResponseListener {
    private:
    std::atomic_bool _stopFlag;
    std::fstream _audioOutputFile;
    bool _firstByteReceived = false;
    std::chrono::high_resolution_clock::time_point _firstByteTimestamp;

    public:
    TTSResponseListener() {
        _stopFlag.store(false);
        _audioOutputFile.open("tts_output.raw", std::ios::out | std::ios::binary);
        if (!_audioOutputFile.is_open()) {
            spdlog::error("Failed to open output audio file.");
        }
    }

    void onConnected() override {
        spdlog::info("Listener: WebSocket connected.");
    }

    void onDisconnected(const std::string& reason) override {
        spdlog::info("Listener: WebSocket disconnected. Reason: {}", reason);
        _stopFlag.store(true);
    }

    void onNetworkError(const std::string& errorMessage) override {
        spdlog::error("Listener: Network error: {}", errorMessage);
        _stopFlag.store(true);
    }

    void onAudioChunkReceived(const cartesiapp::response::tts::AudioChunkResponse& response) override {
        if (!_firstByteReceived) {
            _firstByteReceived = true;
            _firstByteTimestamp = std::chrono::high_resolution_clock::now();
        }
        // uncomment to log each audio chunk received
        /*spdlog::info("Listener: Received audio chunk of size: {}, Context ID: {}, Step Time: {}, Done: {}",
            response.data.size(),
            response.context_id.has_value() ? *response.context_id : "N/A",
            response.step_time,
            response.done);*/
        if (_audioOutputFile.is_open()) {
            _audioOutputFile.write(response.data.data(), response.data.size());
        }
    }

    void onDoneReceived(const cartesiapp::response::tts::DoneResponse& response) override {
        spdlog::info("Listener: TTS done. Context ID: {}", response.context_id.has_value() ? *response.context_id : "N/A");

        auto durationToFirstByte = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - firstByteTimestamp()).count();
        spdlog::info("total audio duration: {} ms", durationToFirstByte);

        //_stopFlag.store(true);
        _audioOutputFile.close();
    }

    void onWordTimestampsReceived(const cartesiapp::response::tts::WordTimestampsResponse& response) override {
        spdlog::info("Listener: Received word timestamps for context ID: {}", response.context_id.has_value() ? *response.context_id : "N/A");
    }

    void onPhonemeTimestampsReceived(const cartesiapp::response::tts::PhonemeTimestampsResponse& response) override {
        spdlog::info("Listener: Received phoneme timestamps for context ID: {}", response.context_id.has_value() ? *response.context_id : "N/A");
    }

    void onFlushDoneReceived(const cartesiapp::response::tts::FlushDoneResponse& response) override {
        spdlog::info("Listener: Flush done for context ID: {}", response.context_id.has_value() ? *response.context_id : "N/A");
    }

    void onError(const cartesiapp::response::tts::ErrorResponse& response) override {
        spdlog::error("Listener: TTS error: {} - {}", response.status_code, response.error);
        _stopFlag.store(true);
    }

    bool stopRequested() const {
        return _stopFlag.load();
    }

    std::chrono::high_resolution_clock::time_point firstByteTimestamp() const {
        return _firstByteTimestamp;
    }
};

/**
 * @brief Test Text-to-Speech functionality with streaming audio.
 */
bool testTTSWithStreaming(cartesiapp::Cartesia& client) {

    cartesiapp::request::VoiceListRequest voiceListRequest;
    voiceListRequest.gender = cartesiapp::request::voice_gender::FEMININE;
    auto voices = client.getVoiceList(voiceListRequest);

    cartesiapp::request::tts::GenerationRequest ttsRequest;
    // uuid for context ID can be generated as needed
    ttsRequest.context_id = generateSimpleID();
    ttsRequest.transcript = "Hello, this is a test of the Cartesia Text to Speech streaming API.";
    ttsRequest.voice.id = voices.voices[1].id;
    ttsRequest.output_format.container = cartesiapp::request::container::RAW;
    ttsRequest.output_format.encoding = cartesiapp::request::tts_encoding::PCM_S16LE;
    ttsRequest.output_format.sample_rate = cartesiapp::request::sample_rate::SR_48000;
    ttsRequest.generation_config.volume = 1.0f;
    ttsRequest.model_id = cartesiapp::request::tts_model::SONIC_3;
    ttsRequest.continue_ = false;    

    std::shared_ptr<TTSResponseListener> listener = std::make_shared<TTSResponseListener>();

    auto websocketClient = std::make_unique<cartesiapp::TTSWebsocketClient>(
        client.getApiKey(),
        client.getApiVersion());
    websocketClient->registerTTSListener(listener);

    if (!websocketClient->connectAndStart()) {
        spdlog::error("Failed to connect and start TTS WebSocket client.");
        return false;
    }

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    if (!websocketClient->requestTTS(ttsRequest)) {
        spdlog::error("Failed to send TTS request.");
        return false;
    }

    while (!listener->stopRequested()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // compute duration from start to first byte received
    auto firstByteTime = listener->firstByteTimestamp();
    auto durationToFirstByte = std::chrono::duration_cast<std::chrono::milliseconds>(firstByteTime - startTime).count();
    spdlog::info("Time from request start to first audio byte received: {} ms", durationToFirstByte);

    websocketClient->unregisterTTSListener();

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

    // uncomment to test TTS with streaming
    if (!testTTSWithStreaming(client)) {
        return -1;
    }

    return 0;
}