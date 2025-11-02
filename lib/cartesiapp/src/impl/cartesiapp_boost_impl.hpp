#ifndef CARTESIA_APP_BOOST_IMPL_HPP
#define CARTESIA_APP_BOOST_IMPL_HPP

#include "cartesiapp.hpp"

#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <sstream>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

// for convenience
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

namespace cartesiapp {
    class CartesiaClientImpl {
        public:
        CartesiaClientImpl(const std::string& apiKey, const std::string& apiVersion, bool verifyCertificates) :
            _apiKey(apiKey),
            _apiVersion(apiVersion),
            _resolver(_ioContext),
            _verifyCertificates(verifyCertificates) {
        }

        ~CartesiaClientImpl() {

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
            httpRequest.set(cartesiapp::request::constants::HEADER_API_KEY, _apiKey);
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

            // create the HTTP GET request
            http::request<http::empty_body> httpRequest{
                http::verb::get,
                std::string(cartesiapp::request::constants::ENDPOINT_VOICES) + "/" + voiceId,
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

            return response::Voice::fromJson(httpResponse.body());
        }

        response::VoiceListPage getVoiceList(request::VoiceListRequest& request) const {
            std::string queryParams = request.toQueryParams();
            spdlog::debug("Getting voice list... query params: {}", queryParams);
            ssl::stream<beast::tcp_stream> sslStream = createSSLStream(_verifyCertificates);

            // create the HTTP GET request
            http::request<http::empty_body> httpRequest{
                http::verb::get,
                cartesiapp::request::constants::ENDPOINT_VOICES + queryParams,
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

            return response::VoiceListPage::fromJson(httpResponse.body());
        }

        std::string ttsBytes(request::TTSBytesRequest& request) const {
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
            httpRequest.set(cartesiapp::request::constants::HEADER_API_KEY, _apiKey);
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

        private:
        ssl::stream<beast::tcp_stream> createSSLStream(bool verifyCertificates) const {
            ssl::context sslContext(ssl::context::tls_client);

            beast::error_code ec;
            // Add SSL verification and options
            sslContext.set_default_verify_paths(ec);

            if (ec) {
                spdlog::error("Error setting default verify paths: {}", ec.message());
                throw beast::system_error{ ec };
            }

            sslContext.set_verify_mode(ssl::verify_peer);
            sslContext.set_verify_callback([verifyCertificates](bool preverified, ssl::verify_context& ctx) {
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

            ssl::stream<beast::tcp_stream> sslStream(_ioContext, sslContext);

            // Set SNI hostname
            if (!SSL_set_tlsext_host_name(sslStream.native_handle(), cartesiapp::request::constants::HOST)) {
                beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
                throw beast::system_error{ ec };
            }

            beast::get_lowest_layer(sslStream).connect(results);
            sslStream.handshake(ssl::stream_base::client);

            return std::move(sslStream);
        }

        private:
        std::string _apiKey;
        std::string _apiVersion;
        mutable net::io_context _ioContext;
        mutable tcp::resolver _resolver;
        bool _verifyCertificates;
    };
}

#endif