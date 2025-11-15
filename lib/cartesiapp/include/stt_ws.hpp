#ifndef CARTESIA_STT_WS_HPP
#define CARTESIA_STT_WS_HPP

#include "cartesiapp.hpp"

namespace cartesiapp {
    /**
     * @brief Interface for receiving Speech-to-Text response callbacks.
     */
    class CARTESIAPP_EXPORT STTResponseListener {
        public:
        virtual ~STTResponseListener() = default;

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
         * @brief Callback method invoked when a transcription response is received.
         * @param response The TranscriptionResponse received from the STT service.
         */
        virtual void onTranscriptionReceived(const response::stt::TranscriptionResponse& response) = 0;

        /**
         * @brief Callback method invoked when a done response is received.
         * @param response The DoneResponse received from the STT service.
         */
        virtual void onDoneReceived(const response::stt::DoneResponse& response) = 0;

        /**
         * @brief Callback method invoked when a flush done response is received.
         * @param response The FlushDoneResponse received from the STT service.
         */
        virtual void onFlushDoneReceived(const response::stt::FlushDoneResponse& response) = 0;

        /**
         * @brief Callback method invoked when an error response is received.
         * @param response The ErrorResponse received from the STT service.
         */
        virtual void onError(const response::stt::ErrorResponse& response) = 0;
    };

    /**
     * @brief Client class for managing Speech-to-Text WebSocket connections.
     */
    class CARTESIAPP_EXPORT STTWebsocketClient {
        public:
        STTWebsocketClient(const std::string& apiKey,
            const std::string& apiVersion = request::api_versions::LATEST);
        virtual ~STTWebsocketClient() = default;

        /**
         * @brief Connects to the STT WebSocket and starts the data reception thread.
         */
        bool connectAndStart();

        /**
         * @brief Disconnects from the STT WebSocket and stops the data reception thread.
         */
        void disconnect();

        /**
         * @brief Sends a done request to the STT service.
         */
        void sendDoneRequest() const;

        /**
         * @brief Sends a flush request to the STT service.
         */
        void sendFlushRequest() const;

        /**
         * @brief Writes audio bytes to the STT WebSocket.
         * @param data Pointer to the audio byte data.
         * @param size Size of the audio byte data.
         */
        void writeAudioBytes(const char* data, size_t size) const;

        /**
         * @brief Registers an STT response listener.
         * @param listener A weak pointer to the STTResponseListener to register.
         */
        void registerSTTListener(std::weak_ptr<STTResponseListener> listener);

        /**
         * @brief Unregisters the STT response listener.
         */
        void unregisterSTTListener();

        private:
        std::unique_ptr<WebsocketClientImpl> _websocketClientImpl;
        std::weak_ptr<STTResponseListener> _sttListener;
    };
}

#endif