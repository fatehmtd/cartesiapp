#ifndef CARTESIAPP_RESPONSE_HPP
#define CARTESIAPP_RESPONSE_HPP

#include <string>
#include <vector>
#include <optional>

#include "cartesiapp_export.hpp"

namespace cartesiapp::response {
    /**
     * Struct to hold API information
     */
    struct CARTESIAPP_EXPORT ApiInfo {
        std::string version;
        bool ok;

        static ApiInfo fromJson(const std::string& jsonStr);
    };

    /**
     * Struct to hold Voice information
     */
    struct CARTESIAPP_EXPORT Voice {
        std::string id;
        bool is_owner;
        bool is_public;
        std::string name;
        std::string description;
        std::string gender;
        std::string created_at;
        std::optional<std::vector<int>> embedding;
        std::optional<bool> is_starred;
        std::string language;

        static Voice fromJson(const std::string& jsonStr);
    };

    /**
     * Struct to hold a page of Voice List results
     */
    struct CARTESIAPP_EXPORT VoiceListPage {
        std::vector<Voice> voices;
        bool has_more;

        static VoiceListPage fromJson(const std::string& jsonStr);
    };

    /**
     * @brief Namespace for Speech-to-Text related responses
     */
    namespace stt {
        /**
         * Struct to hold STT batch response information
         */
        struct CARTESIAPP_EXPORT BatchResponse {
            std::string type;
            std::string text;
            std::string language;
            float duration;
            std::string request_id;
            bool is_final;

            /**
             * Struct to hold word timing information for STT
             */
            struct WordTiming {
                std::string word;
                float start;
                float end;

                static WordTiming fromJson(const std::string& jsonStr);
            };
            std::vector<WordTiming> words;

            static BatchResponse fromJson(const std::string& jsonStr);
        };
    }

    /**
     * @brief Namespace for Text-to-Speech related responses
     */
    namespace tts {

        /**
         * Struct to hold TTS audio chunk response information
         */
        struct CARTESIAPP_EXPORT AudioChunkResponse {
            std::string type;
            std::string data;
            bool done;
            int status_code;
            double step_time;
            std::optional<std::string> context_id;

            static AudioChunkResponse fromJson(const std::string& jsonStr);
        };

        /**
         * Struct to hold flush done response information
         */
        struct CARTESIAPP_EXPORT FlushDoneResponse {
            std::string type;
            bool done;
            bool flush_done;
            int flush_id;
            int status_code;
            std::optional<std::string> context_id;

            static FlushDoneResponse fromJson(const std::string& jsonStr);
        };

        /**
         * Struct to hold done response information
         */
        struct CARTESIAPP_EXPORT DoneResponse {
            std::string type;
            bool done;
            int status_code;
            std::optional<std::string> context_id;

            static DoneResponse fromJson(const std::string& jsonStr);
        };

        /**
         * Struct to hold word timestamps response information
         */
        struct CARTESIAPP_EXPORT WordTimestampsResponse {
            std::string type;
            bool done;
            int status_code;
            std::optional<std::string> context_id;

            /**
             * Struct to hold word-level timing information
             */
            struct WordTimestamps {
                std::vector<std::string> words;
                std::vector<double> start;
                std::vector<double> end;

                static WordTimestamps fromJson(const std::string& jsonStr);
            };
            std::vector<WordTimestamps> word_timestamps;

            static WordTimestampsResponse fromJson(const std::string& jsonStr);
        };

        /**
         * Struct to hold phoneme timestamps response information
         */
        struct CARTESIAPP_EXPORT PhonemeTimestampsResponse {
            std::string type;
            bool done;
            int status_code;
            std::optional<std::string> context_id;

            /**
             * Struct to hold phoneme-level timing information
             */
            struct PhonemeTimestamps {
                std::vector<std::string> phonemes;
                std::vector<double> start;
                std::vector<double> end;

                static PhonemeTimestamps fromJson(const std::string& jsonStr);
            };
            PhonemeTimestamps phoneme_timestamps;

            static PhonemeTimestampsResponse fromJson(const std::string& jsonStr);
        };        

        /**
         * Struct to hold error response information
         */
        struct CARTESIAPP_EXPORT ErrorResponse {
            std::string type;
            bool done;
            std::string error;
            int status_code;
            std::optional<std::string> context_id;

            static ErrorResponse fromJson(const std::string& jsonStr);
        };
    }
}

#endif // CARTESIAPP_RESPONSE_HPP