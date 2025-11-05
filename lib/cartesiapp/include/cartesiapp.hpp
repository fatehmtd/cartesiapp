#ifndef CARTESIA_APP_HPP
#define CARTESIA_APP_HPP

#include <string>
#include <memory>
#include <map>

#include "cartesiapp_export.hpp"

#include "cartesiapp_request.hpp"
#include "cartesiapp_response.hpp"

namespace cartesiapp {

    // Forward declaration of TTSResponseListener class
    class TTSResponseListener;

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
         * @param request The STTBatchRequest containing transcription parameters.
         * @return A BatchResponse containing the transcription result.
         */
        response::stt::BatchResponse sttWithFile(const std::string& filePath,
            const request::STTBatchRequest& request) const;

        /**
         * @brief Performs a Speech-to-Text batch transcription using raw audio bytes.
         * @param audioBytes The raw audio bytes to transcribe.
         * @param request The STTBatchRequest containing transcription parameters.
         * @return A BatchResponse containing the transcription result.
         */
        response::stt::BatchResponse sttWithBytes(const std::vector<char>& audioBytes,
            const request::STTBatchRequest& request) const;

        /**
         * @brief Registers a TTS response listener.
         * @param listener A weak pointer to the TTSResponseListener to register.
         */
        void registerListener(std::weak_ptr<TTSResponseListener> listener);

        /**
         * @brief Unregisters the TTS response listener.
         */
        void unregisterListener();

        /**
         * @brief Starts the TTS WebSocket connection.
         * @return True if the connection was started successfully, false otherwise.
         */
        bool startTTSWebsocketConnection() const;

        /**
         * @brief Stops the TTS WebSocket connection.
         * @return True if the connection was stopped successfully, false otherwise.
         */
        bool stopTTSWebsocketConnection() const;

        /**
         * @brief Initiates a Text-to-Speech generation request via streaming.
         * @param request The TTSGenerationRequest containing generation parameters.
         */
        bool requestTTS(const request::TTSGenerationRequest& request) const;

        /**
         * @brief Cancels an ongoing TTS context/session.
         * @param request The TTSCancelContextRequest containing the context ID to cancel.
         */
        bool cancelTTSContext(const request::TTSCancelContextRequest& request) const;

        private:
        std::unique_ptr<CartesiaClientImpl> _clientImpl;
        std::weak_ptr<TTSResponseListener> _listener;
    };

    /**
     * @brief Interface for receiving Text-to-Speech response callbacks.
     */
    class CARTESIAPP_EXPORT TTSResponseListener {
        public:
        virtual ~TTSResponseListener() = default;

        /**
         * @brief Callback method invoked when the WebSocket connection is established.
         */
        virtual void onConnected() = 0;

        /**
         * @brief Callback method invoked when the WebSocket connection is disconnected.
         * @param reason The reason for the disconnection.
         */
        virtual void onDisconnected(const std::string& reason) = 0;

        /**
         * @brief Callback method invoked when a network error occurs.
         * @param errorMessage The error message describing the network error.
         */
        virtual void onNetworkError(const std::string& errorMessage) = 0;

        /**
         * @brief Callback method invoked when a TTS audio chunk response is received.
         * @param response The AudioChunkResponse received from the TTS service.
         */
        virtual void onAudioChunkReceived(const response::tts::AudioChunkResponse& response) = 0;

        /**
         * @brief Callback method invoked when a TTS done response is received.
         * @param response The DoneResponse received from the TTS service.
         */
        virtual void onDoneReceived(const response::tts::DoneResponse& response) = 0;

        /**
         * @brief Callback method invoked when word timestamps response is received.
         * @param response The WordTimestampsResponse received from the TTS service.
         */
        virtual void onWordTimestampsReceived(const response::tts::WordTimestampsResponse& response) = 0;

        /**
         * @brief Callback method invoked when phoneme timestamps response is received.
         * @param response The PhonemeTimestampsResponse received from the TTS service.
         */
        virtual void onPhonemeTimestampsReceived(const response::tts::PhonemeTimestampsResponse& response) = 0;

        /**
         * @brief Callback method invoked when a flush done response is received.
         * @param response The FlushDoneResponse received from the TTS service.
         */
        virtual void onFlushDoneReceived(const response::tts::FlushDoneResponse& response) = 0;

        /**
         * @brief Callback method invoked when an error response is received.
         * @param response The ErrorResponse received from the TTS service.
         */
        virtual void onError(const response::tts::ErrorResponse& response) = 0;
    };
}

#endif // CARTESIA_APP_HPP