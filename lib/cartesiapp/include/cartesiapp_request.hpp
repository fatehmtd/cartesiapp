#ifndef CARTESIAPP_REQUEST_HPP
#define CARTESIAPP_REQUEST_HPP

#include <string>
#include <vector>
#include <optional>

namespace cartesiapp::request {

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
        // Anger levels
        constexpr const char* ANGER_LOWEST = "anger:lowest";
        constexpr const char* ANGER_LOW = "anger:low";
        constexpr const char* ANGER = "anger";
        constexpr const char* ANGER_HIGH = "anger:high";
        constexpr const char* ANGER_HIGHEST = "anger:highest";
        
        // Positivity levels
        constexpr const char* POSITIVITY_LOWEST = "positivity:lowest";
        constexpr const char* POSITIVITY_LOW = "positivity:low";
        constexpr const char* POSITIVITY = "positivity";
        constexpr const char* POSITIVITY_HIGH = "positivity:high";
        constexpr const char* POSITIVITY_HIGHEST = "positivity:highest";
        
        // Surprise levels
        constexpr const char* SURPRISE_LOWEST = "surprise:lowest";
        constexpr const char* SURPRISE_LOW = "surprise:low";
        constexpr const char* SURPRISE = "surprise";
        constexpr const char* SURPRISE_HIGH = "surprise:high";
        constexpr const char* SURPRISE_HIGHEST = "surprise:highest";
        
        // Sadness levels
        constexpr const char* SADNESS_LOWEST = "sadness:lowest";
        constexpr const char* SADNESS_LOW = "sadness:low";
        constexpr const char* SADNESS = "sadness";
        constexpr const char* SADNESS_HIGH = "sadness:high";
        constexpr const char* SADNESS_HIGHEST = "sadness:highest";
        
        // Curiosity levels
        constexpr const char* CURIOSITY_LOWEST = "curiosity:lowest";
        constexpr const char* CURIOSITY_LOW = "curiosity:low";
        constexpr const char* CURIOSITY = "curiosity";
        constexpr const char* CURIOSITY_HIGH = "curiosity:high";
        constexpr const char* CURIOSITY_HIGHEST = "curiosity:highest";
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
     * @brief Experimental controls for voice synthesis
     */
    struct ExperimentalControls {
        std::optional<int> speed;
        std::vector<std::string> emotion;
    };

    namespace container {
        constexpr const char* RAW = "raw";
    }

    /**
     * @brief Encoding formats for synthesized audio
     */
    namespace encoding {
        constexpr const char* PCM_F32LE = "pcm_f32le";
        constexpr const char* PCM_S16LE = "pcm_s16le";
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
        std::string mode;
        std::string id;
        std::optional<ExperimentalControls> __experimental_controls;
    };

    /**
     * @brief Output format specifications for synthesized audio
     */
    struct OutputFormat {
        std::string container = container::RAW;
        std::string encoding = encoding::PCM_S16LE;
        int sample_rate = sample_rate::SR_24000;
        std::optional<int> bit_rate;
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
     * @brief Request structure for Text-to-Speech byte synthesis
     */
    struct TTSBytesRequest {
        std::string model_id = tts_model::SONIC_3;
        std::string transcript;
        Voice voice;
        std::string language;
        OutputFormat output_format;
        std::optional<int> duration;
        std::optional<std::string> speed = speed::NORMAL;
    };
}

#endif // CARTESIAPP_REQUEST_HPP