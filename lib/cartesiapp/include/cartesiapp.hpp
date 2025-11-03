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
         * @param request The STTBatchRequest containing transcription parameters.
         * @return A BatchResponse containing the transcription result.
         */
        response::stt::BatchResponse sttWithFile(const std::string& filePath, const request::STTBatchRequest& request) const;

        /**
         * @brief Performs a Speech-to-Text batch transcription using raw audio bytes.
         * @param audioBytes The raw audio bytes to transcribe.
         * @param request The STTBatchRequest containing transcription parameters.
         * @return A BatchResponse containing the transcription result.
         */
        response::stt::BatchResponse sttWithBytes(const std::vector<char>& audioBytes, const request::STTBatchRequest& request) const;

        private:
        std::unique_ptr<CartesiaClientImpl> _clientImpl;
    };
}

#endif // CARTESIA_APP_HPP