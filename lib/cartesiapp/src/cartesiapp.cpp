#include "impl/cartesiapp_boost_impl.hpp"
#include "cartesiapp.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace cartesiapp;

cartesiapp::Cartesia::Cartesia(const std::string& apiKey, const std::string& apiVersion)
{
    _clientImpl = std::make_unique<CartesiaClientImpl>(apiKey, apiVersion, false);
}

cartesiapp::Cartesia::~Cartesia()
{

}

void cartesiapp::Cartesia::overrideApiVersion(const std::string& apiVersion)
{
    _clientImpl->overrideApiVersion(apiVersion);
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

response::stt::BatchResponse cartesiapp::Cartesia::sttWithFile(const std::string& filePath,
    const request::stt::BatchRequest& request) const
{
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::vector<char> audioBytes((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return sttWithBytes(std::move(audioBytes), request);
}

response::stt::BatchResponse cartesiapp::Cartesia::sttWithBytes(const std::vector<char>& audioBytes,
    const request::stt::BatchRequest& request) const
{
    return _clientImpl->sttWithBytes(audioBytes, request);
}

void cartesiapp::Cartesia::registerListener(std::weak_ptr<TTSResponseListener> listener)
{
    _listener = listener;
}

void cartesiapp::Cartesia::unregisterListener()
{
    _listener.reset();
}

bool cartesiapp::Cartesia::startTTSWebsocketConnection() const
{
    return _clientImpl->connectWebsocketAndStartThread(
        [this](const std::string& data) {
            auto listener = _listener.lock();
            if (listener) {
                nlohmann::json jsonData = nlohmann::json::parse(data);
                std::string responseType = jsonData.value("type", "");
                if (responseType == "chunk") {
                    cartesiapp::response::tts::AudioChunkResponse audioChunkResponse = std::move(cartesiapp::response::tts::AudioChunkResponse::fromJson(data));
                    listener->onAudioChunkReceived(std::move(audioChunkResponse));
                }
                else if (responseType == "timestamps") {
                    cartesiapp::response::tts::WordTimestampsResponse wordTimestampsResponse = std::move(cartesiapp::response::tts::WordTimestampsResponse::fromJson(data));
                    listener->onWordTimestampsReceived(std::move(wordTimestampsResponse));
                }
                else if (responseType == "phoneme_timestamps") {
                    cartesiapp::response::tts::PhonemeTimestampsResponse phonemeTimestampsResponse = cartesiapp::response::tts::PhonemeTimestampsResponse::fromJson(data);
                    listener->onPhonemeTimestampsReceived(std::move(phonemeTimestampsResponse));
                }
                else if (responseType == "flush_done") {
                    cartesiapp::response::tts::FlushDoneResponse flushDoneResponse = cartesiapp::response::tts::FlushDoneResponse::fromJson(data);
                    listener->onFlushDoneReceived(std::move(flushDoneResponse));
                }
                else if (responseType == "done") {
                    cartesiapp::response::tts::DoneResponse doneResponse = cartesiapp::response::tts::DoneResponse::fromJson(data);
                    listener->onDoneReceived(std::move(doneResponse));
                }
                else if (responseType == "error") {
                    cartesiapp::response::tts::ErrorResponse errorResponse = cartesiapp::response::tts::ErrorResponse::fromJson(data);
                    listener->onError(std::move(errorResponse));
                }
            }
        },
        [this]() {
            auto listener = _listener.lock();
            if (listener) {
                listener->onConnected();
            }
        },
        [this](const std::string& message) {
            auto listener = _listener.lock();
            if (listener) {
                listener->onDisconnected(message);
            }
        },
        [this](const std::string& errorMessage) {
            auto listener = _listener.lock();
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

