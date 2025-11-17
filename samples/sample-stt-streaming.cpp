/**
 * @file sample-stt-streaming.cpp
 * @brief Sample demonstrating real-time Speech-to-Text streaming
 * 
 * This sample shows how to:
 * - Use WebSocket for real-time STT streaming
 * - Process audio files in chunks
 * - Handle partial and final transcription results
 * - Implement custom STT response listeners
 */

#include <streaming_stt.hpp>
#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <fstream>

/**
 * @brief Custom STT response listener that handles transcription events
 */
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
            response.request_id, response.text, response.is_final, response.language.has_value() ? response.language.value() : "N/A");

        spdlog::info("Duration: {} seconds", response.duration);
        spdlog::info("> Word Timings:");
        for (const auto& wordTiming : response.words) {
            spdlog::info("Word: '{}', Start: {} ms, End: {} ms",
                wordTiming.word, wordTiming.start, wordTiming.end);
        }

        if (response.is_final) {
            spdlog::info("Final transcription received, stopping...");
            //_stopFlag.store(true);
        }
    }

    void onDoneReceived(const cartesiapp::response::stt::DoneResponse& response) override {
        spdlog::info("STT Listener: STT done. Request ID: {}", response.request_id);
        _stopFlag.store(true);
    }

    void onFlushDoneReceived(const cartesiapp::response::stt::FlushDoneResponse& response) override {
        spdlog::info("STT Listener: STT flush done. Request ID: {}", response.request_id);
        //_stopFlag.store(true);
    }

    void onError(const cartesiapp::response::stt::ErrorResponse& response) override {
        spdlog::error("STT Listener: STT error. Request ID: {}, Message: {}",
            response.request_id, response.error);
        _stopFlag.store(true);
    }

    bool stopRequested() const {
        return _stopFlag.load();
    }

    private:
    std::atomic_bool _stopFlag;
};


/**
 * @brief Demonstrates STT streaming by processing an audio file in chunks
 */
bool testSTTWithStreaming(cartesiapp::Cartesia& client,
    const std::string& audioFilePath) {

    auto apiInfo = client.getApiInfo();

    std::shared_ptr<STTResponseListener> listener = std::make_shared<STTResponseListener>();

    cartesiapp::STTWebsocketClient sttClient(
        /* apiKey = */client.getApiKey(),
        /* model = */cartesiapp::request::stt_model::INK_WHISPER,
        /* language = */"en",
        /* encoding = */ cartesiapp::request::stt_encoding::PCM_S16LE,
        /* sampleRate = */cartesiapp::request::sample_rate::SR_16000,
        /* minVolume = */0.01f,
        /* apiVersion = */apiInfo.version);

    sttClient.registerSTTListener(listener);

    if (!sttClient.connectAndStart()) {
        spdlog::error("Failed to connect to STT WebSocket.");
        return false;
    }

    // Open audio file
    std::ifstream audioFile(audioFilePath, std::ios::binary);
    if (!audioFile) {
        spdlog::error("Failed to open audio file.");
        return false;
    }
    std::vector<char> buffer;
    audioFile.seekg(0, std::ios::end);
    std::streamsize fileSize = audioFile.tellg();
    const int headerSize = 44; // WAV header size
    if (fileSize <= headerSize) {
        spdlog::error("Audio file is too small to contain valid WAV data.");
        return false;
    }
    audioFile.seekg(headerSize, std::ios::beg);
    buffer.resize(fileSize - headerSize);
    if (!audioFile.read(buffer.data(), fileSize - headerSize)) {
        spdlog::error("Failed to read audio file.");
        return false;
    }
    audioFile.close();
    std::thread dataThread([&sttClient, &buffer]()
        {
            spdlog::info("Starting to send audio data...");
            // Stream audio data in chunks
            const size_t chunkSize = 3200; // 100ms of audio at 16kHz, 16-bit mono
            size_t offset = 0;
            while (offset < buffer.size()) {
                size_t bytesToSend = std::min(chunkSize, buffer.size() - offset);
                if (!sttClient.writeAudioBytes(buffer.data() + offset, bytesToSend)) {
                    spdlog::error("Failed to send audio bytes.");
                    return;
                }
                spdlog::debug("Sent audio chunk: offset: {}, num-bytes: {}", offset, bytesToSend);
                offset += bytesToSend;
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate real-time streaming
            }
            spdlog::info("Finished sending audio data.");
        });

    while (!listener->stopRequested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (dataThread.joinable()) {
        dataThread.join();
    }

    spdlog::info("Stopping STT WebSocket client...");
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

    std::string audioFilePath = "../data/sample_audio.wav";
    if (!testSTTWithStreaming(client, audioFilePath)) {
        return -1;
    }

    return 0;
}