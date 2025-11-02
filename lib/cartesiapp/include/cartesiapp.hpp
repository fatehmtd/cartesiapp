#ifndef CARTESIA_APP_HPP
#define CARTESIA_APP_HPP

#include <string>
#include <memory>
#include <future>

#include "cartesiapp_export.hpp"

#include "cartesiapp_request.hpp"
#include "cartesiapp_response.hpp"

namespace cartesiapp {

    // Forward declaration of implementation class
    class CartesiaClientImpl;

    /**
     * Main class for interacting with the Cartesia API
     */
    class CARTESIAPP_EXPORT Cartesia {
        public:
        Cartesia(const std::string& apiKey, const std::string& apiVersion = request::api_versions::LATEST);
        ~Cartesia();

        /**
         * @brief Retrieves API information.
         */
        response::ApiInfo getApiInfo() const;

        /**
         * @brief Retrieves a list of available voices.
         * @param request The VoiceListRequest containing query parameters.
         * @return A VoiceListPage containing the list of voices.
         */
        response::VoiceListPage getVoiceList(request::VoiceListRequest& request) const;


        /**
         * @brief Retrieves information about a specific voice by its ID.
         * @param voiceId The ID of the voice to retrieve.
         * @return A Voice object containing the voice information.
         */
        response::Voice getVoice(const std::string& voiceId) const;

        /**
         * @brief Performs a Text-to-Speech byte synthesis request.
         */
        std::string ttsBytes(request::TTSBytesRequest& request) const;

        private:
        std::unique_ptr<CartesiaClientImpl> _clientImpl;
    };
}

#endif // CARTESIA_APP_HPP