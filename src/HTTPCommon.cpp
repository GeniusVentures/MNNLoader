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

    void HTTPDevice::StartHTTPDownload(std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read, StatusCallback status)
    {
        //Get DNS result for hostname
        boost::asio::ip::tcp::resolver resolver(*ioc);
        boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(http_host_, "https");
        boost::asio::ip::tcp::endpoint endpoint = *results.begin();

        //Create SSL Context
        auto ssl_context = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls);

        //Disclude certain older insecure options
        ssl_context->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3);

        //Default trusted authority definitions
        ssl_context->set_default_verify_paths();

        //Consider setting verify callback to check whether domain name matches cert
        // ssl_context->set_verify_callback(...);
        //Create Socket with SSL Context
        auto socket = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ssl_context);

        //Connect socket
        status(1);
        socket->lowest_layer().async_connect(endpoint, [self = shared_from_this(), ioc, socket, handle_read, status](const boost::system::error_code& connect_error)
            {
                if (!connect_error)
                {
                    status(9);
                    socket->async_handshake(boost::asio::ssl::stream_base::client, [self , ioc, socket, handle_read, status](const boost::system::error_code& handshake_error) {
                        if (!handshake_error) {
                            // Start the asynchronous download for a specific path
                            self->StartHTTPGet(ioc, socket, handle_read, status);
                        }
                        else {
                            std::cerr << "Handshake error: " << handshake_error.message() << std::endl;
                            status(-9);
                            handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                        }
                        });
                }
                else {
                    std::cerr << "Connection error: " << connect_error.message() << std::endl;
                    status(-1);
                    handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                }
            });
    }

    void HTTPDevice::StartHTTPGet(std::shared_ptr<boost::asio::io_context> ioc,
        std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket,
        CompletionCallback handle_read,
        StatusCallback status)
    {
        //Create HTTP Get request and write to server
        status(8);
        std::string get_request = "GET " + http_path_ + " HTTP/1.1\r\nHost: " + http_host_ + "\r\nConnection: close\r\n\r\n";
        boost::asio::async_write(*socket, boost::asio::buffer(get_request), [self = shared_from_this(), ioc, handle_read, status, socket](const boost::system::error_code& write_error, std::size_t) {
            if (!write_error) {
                //Create a buffer for returned data and read from server
                auto headerbuff = std::make_shared<boost::asio::streambuf>();
                status(7);
                boost::asio::async_read(*socket, *headerbuff, boost::asio::transfer_all(), [self, ioc, handle_read, status, headerbuff, socket](const boost::system::error_code& read_error, std::size_t bytes_transferred) {
                    //Make a vector buffer from data
                    auto buffer = std::make_shared<std::vector<char>>(boost::asio::buffers_begin(headerbuff->data()), boost::asio::buffers_end(headerbuff->data()));

                    //Copy to a string
                    std::string bufferStr(buffer->begin(), buffer->end());

                    //Find end of header 
                    size_t headerEnd = bufferStr.find("\r\n\r\n");

                    //Check if we found an end
                    if (headerEnd != std::string::npos) {
                        //Create vector of binary data by cutting off the header.
                        auto binaryData = std::make_shared<std::vector<char>>(buffer->begin() + headerEnd + 4, buffer->end());

                        //Send this to handler to be processed.
                        std::cout << "HTTPS Finish" << std::endl;
                        status(0);
                        handle_read(ioc, binaryData, self->parse_, self->save_);
                    }
                    else {
                        std::cerr << "Data does not contain header" << std::endl;
                        status(-7);
                        handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                    }
                    });
            }
            else {
                std::cerr << "Error in async_write: " << write_error.message() << std::endl;
                status(-8);
                handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
            }
            });
    }
}