#include "streaming_stt.hpp"
#include "impl/cartesiapp_boost_impl.hpp"
#include <sstream>

cartesiapp::STTWebsocketClient::STTWebsocketClient(const std::string& apiKey,
    const std::string& model,
    const std::string& language,
    const std::string& encoding,
    int sampleRate,
    float minVolume,
    const std::string& apiVersion) :
    _apiKey(apiKey),
    _model(model),
    _language(language),
    _encoding(encoding),
    _sampleRate(sampleRate),
    _minVolume(minVolume),
    _apiVersion(apiVersion)
{
    _websocketClientImpl = std::make_unique<WebsocketClientImpl>(
        /* apiKey = */apiKey,
        /* apiVersion = */apiVersion,
        /* verifyCertificates = */false,
        /* endpoint = */cartesiapp::request::constants::ENDPOINT_STT_WEBSOCKET);
}

cartesiapp::STTWebsocketClient::~STTWebsocketClient() {
}

bool cartesiapp::STTWebsocketClient::connectAndStart()
{
    if (_websocketClientImpl->isConnectedAndStarted()) {
        spdlog::warn("STTWebsocketClient::connectAndStart: WebSocket is already connected and started.");
        return false;
    }

    auto dataReadCallback = [this](const std::string& data) {
        nlohmann::json jsonData = nlohmann::json::parse(data, nullptr, false);
        std::string responseType = jsonData.value("type", "");
        if (responseType == "transcript") {
            response::stt::TranscriptionResponse transcriptionResponse = response::stt::TranscriptionResponse::fromJson(data);
            if (auto listener = _sttListener.lock())
            {
                listener->onTranscriptionReceived(transcriptionResponse);
            }
        }
        else if (responseType == "done") {
            response::stt::DoneResponse doneResponse = response::stt::DoneResponse::fromJson(data);
            if (auto listener = _sttListener.lock())
            {
                listener->onDoneReceived(doneResponse);
            }
        }
        else if (responseType == "flush_done") {
            response::stt::FlushDoneResponse flushDoneResponse = response::stt::FlushDoneResponse::fromJson(data);
            if (auto listener = _sttListener.lock())
            {
                listener->onFlushDoneReceived(flushDoneResponse);
            }
        }
        else if (responseType == "error") {
            response::stt::ErrorResponse errorResponse = response::stt::ErrorResponse::fromJson(data);
            if (auto listener = _sttListener.lock())
            {
                listener->onError(errorResponse);
            }
        }
        else {
            spdlog::warn("STTWebsocketClient: Unknown response type received: {}", responseType);
        };
        };

    auto onConnectedCallback = [this]() {
        if (auto listener = _sttListener.lock())
        {
            listener->onConnected();
        }
        };

    auto onDisconnectedCallback = [this](const std::string& reason) {
        if (auto listener = _sttListener.lock())
        {
            listener->onDisconnected(reason);
        }
        };

    auto onNetworkErrorCallback = [this](const std::string& errorMessage) {
        if (auto listener = _sttListener.lock())
        {
            listener->onNetworkError(errorMessage);
        }
        };

    std::map<std::string, std::string> headers;
    std::stringstream queryParamsStream;
    queryParamsStream << "?model=" << _model
        << "&language=" << _language
        << "&encoding=" << _encoding
        << "&sample_rate=" << _sampleRate
        << "&min_volume=" << _minVolume
        << "&api_key=" << _apiKey;

    std::string queryParams = queryParamsStream.str();

    return _websocketClientImpl->connectWebsocketAndStartThread(
        /* dataReadCallback = */ dataReadCallback,
        /* onConnectedCallback = */ onConnectedCallback,
        /* onDisconnectedCallback = */ onDisconnectedCallback,
        /* onNetworkErrorCallback = */ onNetworkErrorCallback,
        /* headers = */ headers,
        /* queryParams = */ queryParams
    );
}

void cartesiapp::STTWebsocketClient::disconnect()
{
    _websocketClientImpl->disconnectAndStop();
}

bool cartesiapp::STTWebsocketClient::isConnectedAndStarted() const
{
    return _websocketClientImpl->isConnectedAndStarted();
}

bool cartesiapp::STTWebsocketClient::sendDoneRequest() const
{
    return _websocketClientImpl->sendText("done");
}

bool cartesiapp::STTWebsocketClient::sendFinalizeRequest() const
{
    return _websocketClientImpl->sendText("finalize");
}

bool cartesiapp::STTWebsocketClient::writeAudioBytes(const char* data, size_t size) const
{
    return _websocketClientImpl->sendBytes(data, size);
}

void cartesiapp::STTWebsocketClient::registerSTTListener(std::weak_ptr<STTResponseListener> listener)
{
    _sttListener = listener;
}

void cartesiapp::STTWebsocketClient::unregisterSTTListener()
{
    _sttListener.reset();
}
