#include "cartesiapp_response.hpp"
#include <nlohmann/json.hpp>
#include "cartesiapp_response.hpp"
#include <base64.hpp>

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
    voice.id = std::move(jsonObj["id"].get<std::string>());
    voice.name = std::move(jsonObj["name"].get<std::string>());
    voice.is_owner = jsonObj["is_owner"].get<bool>();
    voice.is_public = jsonObj["is_public"].get<bool>();
    voice.gender = std::move(jsonObj["gender"].get<std::string>());
    voice.description = std::move(jsonObj["description"].get<std::string>());
    voice.created_at = std::move(jsonObj["created_at"].get<std::string>());
    if (jsonObj.contains("embedding") && !jsonObj["embedding"].is_null()) {
        voice.embedding = std::move(jsonObj["embedding"].get<std::vector<int>>());
    }
    if (jsonObj.contains("is_starred") && !jsonObj["is_starred"].is_null()) {
        voice.is_starred = jsonObj["is_starred"].get<bool>();
    }
    voice.language = std::move(jsonObj["language"].get<std::string>());
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

cartesiapp::response::stt::WordTiming cartesiapp::response::stt::WordTiming::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    WordTiming wordTiming;
    wordTiming.word = std::move(jsonObj["word"].get<std::string>());
    wordTiming.start = jsonObj["start"].get<float>();
    wordTiming.end = jsonObj["end"].get<float>();
    return wordTiming;
}

cartesiapp::response::stt::TranscriptionResponse cartesiapp::response::stt::TranscriptionResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    TranscriptionResponse sttResponse;
    sttResponse.type = std::move(jsonObj["type"].get<std::string>());
    sttResponse.text = std::move(jsonObj["text"].get<std::string>());
    if (jsonObj.contains("language") && !jsonObj["language"].is_null()) {
        sttResponse.language = std::move(jsonObj["language"].get<std::string>());
    }
    sttResponse.duration = jsonObj["duration"].get<float>();
    sttResponse.is_final = jsonObj["is_final"].get<bool>();
    sttResponse.request_id = std::move(jsonObj["request_id"].get<std::string>());

    for (const auto& wordJson : jsonObj["words"]) {
        sttResponse.words.push_back(WordTiming::fromJson(wordJson.dump()));
    }
    return sttResponse;
}

cartesiapp::response::tts::AudioChunkResponse cartesiapp::response::tts::AudioChunkResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    AudioChunkResponse audioChunkResponse;
    audioChunkResponse.type = std::move(jsonObj["type"].get<std::string>());
    audioChunkResponse.data = std::move(base64::from_base64(jsonObj["data"].get<std::string>()));
    audioChunkResponse.done = jsonObj["done"].get<bool>();
    audioChunkResponse.status_code = jsonObj["status_code"].get<int>();
    audioChunkResponse.step_time = jsonObj["step_time"].get<double>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        audioChunkResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }
    return audioChunkResponse;
}

cartesiapp::response::tts::FlushDoneResponse cartesiapp::response::tts::FlushDoneResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    FlushDoneResponse flushDoneResponse;
    flushDoneResponse.type = std::move(jsonObj["type"].get<std::string>());
    flushDoneResponse.done = jsonObj["done"].get<bool>();
    flushDoneResponse.flush_done = jsonObj["flush_done"].get<bool>();
    flushDoneResponse.flush_id = jsonObj["flush_id"].get<int>();
    flushDoneResponse.status_code = jsonObj["status_code"].get<int>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        flushDoneResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }
    return flushDoneResponse;
}

cartesiapp::response::tts::DoneResponse cartesiapp::response::tts::DoneResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    DoneResponse doneResponse;
    doneResponse.type = std::move(jsonObj["type"].get<std::string>());
    doneResponse.done = jsonObj["done"].get<bool>();
    doneResponse.status_code = jsonObj["status_code"].get<int>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        doneResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }
    return doneResponse;
}

cartesiapp::response::tts::WordTimestampsResponse::WordTimestamps cartesiapp::response::tts::WordTimestampsResponse::WordTimestamps::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    WordTimestamps wordTimestamps;
    wordTimestamps.words = std::move(jsonObj["words"].get<std::vector<std::string>>());
    wordTimestamps.start = jsonObj["start"].get<std::vector<double>>();
    wordTimestamps.end = jsonObj["end"].get<std::vector<double>>();
    return wordTimestamps;
}

cartesiapp::response::tts::WordTimestampsResponse cartesiapp::response::tts::WordTimestampsResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    WordTimestampsResponse wordTimestampsResponse;
    wordTimestampsResponse.type = std::move(jsonObj["type"].get<std::string>());
    wordTimestampsResponse.done = jsonObj["done"].get<bool>();
    wordTimestampsResponse.status_code = jsonObj["status_code"].get<int>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        wordTimestampsResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }

    // Based on header definition, word_timestamps is a vector, so parse as array
    for (const auto& timestampJson : jsonObj["word_timestamps"]) {
        wordTimestampsResponse.word_timestamps.push_back(WordTimestamps::fromJson(timestampJson.dump()));
    }
    return wordTimestampsResponse;
}

cartesiapp::response::tts::PhonemeTimestampsResponse::PhonemeTimestamps cartesiapp::response::tts::PhonemeTimestampsResponse::PhonemeTimestamps::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    PhonemeTimestamps phonemeTimestamps;
    phonemeTimestamps.phonemes = std::move(jsonObj["phonemes"].get<std::vector<std::string>>());
    phonemeTimestamps.start = jsonObj["start"].get<std::vector<double>>();
    phonemeTimestamps.end = jsonObj["end"].get<std::vector<double>>();
    return phonemeTimestamps;
}

cartesiapp::response::tts::PhonemeTimestampsResponse cartesiapp::response::tts::PhonemeTimestampsResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    PhonemeTimestampsResponse phonemeTimestampsResponse;
    phonemeTimestampsResponse.type = std::move(jsonObj["type"].get<std::string>());
    phonemeTimestampsResponse.done = jsonObj["done"].get<bool>();
    phonemeTimestampsResponse.status_code = jsonObj["status_code"].get<int>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        phonemeTimestampsResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }

    // Based on header definition, phoneme_timestamps is a single object
    phonemeTimestampsResponse.phoneme_timestamps = PhonemeTimestamps::fromJson(jsonObj["phoneme_timestamps"].dump());
    return phonemeTimestampsResponse;
}

cartesiapp::response::tts::ErrorResponse cartesiapp::response::tts::ErrorResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    ErrorResponse errorResponse;
    errorResponse.type = std::move(jsonObj["type"].get<std::string>());
    errorResponse.done = jsonObj["done"].get<bool>();
    errorResponse.error = std::move(jsonObj["error"].get<std::string>());
    errorResponse.status_code = jsonObj["status_code"].get<int>();
    if (jsonObj.contains("context_id") && !jsonObj["context_id"].is_null()) {
        errorResponse.context_id = std::move(jsonObj["context_id"].get<std::string>());
    }
    return errorResponse;
}

cartesiapp::response::stt::FlushDoneResponse cartesiapp::response::stt::FlushDoneResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    FlushDoneResponse flushDoneResponse;
    flushDoneResponse.type = std::move(jsonObj["type"].get<std::string>());
    flushDoneResponse.request_id = std::move(jsonObj["request_id"].get<std::string>());
    return flushDoneResponse;
}

cartesiapp::response::stt::DoneResponse cartesiapp::response::stt::DoneResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    DoneResponse doneResponse;
    doneResponse.type = std::move(jsonObj["type"].get<std::string>());
    doneResponse.request_id = std::move(jsonObj["request_id"].get<std::string>());
    return doneResponse;
}

cartesiapp::response::stt::ErrorResponse cartesiapp::response::stt::ErrorResponse::fromJson(const std::string& jsonStr)
{
    nlohmann::json jsonObj = nlohmann::json::parse(jsonStr);
    ErrorResponse errorResponse;
    errorResponse.type = std::move(jsonObj["type"].get<std::string>());
    errorResponse.error = std::move(jsonObj["error"].get<std::string>());
    errorResponse.request_id = std::move(jsonObj["request_id"].get<std::string>());
    return errorResponse;
}