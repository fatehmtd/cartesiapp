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
}

#endif // CARTESIAPP_RESPONSE_HPP