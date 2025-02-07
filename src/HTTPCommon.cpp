/**
 * Source file for the HTTPCommon
 */
#include "HTTPCommon.hpp"

namespace sgns
{
    using namespace boost::asio;
    HTTPDevice::HTTPDevice(
        std::string http_host,
        std::string http_path,
        std::string http_port,
        bool parse, bool save) 
    {
        http_host_ = http_host;
        http_path_ = http_path;
        http_port_ = http_port;
        parse_ = parse;
        save_ = save;
    }


    void HTTPDevice::StartHTTPDownload(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<boost::asio::ssl::context> ssl_context , CompletionCallback handle_read, StatusCallback status)
    {
        //Get DNS result for hostname

        boost::asio::ip::tcp::resolver resolver(*ioc);
        boost::asio::ip::tcp::endpoint endpoint;
        try {
            std::cout << "resolving address" << std::endl;
            boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(http_host_, "https");
            endpoint = *results.begin();
        }
        catch (const boost::system::system_error& e) {
            std::cerr << "Error resolving address: " << e.what() << std::endl;
            status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Could not resolve address")));
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Could not resolve address")));
        }
        catch (...) {
            std::cerr << "Unknown error occurred during address resolution." << std::endl;
            status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Could not resolve address")));
        }
        //boost::asio::ip::tcp::endpoint endpoint = *results.begin();

        //Create SSL Context
        // static std::mutex openssl_mutex;
        // std::lock_guard<std::mutex> lock(openssl_mutex);
        // auto ssl_context = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls);

        // //Disclude certain older insecure options
        // ssl_context->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3);

        // //Default trusted authority definitions
        // ssl_context->set_default_verify_paths();

        //Consider setting verify callback to check whether domain name matches cert
        // ssl_context->set_verify_callback(...);
        //Create Socket with SSL Context
        auto socket = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ssl_context);

        //Connect socket
        status(CustomResult(sgns::AsyncError::outcome::success(Success{ "Starting HTTP Connection" })));
        socket->lowest_layer().async_connect(endpoint, [self = shared_from_this(), ioc, socket, handle_read, status](const boost::system::error_code& connect_error)
            {
                if (!connect_error)
                {
                    status(CustomResult(sgns::AsyncError::outcome::success(Success{ "SSL Handshake Started" })));
                    unsigned long ssl_err = ERR_get_error();
                    while (ssl_err) {
                        std::cerr << "OpenSSL Error before handshake: " << ERR_error_string(ssl_err, nullptr) << std::endl;
                        ssl_err = ERR_get_error();
                    }
                    socket->async_handshake(boost::asio::ssl::stream_base::client, [self , ioc, socket, handle_read, status](const boost::system::error_code& handshake_error) {
                        if (!handshake_error) {
                            // Start the asynchronous download for a specific path
                            self->StartHTTPGet(ioc, socket, handle_read, status);
                        }
                        else {
                            std::cerr << "Handshake error: " << handshake_error.message() << std::endl;
                            status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Handshake Error")));
                            self->shutdownSocket(socket);
                            handle_read(std::shared_ptr<std::pair<std::vector<std::string>, std::vector<std::vector<char>>>>(), false, false);
                        }
                        });
                }
                else {
                    std::cerr << "Connection error: " << connect_error.message() << std::endl;
                    status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Connection Error")));
                    self->shutdownSocket(socket);
                    handle_read(std::shared_ptr<std::pair<std::vector<std::string>, std::vector<std::vector<char>>>>(), false, false);
                }
            });
    }

    void HTTPDevice::StartHTTPGet(std::shared_ptr<boost::asio::io_context> ioc,
        std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket,
        CompletionCallback handle_read,
        StatusCallback status)
    {
        //Create HTTP Get request and write to server
        status(CustomResult(sgns::AsyncError::outcome::success(Success{ "Starting HTTP Get Request" })));
        std::string get_request = "GET " + http_path_ + " HTTP/1.1\r\nHost: " + http_host_ + "\r\nConnection: close\r\n\r\n";
        boost::asio::async_write(*socket, boost::asio::buffer(get_request), [self = shared_from_this(), ioc, handle_read, status, socket](const boost::system::error_code& write_error, std::size_t) {
            if (!write_error) {
                //Create a buffer for returned data and read from server
                auto headerbuff = std::make_shared<boost::asio::streambuf>();
                status(CustomResult(sgns::AsyncError::outcome::success(Success{ "Starting HTTP File Read" })));
                if (!socket->lowest_layer().is_open()) {
                    std::cerr << "Socket unexpectedly closed before async_read!" << std::endl;
                    return;
                }
                unsigned long ssl_err = ERR_get_error();
                while (ssl_err) {
                    std::cerr << "OpenSSL Pre-Read Error: " << ERR_error_string(ssl_err, nullptr) << std::endl;
                    ssl_err = ERR_get_error();
                }
                socket->lowest_layer().async_wait(boost::asio::ip::tcp::socket::wait_read,
                [socket](const boost::system::error_code& ec) {
                    if (ec) {
                        std::cerr << "ERROR: Socket closed by another process! " << ec.message() << std::endl;
                    }
                });

                std::cout << "Boost.Asio pending handlers: " << ioc->poll() << std::endl;
                std::cout << "Queueing async_read now..." << std::endl;

                boost::asio::async_read(*socket, *headerbuff, boost::asio::transfer_all(), [self, ioc, handle_read, status, headerbuff, socket](const boost::system::error_code& read_error, std::size_t bytes_transferred) {
                    std::cout << "Async Read Done" << std::endl;
                    try{
                        //Make a vector buffer from data
                        auto buffer = std::make_shared<std::vector<char>>(boost::asio::buffers_begin(headerbuff->data()), boost::asio::buffers_end(headerbuff->data()));

                        //Copy to a string
                        std::string bufferStr(buffer->begin(), buffer->end());

                        //Find end of header 
                        size_t headerEnd = bufferStr.find("\r\n\r\n");

                        //Check if we found an end
                        if (headerEnd != std::string::npos) {
                            //Create vector of binary data by cutting off the header.
                            //auto binaryData = std::make_shared<std::vector<char>>(buffer->begin() + headerEnd + 4, buffer->end());

                            //Send this to handler to be processed.
                            //std::cout << "HTTPS Finish" << std::endl;
                            status(CustomResult(sgns::AsyncError::outcome::success(Success{ "HTTP Get finished" })));
                            auto finaldata = std::make_shared<std::pair<std::vector<std::string>, std::vector<std::vector<char>>>>();
                            std::filesystem::path p(self->http_path_);
                            finaldata->first.push_back(p.filename().string());
                            //finaldata->second.push_back(*binaryData);
                            finaldata->second.emplace_back(
                                buffer->begin() + headerEnd + 4,  
                                buffer->end()                     
                            );
                            self->shutdownSocket(socket);
                            handle_read(finaldata, self->parse_, self->save_);
                        }
                        else {
                            status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Data Read failed. No header.")));
                            self->shutdownSocket(socket);
                            handle_read(std::shared_ptr<std::pair<std::vector<std::string>, std::vector<std::vector<char>>>>(), false, false);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Exception in read handler: " << e.what() << std::endl;
                        status(CustomResult(sgns::AsyncError::outcome::failure("Exception in HTTP read handler")));
                        self->shutdownSocket(socket);
                        handle_read(std::shared_ptr<std::pair<std::vector<std::string>, 
                            std::vector<std::vector<char>>>>(), false, false);
                    }
                    });
            }
            else {
                std::cerr << "Error in async_write: " << write_error.message() << std::endl;
                status(CustomResult(sgns::AsyncError::outcome::failure("HTTP Data Read failed. Get Request Fail.")));
                self->shutdownSocket(socket);
                handle_read(std::shared_ptr<std::pair<std::vector<std::string>, std::vector<std::vector<char>>>>(), false, false);
            }
            });
    }
    void HTTPDevice::shutdownSocket(std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket) {
        if (!socket) return;

        std::cout << "Shutting down SSL socket..." << std::endl;

        socket->async_shutdown([self = shared_from_this(), socket](const boost::system::error_code& ec) {
            if (ec) {
                if (ec == boost::asio::error::eof) {
                    std::cout << "SSL connection closed gracefully.\n";
                } else {
                    std::cerr << "SSL shutdown error: " << ec.message() << std::endl;
                }
            } else {
                std::cout << "SSL shutdown completed successfully.\n";
            }

            // Close the underlying TCP socket
            boost::system::error_code ignored_ec;
            socket->lowest_layer().close(ignored_ec);
            if (ignored_ec) {
                std::cerr << "Error closing socket: " << ignored_ec.message() << std::endl;
            }
            std::cout << "Socket fully closed.\n";
        });
    }


}