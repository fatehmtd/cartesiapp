#ifndef CARTESIA_APP_HPP
#define CARTESIA_APP_HPP

#include <string>
#include <memory>
#include <future>

#include "cartesiapp_request.hpp"
#include "cartesiapp_response.hpp"

namespace cartesiapp {
    /**
     * Constants used across the Cartesia Library
     */
    namespace constants {
        constexpr const char* API_URL = "api.cartesi.io";
        constexpr const char* USER_AGENT = "CartesiaCPP/0.1.0";
        constexpr const char* API_KEY_HEADER = "X-API-KEY";
        constexpr const char* CARTESIA_VERSION_HEADER = "Cartesia-Version";
    }

    // Forward declaration of implementation class
    class CartesiaClientImpl;

    /**
     * Main class for interacting with the Cartesia API
     */
    class Cartesia {
        public:
        Cartesia(const std::string& apiKey);
        ~Cartesia();

        /**
         * @brief Retrieves API information asynchronously.
         */
        std::future<response::ApiInfo> getApiInfo() const;

        void ttsBytes(request::TTSBytesRequest& request);

        private:
        std::unique_ptr<CartesiaClientImpl> _clientImpl;
    };
}

#endif // CARTESIA_APP_HPP