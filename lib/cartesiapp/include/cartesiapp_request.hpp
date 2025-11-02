#ifndef CARTESIAPP_REQUEST_HPP
#define CARTESIAPP_REQUEST_HPP

#include <string>
#include <vector>
#include <optional>

#include "cartesiapp_export.hpp"

namespace cartesiapp::request {

    /**
     * @brief Constants used across the Cartesia Library
     */
    namespace constants {
        constexpr const char* HOST = "api.cartesia.ai";
        constexpr const char* USER_AGENT = "CartesiaCPP/0.1.0";

        // headers
        constexpr const char* HEADER_API_KEY = "X-API-KEY";
        constexpr const char* HEADER_CARTESIA_VERSION = "Cartesia-Version";

        // endpoints
        constexpr const char* ENDPOINT_API_STATUS_INFO = "/";
        constexpr const char* ENDPOINT_VOICES = "/voices";
        constexpr const char* ENDPOINT_TTS_BYTES = "/tts/bytes";
        constexpr const char* ENDPOINT_TTS_SSE = "/tts/sse";
        constexpr const char* ENDPOINT_TTS_WEBSOCKET = "/tts/websocket";
        constexpr const char* ENDPOINT_STT = "/stt";
        constexpr const char* ENDPOINT_STT_WEBSOCKET = "/stt/websocket";
    }

    /**
     * @brief Supported API version identifiers
     */
    namespace api_versions {
        constexpr const char* VERSION_2024_06_10 = "2024-06-10";
        constexpr const char* VERSION_2024_11_13 = "2024-11-13";
        constexpr const char* VERSION_2025_04_16 = "2025-04-16";
        constexpr const char* LATEST = VERSION_2025_04_16;
    }

    /**
     * @brief Emotion levels for voice synthesis
     */
    namespace emotion {
        constexpr const char* NEUTRAL = "neutral";
        constexpr const char* HAPPY = "happy";
        constexpr const char* EXCITED = "excited";
        constexpr const char* ENTHUSIASTIC = "enthusiastic";
        constexpr const char* ELATED = "elated";
        constexpr const char* EUPHORIC = "euphoric";
        constexpr const char* TRIUMPHANT = "triumphant";
        constexpr const char* AMAZED = "amazed";
        constexpr const char* SURPRISED = "surprised";
        constexpr const char* FLIRTATIOUS = "flirtatious";
        constexpr const char* CURIOUS = "curious";
        constexpr const char* CONTENT = "content";
        constexpr const char* PEACEFUL = "peaceful";
        constexpr const char* SERENE = "serene";
        constexpr const char* CALM = "calm";
        constexpr const char* GRATEFUL = "grateful";
        constexpr const char* AFFECTIONATE = "affectionate";
        constexpr const char* TRUST = "trust";
        constexpr const char* SYMPATHETIC = "sympathetic";
        constexpr const char* ANTICIPATION = "anticipation";
        constexpr const char* MYSTERIOUS = "mysterious";
        constexpr const char* ANGRY = "angry";
        constexpr const char* MAD = "mad";
        constexpr const char* OUTRAGED = "outraged";
        constexpr const char* FRUSTRATED = "frustrated";
        constexpr const char* AGITATED = "agitated";
        constexpr const char* THREATENED = "threatened";
        constexpr const char* DISGUSTED = "disgusted";
        constexpr const char* CONTEMPT = "contempt";
        constexpr const char* ENVIOUS = "envious";
        constexpr const char* SARCASTIC = "sarcastic";
        constexpr const char* IRONIC = "ironic";
        constexpr const char* SAD = "sad";
        constexpr const char* DEJECTED = "dejected";
        constexpr const char* MELANCHOLIC = "melancholic";
        constexpr const char* DISAPPOINTED = "disappointed";
        constexpr const char* HURT = "hurt";
        constexpr const char* GUILTY = "guilty";
        constexpr const char* BORED = "bored";
        constexpr const char* TIRED = "tired";
        constexpr const char* REJECTED = "rejected";
        constexpr const char* NOSTALGIC = "nostalgic";
        constexpr const char* WISTFUL = "wistful";
        constexpr const char* APOLOGETIC = "apologetic";
        constexpr const char* HESITANT = "hesitant";
        constexpr const char* INSECURE = "insecure";
        constexpr const char* CONFUSED = "confused";
        constexpr const char* RESIGNED = "resigned";
        constexpr const char* ANXIOUS = "anxious";
        constexpr const char* PANICKED = "panicked";
        constexpr const char* ALARMED = "alarmed";
        constexpr const char* SCARED = "scared";
        constexpr const char* PROUD = "proud";
        constexpr const char* CONFIDENT = "confident";
        constexpr const char* DISTANT = "distant";
        constexpr const char* SKEPTICAL = "skeptical";
        constexpr const char* CONTEMPLATIVE = "contemplative";
        constexpr const char* DETERMINED = "determined";
    }

    /**
     * @brief Speed settings for voice synthesis
     */
    namespace speed {
        constexpr const char* SLOW = "slow";
        constexpr const char* NORMAL = "normal";
        constexpr const char* FAST = "fast";
    }

    /**
     * @brief Container formats for synthesized audio
     */
    namespace container {
        constexpr const char* RAW = "raw";
    }

    /**
     * @brief Encoding formats for synthesized audio
     */
    namespace tts_encoding {
        constexpr const char* PCM_F32LE = "pcm_f32le";
        constexpr const char* PCM_S16LE = "pcm_s16le";
        constexpr const char* PCM_MULAW = "pcm_mulaw";
        constexpr const char* PCM_ALAW = "pcm_alaw";
    }

    /**
     * @brief Encoding formats for speech-to-text audio
     */
    namespace stt_encoding {
        constexpr const char* PCM_S16LE = "pcm_s16le";
        constexpr const char* PCM_S32LE = "pcm_s32le";
        constexpr const char* PCM_F16LE = "pcm_f16le";
        constexpr const char* PCM_F32LE = "pcm_f32le";
        constexpr const char* PCM_MULAW = "pcm_mulaw";
        constexpr const char* PCM_ALAW = "pcm_alaw";
    }

    /**
     * @brief Sample rates for synthesized audio
     */
    namespace sample_rate {
        constexpr const int SR_8000 = 8000;
        constexpr const int SR_16000 = 16000;
        constexpr const int SR_22050 = 22050;
        constexpr const int SR_24000 = 24000;
        constexpr const int SR_44100 = 44100;
        constexpr const int SR_48000 = 48000;
    }

    /**
     * @brief Voice modes for TTS synthesis
     */
    namespace voice_mode {
        constexpr const char* ID = "id";
        constexpr const char* EMBEDDED = "embedded";
    }

    /**
     * @brief Voice specifications for TTS synthesis
     */
    struct Voice {
        std::string mode = voice_mode::ID;
        std::string id;

        std::string toJson() const;
    };

    /**
     * @brief Output format specifications for synthesized audio
     */
    struct OutputFormat {
        std::string container = container::RAW;
        std::string encoding = tts_encoding::PCM_S16LE;
        int sample_rate = sample_rate::SR_24000;
        std::optional<int> bit_rate;

        std::string toJson() const;
    };

    /**
     * @brief Supported TTS model identifiers
     */
    namespace tts_model {
        constexpr const char* SONIC_3 = "sonic-3";
        constexpr const char* SONIC_3_2025_10_27 = "sonic-3-2025-10-27";
        // older versions
        constexpr const char* SONIC_2 = "sonic-2";
    }

    /**
     * @brief Configuration for voice generation parameters
     */
    struct CARTESIAPP_EXPORT GenerationConfig {
        float volume = 1.0f;
        float speed = 1.0f;
        std::string emotion = emotion::NEUTRAL;

        std::string toJson() const;
    };

    /**
     * @brief Request structure for Text-to-Speech byte synthesis
     */
    struct CARTESIAPP_EXPORT TTSBytesRequest {
        std::string model_id = tts_model::SONIC_3;
        std::string transcript;
        Voice voice;
        std::optional<std::string> language;
        OutputFormat output_format;
        std::optional<int> duration;
        std::optional<std::string> speed = speed::NORMAL;
        std::optional<GenerationConfig> generation_config;
        std::optional<std::string> pronunciation_dict_id;
        std::optional<bool> save;

        std::string toJson() const;
    };

    /**
     * @brief Available voice genders for filtering
     */
    namespace voice_gender {
        constexpr const char* MASCULINE = "masculine";
        constexpr const char* FEMININE = "feminine";
        constexpr const char* GENDER_NEUTRAL = "gender_neutral";
    }

    /**
     * @brief Request structure for Voice List retrieval
     */
    struct CARTESIAPP_EXPORT VoiceListRequest {
        std::optional<int> limit;
        std::optional<std::string> start_after;
        std::optional<std::string> end_before;
        std::optional<bool> is_owner;
        std::optional<bool> is_starred;
        std::string gender = voice_gender::GENDER_NEUTRAL;
        std::optional<std::vector<std::string>> expand;

        std::string toQueryParams() const;
    };

    struct CARTESIAPP_EXPORT STTBatchRequest {
        // query
        std::optional<std::string> encoding = stt_encoding::PCM_S16LE;
        std::optional<int> sample_rate;

        // body
        std::optional<std::string> model;
    };
}

#endif // CARTESIAPP_REQUEST_HPP