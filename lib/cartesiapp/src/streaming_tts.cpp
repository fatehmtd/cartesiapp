#include "streaming_tts.hpp"
#include <nlohmann/json.hpp>
#include "impl/cartesiapp_boost_impl.hpp"


cartesiapp::TTSWebsocketClient::TTSWebsocketClient(const std::string& apiKey, const std::string& apiVersion) : _apiKey(apiKey), _apiVersion(apiVersion)
{
    _websocketClientImpl = std::make_unique<WebsocketClientImpl>(
        /* apiKey = */apiKey,
        /* apiVersion = */apiVersion,
        /* verifyCertificates = */false,
        /* endpoint = */cartesiapp::request::constants::ENDPOINT_TTS_WEBSOCKET);
}

cartesiapp::TTSWebsocketClient::~TTSWebsocketClient()
{
    
}

bool cartesiapp::TTSWebsocketClient::connectAndStart()
{
    if (_websocketClientImpl->isConnectedAndStarted()) {
        spdlog::warn("TTSWebsocketClient::connectAndStart: WebSocket is already connected and started.");
        return false;
    }

    auto dataReceptionCallback = [this](const std::string& data) {
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
        };

    auto connectionEstablishedCallback = [this]() {
        auto listener = _ttsListener.lock();
        if (listener) {
            listener->onConnected();
        }
        };

    auto disconnectionCallback = [this](const std::string& message) {
        auto listener = _ttsListener.lock();
        if (listener) {
            listener->onDisconnected(message);
        }
        };

    auto networkErrorCallback = [this](const std::string& errorMessage) {
        auto listener = _ttsListener.lock();
        if (listener) {
            listener->onNetworkError(errorMessage);
        }
        };

    std::map<std::string, std::string> headers;

    std::stringstream queryParamsStream;
    queryParamsStream << "?api_key=" << _apiKey
        << "&api_version=" << _apiVersion;

    std::string queryParams = queryParamsStream.str();

    return _websocketClientImpl->connectWebsocketAndStartThread(
        /* dataReadCallback = */ dataReceptionCallback,
        /* onConnectedCallback = */ connectionEstablishedCallback,
        /* onDisconnectedCallback = */ disconnectionCallback,
        /* onNetworkErrorCallback = */ networkErrorCallback,
        /* headers = */ headers,
        /* queryParams = */ queryParams
    );
}

bool cartesiapp::TTSWebsocketClient::isConnectedAndStarted() const
{
    return _websocketClientImpl->isConnectedAndStarted();
}

bool cartesiapp::TTSWebsocketClient::requestTTS(const request::tts::GenerationRequest& request) const
{
    return _websocketClientImpl->sendText(request.toJson());
}

bool cartesiapp::TTSWebsocketClient::cancelTTSContext(const request::tts::CancelContextRequest& request) const
{
    return _websocketClientImpl->sendText(request.toJson());
}

void cartesiapp::TTSWebsocketClient::registerTTSListener(std::weak_ptr<TTSResponseListener> listener)
{
    _ttsListener = listener;
}

void cartesiapp::TTSWebsocketClient::unregisterTTSListener()
{
    _ttsListener.reset();
}
