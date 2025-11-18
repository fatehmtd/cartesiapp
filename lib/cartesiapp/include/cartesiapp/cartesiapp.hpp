#ifndef CARTESIA_APP_HPP
#define CARTESIA_APP_HPP

#include <string>
#include <memory>
#include <map>

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
        Cartesia(const std::string& apiKey,
            const std::string& apiVersion = request::api_versions::LATEST);
        ~Cartesia();

        /**
         * @brief Overrides the API version for subsequent requests.
         * @param apiVersion The API version to use.
         */
        void overrideApiVersion(const std::string& apiVersion);

        /**
         * @brief Retrieves the current API key being used.
         * @return The API key string.
         */
        std::string getApiKey() const;

        /**
         * @brief Retrieves the current API version being used.
         * @return The API version string.
         */
        std::string getApiVersion() const;

        /**
         * @brief Retrieves API information.
         */
        response::ApiInfo getApiInfo() const;

        /**
         * @brief Retrieves a list of available voices.
         * @param request The VoiceListRequest containing query parameters.
         * @return A VoiceListPage containing the list of voices.
         */
        response::VoiceListPage getVoiceList(const request::VoiceListRequest& request) const;

        /**
         * @brief Retrieves information about a specific voice by its ID.
         * @param voiceId The ID of the voice to retrieve.
         * @return A Voice object containing the voice information.
         */
        response::Voice getVoice(const std::string& voiceId) const;

        /**
         * @brief Performs a Text-to-Speech byte synthesis request.
         */
        std::string ttsBytes(const request::TTSBytesRequest& request) const;

        /**
         * @brief Performs a Speech-to-Text batch transcription using an audio file.
         * @param filePath The path to the audio file to transcribe.
         * @param request The BatchRequest containing transcription parameters.
         * @return A TranscriptionResponse containing the transcription result.
         */
        response::stt::TranscriptionResponse sttWithFile(const std::string& filePath,
            const request::stt::BatchRequest& request) const;

        /**
         * @brief Performs a Speech-to-Text batch transcription using raw audio bytes.
         * @param audioBytes The raw audio bytes to transcribe.
         * @param request The BatchRequest containing transcription parameters.
         * @return A TranscriptionResponse containing the transcription result.
         */
        response::stt::TranscriptionResponse sttWithBytes(const std::vector<char>& audioBytes,
            const request::stt::BatchRequest& request) const;

        private:
        std::unique_ptr<CartesiaClientImpl> _clientImpl;
        std::string _apiKey;
    };
}

#endif // CARTESIA_APP_HPP