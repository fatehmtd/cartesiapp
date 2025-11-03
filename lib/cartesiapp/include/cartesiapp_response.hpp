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
    }
}

#endif // CARTESIAPP_RESPONSE_HPP