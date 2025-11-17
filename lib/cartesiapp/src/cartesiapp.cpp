#include "impl/cartesiapp_boost_impl.hpp"
#include "cartesiapp.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace cartesiapp;

cartesiapp::Cartesia::Cartesia(const std::string& apiKey, const std::string& apiVersion): _apiKey(apiKey)
{
    _clientImpl = std::make_unique<CartesiaClientImpl>(apiKey, apiVersion, false);
}

cartesiapp::Cartesia::~Cartesia()
{
    // Stop and unregister STT listener
    unregisterSTTListener();    
    stopSTTWebsocketConnection();

    // Stop and unregister TTS listener
    unregisterTTSListener();
    stopTTSWebsocketConnection();
}

void cartesiapp::Cartesia::overrideApiVersion(const std::string& apiVersion)
{
    _clientImpl->overrideApiVersion(apiVersion);
}

std::string cartesiapp::Cartesia::getApiKey() const
{
    return _apiKey;
}

std::string cartesiapp::Cartesia::getApiVersion() const
{
    return _clientImpl->getApiVersion();
}

response::ApiInfo cartesiapp::Cartesia::getApiInfo() const
{
    return  _clientImpl->getApiInfo();
}

response::VoiceListPage cartesiapp::Cartesia::getVoiceList(const request::VoiceListRequest& request) const
{
    return _clientImpl->getVoiceList(request);
}

response::Voice cartesiapp::Cartesia::getVoice(const std::string& voiceId) const
{
    return _clientImpl->getVoice(voiceId);
}

std::string cartesiapp::Cartesia::ttsBytes(const request::TTSBytesRequest& request) const
{
    return _clientImpl->ttsBytes(request);
}

response::stt::TranscriptionResponse cartesiapp::Cartesia::sttWithFile(const std::string& filePath,
    const request::stt::BatchRequest& request) const
{
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::vector<char> audioBytes((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return sttWithBytes(std::move(audioBytes), request);
}

response::stt::TranscriptionResponse cartesiapp::Cartesia::sttWithBytes(const std::vector<char>& audioBytes,
    const request::stt::BatchRequest& request) const
{
    return _clientImpl->sttWithBytes(audioBytes, request);
}

bool cartesiapp::Cartesia::startSTTWebsocketConnection() const
{
    return false;
}

bool cartesiapp::Cartesia::stopSTTWebsocketConnection() const
{
    return false;
}

void cartesiapp::Cartesia::registerSTTListener(std::weak_ptr<STTResponseListener> listener)
{
    _sttListener = listener;
}

void cartesiapp::Cartesia::unregisterSTTListener()
{
    _sttListener.reset();
}

void cartesiapp::Cartesia::registerTTSListener(std::weak_ptr<TTSResponseListener> listener)
{
    _ttsListener = listener;
}

void cartesiapp::Cartesia::unregisterTTSListener()
{
    _ttsListener.reset();
}

bool cartesiapp::Cartesia::startTTSWebsocketConnection() const
{
    return _clientImpl->connectWebsocketAndStartThread(
        [this](const std::string& data) {
            auto listener = _ttsListener.lock();
            if (listener) {
                nlohmann::json jsonData = nlohmann::json::parse(data);
                std::string responseType = jsonData.value("type", "");
                if (responseType == tts_events::AUDIO_CHUNK) {
                    listener->onAudioChunkReceived(std::move(cartesiapp::response::tts::AudioChunkResponse::fromJson(data)));
                }
                else if (responseType == tts_events::WORD_TIMESTAMPS) {
                    listener->onWordTimestampsReceived(std::move(cartesiapp::response::tts::WordTimestampsResponse::fromJson(data)));
                }
                else if (responseType == tts_events::PHONEME_TIMESTAMPS) {
                    listener->onPhonemeTimestampsReceived(std::move(cartesiapp::response::tts::PhonemeTimestampsResponse::fromJson(data)));
                }
                else if (responseType == tts_events::FLUSH_DONE) {
                    listener->onFlushDoneReceived(std::move(cartesiapp::response::tts::FlushDoneResponse::fromJson(data)));
                }
                else if (responseType == tts_events::DONE) {
                    listener->onDoneReceived(std::move(cartesiapp::response::tts::DoneResponse::fromJson(data)));
                }
                else if (responseType == tts_events::ERROR_) {
                    listener->onError(std::move(cartesiapp::response::tts::ErrorResponse::fromJson(data)));
                }
            }
        },
        [this]() {
            auto listener = _ttsListener.lock();
            if (listener) {
                listener->onConnected();
            }
        },
        [this](const std::string& message) {
            auto listener = _ttsListener.lock();
            if (listener) {
                listener->onDisconnected(message);
            }
        },
        [this](const std::string& errorMessage) {
            auto listener = _ttsListener.lock();
            if (listener) {
                listener->onNetworkError(errorMessage);
            }
        });
}

bool cartesiapp::Cartesia::stopTTSWebsocketConnection() const
{
    return _clientImpl->disconnectWebsocket();
}

bool cartesiapp::Cartesia::requestTTS(const request::tts::GenerationRequest& request) const
{
    return _clientImpl->sendWebsocketData(request.toJson());
}

bool cartesiapp::Cartesia::cancelTTSContext(const request::tts::CancelContextRequest& request) const
{
    return _clientImpl->sendWebsocketData(request.toJson());
}

