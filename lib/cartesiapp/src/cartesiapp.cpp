#include "impl/cartesiapp_boost_impl.hpp"
#include "cartesiapp.hpp"
#include <spdlog/spdlog.h>

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

response::VoiceListPage cartesiapp::Cartesia::getVoiceList(request::VoiceListRequest& request) const
{
    return _clientImpl->getVoiceList(request);
}

response::Voice cartesiapp::Cartesia::getVoice(const std::string& voiceId) const
{
    return _clientImpl->getVoice(voiceId);
}

std::string cartesiapp::Cartesia::ttsBytes(request::TTSBytesRequest& request) const
{
    return _clientImpl->ttsBytes(request);
}
