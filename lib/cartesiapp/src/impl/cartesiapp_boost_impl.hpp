#ifndef CARTESIA_APP_BOOST_IMPL_HPP
#define CARTESIA_APP_BOOST_IMPL_HPP

#include "cartesiapp.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

// boost
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>

// for convenience
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

using Websocket = boost::beast::websocket::stream<ssl::stream<tcp::socket>>;

namespace cartesiapp {
    /**
     * @brief Implementation class for Cartesia API client using Boost.Beast
     */
    class CartesiaClientImpl {
        public:
        CartesiaClientImpl(const std::string& apiKey, const std::string& apiVersion, bool verifyCertificates) :
            _apiKey(apiKey),
            _apiVersion(apiVersion),
            _resolver(_ioContext),
            _sslContext(ssl::context::tls_client),
            _websocket(_ioContext, _sslContext),
            _verifyCertificates(verifyCertificates) {

            try {
                bool c = connectWebsocket(verifyCertificates);
                spdlog::debug(">>>>>>>>>>>> Websocket connected: {}", c);
            }
            catch (const std::exception& ex) {
                spdlog::error(">>>>>>>>>>>>>> Error connecting websocket: {}", ex.what());
            }
        }

        ~CartesiaClientImpl() {

        }

        void overrideApiVersion(const std::string& apiVersion) {
            _apiVersion = apiVersion;
        }

        std::string getApiVersion() const {
            return _apiVersion;
        }

        bool isWebsocketConnected() const {
            return _websocket.is_open();
        }

        response::ApiInfo getApiInfo() const {
            spdlog::debug("Getting API info...");

            beast::error_code ec;

            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            http::request<http::empty_body> httpRequest{
                http::verb::get,
                cartesiapp::request::constants::ENDPOINT_API_STATUS_INFO,
                11
            };

            httpRequest.set(http::field::host, cartesiapp::request::constants::HOST);
            httpRequest.set(http::field::user_agent, cartesiapp::request::constants::USER_AGENT);
            httpRequest.set(cartesiapp::request::constants::HEADER_CARTESIA_VERSION, _apiVersion);
            httpRequest.set(http::field::connection, "close");
            httpRequest.prepare_payload();

            http::write(sslStream, httpRequest);

            beast::flat_buffer buffer;
            http::response<http::string_body> httpResponse;
            http::read(sslStream, buffer, httpResponse);

            sslStream.shutdown(ec);

            if (ec && ec != beast::errc::not_connected) {
                throw beast::system_error{ ec };
            }

            response::ApiInfo apiInfo = response::ApiInfo::fromJson(httpResponse.body());
            return apiInfo;
        }

        response::Voice getVoice(const std::string& voiceId) const {
            spdlog::debug("Getting voice with ID: {}", voiceId);
            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            // request url with voice id
            std::string requestUrl = std::string(cartesiapp::request::constants::ENDPOINT_VOICES) + "/" + voiceId;

            // create the HTTP GET request
            http::request<http::empty_body> httpRequest{
                http::verb::get,
                requestUrl,
                11
            };

            httpRequest.set(http::field::host, cartesiapp::request::constants::HOST);
            httpRequest.set(http::field::user_agent, cartesiapp::request::constants::USER_AGENT);
            httpRequest.set(http::field::authorization, "Bearer " + _apiKey);
            httpRequest.set(cartesiapp::request::constants::HEADER_CARTESIA_VERSION, _apiVersion);
            httpRequest.set(http::field::connection, "close");

            httpRequest.prepare_payload();

            http::write(sslStream, httpRequest);

            beast::flat_buffer buffer;
            http::response<http::string_body> httpResponse;
            http::read(sslStream, buffer, httpResponse);
            beast::error_code ec;
            sslStream.shutdown(ec);

            if (ec && ec != beast::errc::not_connected) {
                throw beast::system_error{ ec };
            }

            int code = httpResponse.result_int();
            std::string response = std::move(httpResponse.body());

            // check for non-200-series HTTP response code
            if (code / 200 != 1) {
                std::string errorMsg = "Error getting voice, HTTP code: " + std::to_string(code) + ", response: " + response;
                throw spdlog::spdlog_ex(errorMsg, code);
            }

            return response::Voice::fromJson(response);
        }

        response::VoiceListPage getVoiceList(const request::VoiceListRequest& request) const {
            std::string queryParams = request.toQueryParams();
            spdlog::debug("Getting voice list... query params: {}", queryParams);
            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            // request url with query parameters
            std::string requestUrl = cartesiapp::request::constants::ENDPOINT_VOICES + queryParams;

            // create the HTTP GET request
            http::request<http::empty_body> httpRequest{
                http::verb::get,
                requestUrl,
                11
            };

            httpRequest.set(http::field::host, cartesiapp::request::constants::HOST);
            httpRequest.set(http::field::user_agent, cartesiapp::request::constants::USER_AGENT);
            httpRequest.set(http::field::authorization, "Bearer " + _apiKey);
            httpRequest.set(cartesiapp::request::constants::HEADER_CARTESIA_VERSION, _apiVersion);
            httpRequest.set(http::field::connection, "close");

            httpRequest.prepare_payload();

            http::write(sslStream, httpRequest);

            beast::flat_buffer buffer;
            http::response<http::string_body> httpResponse;
            http::read(sslStream, buffer, httpResponse);
            beast::error_code ec;
            sslStream.shutdown(ec);

            if (ec && ec != beast::errc::not_connected) {
                throw beast::system_error{ ec };
            }

            int code = httpResponse.result_int();
            std::string response = std::move(httpResponse.body());

            // check for non-200-series HTTP response code
            if (code / 200 != 1) {
                std::string errorMsg = "Error getting voice list, HTTP code: " + std::to_string(code) + ", response: " + response;
                throw spdlog::spdlog_ex(errorMsg, code);
            }

            return response::VoiceListPage::fromJson(response);
        }

        std::string ttsBytes(const request::TTSBytesRequest& request) const {
            spdlog::debug("Performing TTS Bytes request...");

            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            // create the HTTP POST request
            http::request<http::string_body> httpRequest{
                http::verb::post,
                cartesiapp::request::constants::ENDPOINT_TTS_BYTES,
                11
            };

            httpRequest.set(http::field::host, cartesiapp::request::constants::HOST);
            httpRequest.set(http::field::user_agent, cartesiapp::request::constants::USER_AGENT);
            httpRequest.set(http::field::authorization, "Bearer " + _apiKey);
            httpRequest.set(cartesiapp::request::constants::HEADER_CARTESIA_VERSION, _apiVersion);
            httpRequest.set(http::field::content_type, "application/json");
            httpRequest.set(http::field::connection, "close");

            // set the body of the request to the JSON representation of the TTSBytesRequest
            httpRequest.body() = request.toJson();

            spdlog::debug("TTS Bytes request JSON body: {}", httpRequest.body());

            httpRequest.prepare_payload();

            // send the HTTP request to the remote host
            http::write(sslStream, httpRequest);

            // receive the HTTP response
            beast::flat_buffer buffer;
            http::response<http::string_body> httpResponse;
            http::read(sslStream, buffer, httpResponse);

            beast::error_code ec;
            sslStream.shutdown(ec);

            // check for errors
            if (ec && ec != beast::errc::not_connected) {
                throw beast::system_error{ ec };
            }

            std::string response = httpResponse.body();
            return response;
        }

        response::stt::BatchResponse sttWithBytes(const std::vector<char>& audioBytes,
            const request::STTBatchRequest& request,
            const std::string& mime = "application/octet-stream") const {
            spdlog::debug("Performing STT Batch request...");

            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            // request URL with query parameters
            std::string requestUrl = cartesiapp::request::constants::ENDPOINT_STT + request.toQueryParams();

            // create the HTTP POST request
            http::request<http::vector_body<char>> httpRequest{
                http::verb::post,
                requestUrl,
                11
            };

            httpRequest.set(http::field::host, cartesiapp::request::constants::HOST);
            httpRequest.set(http::field::user_agent, cartesiapp::request::constants::USER_AGENT);
            httpRequest.set(http::field::authorization, "Bearer " + _apiKey);
            httpRequest.set(cartesiapp::request::constants::HEADER_CARTESIA_VERSION, _apiVersion);

            // set connection to close after the transaction
            httpRequest.set(http::field::connection, "close");

            // Generate boundary for multipart form-data
            std::string boundary = "----CartesiaPP-FormBoundary" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());

            // Set content type to multipart/form-data with boundary
            httpRequest.set(http::field::content_type, "multipart/form-data; boundary=" + boundary);

            // Build multipart form-data body using vector<char> to handle binary data properly
            std::vector<char> formData;

            auto appendString = [&formData](const std::string& str) {
                formData.insert(formData.end(), str.begin(), str.end());
                };

            auto appendBoundary = [&formData, &boundary, appendString]() {
                appendString("--" + boundary + "\r\n");
                };

            // Add model field
            appendBoundary();
            appendString("Content-Disposition: form-data; name=\"model\"\r\n\r\n");
            appendString(request.model + "\r\n");

            // Add language field (if available)
            if (request.language.has_value()) {
                appendBoundary();
                appendString("Content-Disposition: form-data; name=\"language\"\r\n\r\n");
                appendString(request.language.value() + "\r\n");
            }

            // Add timestamp granularities field (if available)
            if (!request.timestamp_granularities.empty()) {
                for (const auto& granularity : request.timestamp_granularities) {
                    appendBoundary();
                    appendString("Content-Disposition: form-data; name=\"timestamp_granularities[]\"\r\n\r\n");
                    appendString(granularity + "\r\n");
                }
            }

            // Add audio file field
            appendBoundary();
            appendString("Content-Disposition: form-data; name=\"file\"; filename=\"file\"\r\n");
            // append the mime type
            std::string contentType = "Content-Type: " + mime + "\r\n\r\n";
            appendString(contentType);

            // Append audio bytes directly
            formData.insert(formData.end(), audioBytes.begin(), audioBytes.end());
            appendString("\r\n");

            appendString("--" + boundary + "--\r\n");

            httpRequest.body() = std::move(formData);
            httpRequest.prepare_payload();

            // send the HTTP request to the remote host
            http::write(sslStream, httpRequest);

            // receive the HTTP response
            beast::flat_buffer buffer;
            http::response<http::string_body> httpResponse;
            http::read(sslStream, buffer, httpResponse);

            beast::error_code ec;
            sslStream.shutdown(ec);

            // check for errors
            if (ec && ec != beast::errc::not_connected) {
                throw beast::system_error{ ec };
            }

            int code = httpResponse.result_int();
            std::string response = std::move(httpResponse.body());

            // check for non-200-series HTTP response code
            if (code / 200 != 1) {
                std::string errorMsg = "Error transcribing audio, HTTP code: " + std::to_string(code) + ", response: " + response;
                throw spdlog::spdlog_ex(errorMsg, code);
            }

            spdlog::debug("STT Batch response: {}", response);

            return response::stt::BatchResponse::fromJson(response);
        }

        private:
        ssl::stream<beast::tcp_stream> createSSLStream(bool verifyCertificates) const {
            //ssl::context _sslContext(ssl::context::tls_client);

            beast::error_code ec;
            // Add SSL verification and options
            _sslContext.set_default_verify_paths(ec);

            if (ec) {
                spdlog::error("Error setting default verify paths: {}", ec.message());
                throw beast::system_error{ ec };
            }

            _sslContext.set_verify_mode(ssl::verify_peer);
            _sslContext.set_verify_callback([verifyCertificates](bool preverified, ssl::verify_context& ctx) {
                // Log certificate info for debugging
                X509_STORE_CTX* store_ctx = ctx.native_handle();
                X509* cert = X509_STORE_CTX_get_current_cert(store_ctx);
                if (cert) {
                    char subject_name[256];
                    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
                    spdlog::debug("Certificate subject: {}", subject_name);
                }

                if (!preverified) {
                    int error = X509_STORE_CTX_get_error(store_ctx);
                    spdlog::error("Certificate verification failed: {}, verification {}", X509_verify_cert_error_string(error), verifyCertificates ? "enabled" : "disabled");
                    return !verifyCertificates; // Reject invalid certificates if verification is enabled
                }
                return true;
                });

            auto const results = _resolver.resolve(cartesiapp::request::constants::HOST, "443");

            ssl::stream<beast::tcp_stream> sslStream(_ioContext, _sslContext);

            // Set SNI hostname
            if (!SSL_set_tlsext_host_name(sslStream.native_handle(), cartesiapp::request::constants::HOST)) {
                beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
                throw beast::system_error{ ec };
            }

            beast::get_lowest_layer(sslStream).connect(results);
            sslStream.handshake(ssl::stream_base::client);

            return std::move(sslStream);
        }

        bool connectWebsocket(bool verifyCertificates)
        {
            try
            {
                auto const results = _resolver.resolve(cartesiapp::request::constants::HOST, "443");
                net::connect(_websocket.next_layer().lowest_layer(), results.begin(), results.end());
                spdlog::info("Performing SSL handshake...");                
                _websocket.next_layer().set_verify_callback([verifyCertificates](bool preverified, ssl::verify_context& ctx) {
                    return true;
                    // Log certificate info for debugging
                    X509_STORE_CTX* store_ctx = ctx.native_handle();
                    X509* cert = X509_STORE_CTX_get_current_cert(store_ctx);
                    if (cert) {
                        char subject_name[256];
                        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
                        spdlog::debug("Certificate subject: {}", subject_name);
                    }

                    if (!preverified) {
                        int error = X509_STORE_CTX_get_error(store_ctx);
                        spdlog::error("Certificate verification failed: {}, verification {}", X509_verify_cert_error_string(error), verifyCertificates ? "enabled" : "disabled");
                        return !verifyCertificates; // Reject invalid certificates if verification is enabled
                    }
                    return true;
                    });
                _websocket.next_layer().set_verify_mode(ssl::verify_none);
                _websocket.next_layer().handshake(ssl::stream_base::handshake_type::client);
                _websocket.handshake(cartesiapp::request::constants::HOST, cartesiapp::request::constants::ENDPOINT_TTS_WEBSOCKET);
                spdlog::info("WebSocket connected successfully!");
            }
            catch (std::exception& e)
            {
                spdlog::error("Error occurred: {}", e.what());
                return false;
            }
            return true;
        }

        private:
        std::string _apiKey;
        std::string _apiVersion;
        bool _verifyCertificates;

        std::thread _workerThread;

        // Boost.Asio components, mutable to allow modification in const methods that are exposed to users
        mutable ssl::context _sslContext;
        mutable net::io_context _ioContext;
        mutable tcp::resolver _resolver;
        mutable Websocket _websocket;
    };
}

#endif