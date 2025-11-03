#include "impl/cartesiapp_boost_impl.hpp"
#include "cartesiapp.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>

using namespace cartesiapp;

cartesiapp::Cartesia::Cartesia(const std::string& apiKey, const std::string& apiVersion)
{
    _clientImpl = std::make_unique<CartesiaClientImpl>(apiKey, apiVersion, false);
}

cartesiapp::Cartesia::~Cartesia()
{

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
    const request::STTBatchRequest& request) const
{
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::vector<char> audioBytes((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    return sttWithBytes(std::move(audioBytes), request);
}

response::stt::BatchResponse cartesiapp::Cartesia::sttWithBytes(const std::vector<char>& audioBytes,
    const request::STTBatchRequest& request) const
{
    return _clientImpl->sttWithBytes(audioBytes, request);
}
