#ifndef CARTESIAPP_RESPONSE_HPP
#define CARTESIAPP_RESPONSE_HPP

#include <string>
#include <vector>
#include <optional>

namespace cartesiapp::response {
    /**
     * Struct to hold API information
     */
    struct ApiInfo {
        std::string version;
        bool ok;
    };

    /**
     * Struct to hold Voice information
     */
    struct Voice {
        std::string id;
        bool is_owner;
        std::string name;
        std::string description;
        std::string created_at;
        std::optional<std::vector<int>> embedding;
        std::optional<bool> is_starred;
        std::string language;
    };
}

#endif // CARTESIAPP_RESPONSE_HPP