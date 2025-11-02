#include "cartesiapp_response.hpp"
#include <nlohmann/json.hpp>
#include "cartesiapp_response.hpp"

using namespace cartesiapp::response;

ApiInfo cartesiapp::response::ApiInfo::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    ApiInfo apiInfo;
    apiInfo.version = jsonObj["version"].get<std::string>();
    apiInfo.ok = jsonObj["ok"].get<bool>();
    return apiInfo;
}

Voice cartesiapp::response::Voice::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    Voice voice;
    voice.id = jsonObj["id"].get<std::string>();
    voice.name = jsonObj["name"].get<std::string>();
    voice.is_owner = jsonObj["is_owner"].get<bool>();
    voice.is_public = jsonObj["is_public"].get<bool>();
    voice.gender = jsonObj["gender"].get<std::string>();
    voice.description = jsonObj["description"].get<std::string>();
    voice.created_at = jsonObj["created_at"].get<std::string>();
    if (jsonObj.contains("embedding") && !jsonObj["embedding"].is_null()) {
        voice.embedding = jsonObj["embedding"].get<std::vector<int>>();
    }
    if( jsonObj.contains("is_starred") && !jsonObj["is_starred"].is_null()) {
        voice.is_starred = jsonObj["is_starred"].get<bool>();
    }
    voice.language = jsonObj["language"].get<std::string>();
    return voice;
}

VoiceListPage cartesiapp::response::VoiceListPage::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    VoiceListPage voiceListPage;
    voiceListPage.has_more = jsonObj["has_more"].get<bool>();
    for (const auto& voiceJson : jsonObj["data"]) {
        voiceListPage.voices.push_back(Voice::fromJson(voiceJson.dump()));
    }
    return voiceListPage;
}

cartesiapp::response::SttBatchResponse::WordTiming cartesiapp::response::SttBatchResponse::WordTiming::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    WordTiming wordTiming;
    wordTiming.word = jsonObj["word"].get<std::string>();
    wordTiming.start = jsonObj["start"].get<float>();
    wordTiming.end = jsonObj["end"].get<float>();
    return wordTiming;
}

cartesiapp::response::SttBatchResponse cartesiapp::response::SttBatchResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    SttBatchResponse sttResponse;
    sttResponse.type = jsonObj["type"].get<std::string>();
    sttResponse.text = jsonObj["text"].get<std::string>();
    sttResponse.language = jsonObj["language"].get<std::string>();
    sttResponse.duration = jsonObj["duration"].get<float>();
    sttResponse.is_final = jsonObj["is_final"].get<bool>();
    sttResponse.request_id = jsonObj["request_id"].get<std::string>();
    
    for (const auto& wordJson : jsonObj["words"]) {
        sttResponse.words.push_back(SttBatchResponse::WordTiming::fromJson(wordJson.dump()));
    }
    return sttResponse;
}
