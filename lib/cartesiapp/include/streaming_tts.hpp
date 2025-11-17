#ifndef STREAMING_TTS_HPP
#define STREAMING_TTS_HPP

#include "cartesiapp.hpp"

namespace cartesiapp {

    // Forward declaration of WebsocketClient implementation class
    class WebsocketClientImpl;

    // Forward declaration of TTSResponseListener interface
    class TTSResponseListener;

    namespace tts_events {
        constexpr const char* AUDIO_CHUNK = "chunk";
        constexpr const char* DONE = "done";
        constexpr const char* WORD_TIMESTAMPS = "timestamps";
        constexpr const char* PHONEME_TIMESTAMPS = "phoneme_timestamps";
        constexpr const char* FLUSH_DONE = "flush_done";
        constexpr const char* ERROR_ = "error";
    }

    /**
     * @brief Client class for managing Text-to-Speech WebSocket connections.
     */
    class CARTESIAPP_EXPORT TTSWebsocketClient {
        public:
        TTSWebsocketClient(const std::string& apiKey,
            const std::string& apiVersion = request::api_versions::LATEST);
        ~TTSWebsocketClient();

        /**
         * @brief Connects to the TTS WebSocket and starts the data reception thread.
         */
        bool connectAndStart();

        /**
         * @brief Disconnects from the TTS WebSocket and stops the data reception thread.
         */
        void disconnect();

        /**
         * @brief Checks if the WebSocket is connected and the data reception thread is running.
         */
        bool isConnectedAndStarted() const;

        /**
         * @brief Initiates a Text-to-Speech generation request via streaming.
         * @param request The GenerationRequest containing generation parameters.
         */
        bool requestTTS(const request::tts::GenerationRequest& request) const;

        /**
         * @brief Cancels an ongoing TTS context/session.
         * @param request The TTSCancelContextRequest containing the context ID to cancel.
         */
        bool cancelTTSContext(const request::tts::CancelContextRequest& request) const;

        /**
         * @brief Registers a TTS response listener.
         * @param listener A weak pointer to the TTSResponseListener to register.
         */
        void registerTTSListener(std::weak_ptr<TTSResponseListener> listener);

        /**
         * @brief Unregisters the TTS response listener.
         */
        void unregisterTTSListener();

        private:
        std::unique_ptr<WebsocketClientImpl> _websocketClientImpl;
        std::weak_ptr<TTSResponseListener> _ttsListener;
        std::string _apiVersion;
        std::string _apiKey;
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
};

#endif // STREAMING_TTS_HPP