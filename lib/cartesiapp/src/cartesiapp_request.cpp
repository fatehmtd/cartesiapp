#include "cartesiapp_request.hpp"
#include <nlohmann/json.hpp>
#include <sstream>

std::string cartesiapp::request::Voice::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["mode"] = mode;
    jsonObj["id"] = id;
    return jsonObj.dump();
}

std::string cartesiapp::request::OutputFormat::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["container"] = container;
    jsonObj["encoding"] = encoding;
    jsonObj["sample_rate"] = sample_rate;
    if (bit_rate) {
        jsonObj["bit_rate"] = bit_rate.value();
    }
    return jsonObj.dump();
}

std::string cartesiapp::request::TTSBytesRequest::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["model_id"] = model_id;
    jsonObj["transcript"] = transcript;
    jsonObj["voice"] = nlohmann::json::parse(voice.toJson());
    if (language) {
        jsonObj["language"] = language.value();
    }
    jsonObj["output_format"] = nlohmann::json::parse(output_format.toJson());
    if (duration) {
        jsonObj["duration"] = duration.value();
    }
    if (speed) {
        jsonObj["speed"] = speed.value();
    }
    if (generation_config) {
        jsonObj["generation_config"] = nlohmann::json::parse(generation_config->toJson());
    }
    if (pronunciation_dict_id) {
        jsonObj["pronunciation_dict_id"] = pronunciation_dict_id.value();
    }
    if (save) {
        jsonObj["save"] = save.value();
    }
    return jsonObj.dump();
}

std::string cartesiapp::request::VoiceListRequest::toQueryParams() const
{
    std::stringstream queryParams;
    bool firstParam = true;
    if (limit) {
        queryParams << (firstParam ? "?" : "&") << "limit=" << limit.value();
        firstParam = false;
    }
    if (start_after) {
        queryParams << (firstParam ? "?" : "&") << "start_after=" << start_after.value();
        firstParam = false;
    }
    if (end_before) {
        queryParams << (firstParam ? "?" : "&") << "end_before=" << end_before.value();
        firstParam = false;
    }
    if (is_owner) {
        queryParams << (firstParam ? "?" : "&") << "is_owner=" << is_owner.value();
        firstParam = false;
    }
    if (is_starred) {
        queryParams << (firstParam ? "?" : "&") << "is_starred=" << is_starred.value();
        firstParam = false;
    }

    queryParams << (firstParam ? "?" : "&") << "gender=" << gender;
    firstParam = false;

    if (expand && !expand->empty()) {
        queryParams << (firstParam ? "?" : "&") << "expand=";
        for (size_t i = 0; i < expand->size(); ++i) {
            queryParams << (*expand)[i];
            if (i < expand->size() - 1) {
                queryParams << ",";
            }
        }
    }

    return queryParams.str();
}

std::string cartesiapp::request::GenerationConfig::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["volume"] = volume;
    jsonObj["speed"] = speed;
    jsonObj["emotion"] = emotion;
    return jsonObj.dump();
}

std::string cartesiapp::request::stt::BatchRequest::toQueryParams() const
{
    std::stringstream queryParams;
    bool firstParam = true;
    if (encoding) {
        queryParams << (firstParam ? "?" : "&") << "encoding=" << encoding.value();
        firstParam = false;
    }
    if (sample_rate) {
        queryParams << (firstParam ? "?" : "&") << "sample_rate=" << sample_rate.value();
        firstParam = false;
    }
    return queryParams.str();
}

std::string cartesiapp::request::tts::GenerationRequest::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["model_id"] = model_id;
    jsonObj["transcript"] = transcript;
    jsonObj["voice"] = nlohmann::json::parse(voice.toJson());
    jsonObj["generation_config"] = nlohmann::json::parse(generation_config.toJson());
    jsonObj["output_format"] = nlohmann::json::parse(output_format.toJson());
    if (language) {
        jsonObj["language"] = language.value();
    }
    if (context_id) {
        jsonObj["context_id"] = context_id.value();
    }
    if (continue_) {
        jsonObj["continue"] = continue_.value();
    }
    if (max_buffer_delay_ms) {
        jsonObj["max_buffer_delay_ms"] = max_buffer_delay_ms.value();
    }
    if (flush) {
        jsonObj["flush"] = flush.value();
    }
    if (add_timestamps) {
        jsonObj["add_timestamps"] = add_timestamps.value();
    }
    if (add_phoneme_timestamps) {
        jsonObj["add_phoneme_timestamps"] = add_phoneme_timestamps.value();
    }
    if (use_normalized_timestamps) {
        jsonObj["use_normalized_timestamps"] = use_normalized_timestamps.value();
    }
    if (pronunciation_dict_id) {
        jsonObj["pronunciation_dict_id"] = pronunciation_dict_id.value();
    }    
    return jsonObj.dump();
}

std::string cartesiapp::request::tts::CancelContextRequest::toJson() const
{
    nlohmann::json jsonObj;
    jsonObj["context_id"] = context_id;
    jsonObj["cancel"] = cancel;
    return jsonObj.dump();
}
