#include <cartesiapp.hpp>
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
    std::atomic_bool& _stopFlag;
    std::fstream _audioOutputFile;

    public:
    TTSResponseListener(std::atomic_bool& stopFlag) : _stopFlag(stopFlag) {
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
    }

    void onNetworkError(const std::string& errorMessage) override {
        spdlog::error("Listener: Network error: {}", errorMessage);
    }

    void onAudioChunkReceived(const cartesiapp::response::tts::AudioChunkResponse& response) override {
        spdlog::info("Listener: Received audio chunk of size: {}, Context ID: {}, Step Time: {}, Done: {}",
            response.data.size(),
            response.context_id.has_value() ? *response.context_id : "N/A",
            response.step_time,
            response.done);
        if (_audioOutputFile.is_open()) {
            _audioOutputFile.write(response.data.data(), response.data.size());
        }
    }

    void onDoneReceived(const cartesiapp::response::tts::DoneResponse& response) override {
        spdlog::info("Listener: TTS done. Context ID: {}", response.context_id.has_value() ? *response.context_id : "N/A");
        _stopFlag.store(true);
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
};

/**
 * @brief Test Text-to-Speech functionality with streaming audio.
 */
bool testTTSWithStreaming(cartesiapp::Cartesia& client) {

    std::atomic_bool stopFlag = false;
    std::shared_ptr<TTSResponseListener> listener = std::make_shared<TTSResponseListener>(stopFlag);
    client.registerListener(listener);

    if (!client.startTTSWebsocketConnection()) {
        spdlog::error("Failed to start TTS WebSocket connection.");
        return false;
    }

    cartesiapp::request::VoiceListRequest voiceListRequest;
    voiceListRequest.gender = cartesiapp::request::voice_gender::FEMININE;
    auto voices = client.getVoiceList(voiceListRequest);

    cartesiapp::request::TTSGenerationRequest ttsRequest;
    // uuid for context ID can be generated as needed
    ttsRequest.context_id = generateSimpleID();
    ttsRequest.transcript = "Hello, this is a test of the Cartesia Text to Speech streaming API.";
    ttsRequest.voice.id = voices.voices[0].id;
    ttsRequest.output_format.container = cartesiapp::request::container::RAW;
    ttsRequest.output_format.encoding = cartesiapp::request::tts_encoding::PCM_F32LE;
    ttsRequest.output_format.sample_rate = cartesiapp::request::sample_rate::SR_44100;
    ttsRequest.generation_config.volume = 1.0f;
    ttsRequest.model_id = cartesiapp::request::tts_model::SONIC_3;
    ttsRequest.continue_ = false;

    if (!client.requestTTS(ttsRequest)) {
        spdlog::error("Failed to send TTS request.");
        return false;
    }

    while (!stopFlag.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    client.stopTTSWebsocketConnection();
    client.unregisterListener();

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

    // uncomment to test TTS with streaming
    if (!testTTSWithStreaming(client)) {
        return -1;
    }

    return 0;
}