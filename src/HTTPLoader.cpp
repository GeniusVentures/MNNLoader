#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include "FileManager.hpp"
#include "HTTPLoader.hpp"
#include "boost/asio/ssl.hpp"
#include "URLStringUtil.h"


namespace sgns
{
    SINGLETON_PTR_INIT(HTTPLoader);
    HTTPLoader::HTTPLoader()
    {
        FileManager::GetInstance().RegisterLoader("http", this);
        FileManager::GetInstance().RegisterLoader("https", this);
    }

    std::shared_ptr<void> HTTPLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the HTTPLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }

    boost::asio::ssl::context create_ssl_context() {
        boost::asio::ssl::context context(boost::asio::ssl::context::sslv23);
        context.set_default_verify_paths();
        return context;
    }

    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred, std::shared_ptr<std::vector<char>> buffer) {
        if (!error) {
            // Handle the read data in 'buffer'
            std::cout << "Received data: ";
            //std::cout.write(buffer->data(), bytes_transferred);
            std::cout << bytes_transferred;
            std::cout << std::endl;
        }
        else {
            std::cerr << "Error in async_read: " << error.message() << ":" << bytes_transferred << std::endl;
        }
    }

    void handle_head(std::shared_ptr<boost::asio::io_context> ioc, const boost::system::error_code& error, std::shared_ptr<std::string> headers, boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket, const std::string& host, const std::string& path) {
        if (!error) {
            // Find the Content-Length header to determine the file size
            std::size_t content_length_pos = headers->find("Content-Length:");
            if (content_length_pos != std::string::npos) {
                // Find the end of the line after "Content-Length:"
                content_length_pos = headers->find_first_of("0123456789", content_length_pos);
                std::size_t content_length_end = headers->find("\r\n", content_length_pos);

                // Extract and convert the content length to an integer
                std::size_t file_size = std::stoull(headers->substr(content_length_pos, content_length_end - content_length_pos));

                // Create a shared_ptr to the buffer with the appropriate size
                auto buffer = std::make_shared<std::vector<char>>(file_size);



                // Issue a GET request to retrieve the file content
                std::string get_request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
                std::cout << "REQUEST--------------------" << get_request << std::endl;
                boost::asio::async_write(socket, boost::asio::buffer(get_request), [ioc, buffer, &socket, host, path](const boost::system::error_code& write_error, std::size_t) {
                    std::cout << "getreq" << std::endl;
                    if (!write_error) {
                        // Start the asynchronous download
                        boost::asio::async_read(socket, boost::asio::buffer(*buffer), [buffer](const boost::system::error_code& read_error, std::size_t bytes_transferred) {
                            handle_read(read_error, bytes_transferred, buffer);
                            });
                    }
                    else {
                        std::cerr << "Error in async_write: " << write_error.message() << std::endl;
                    }
                    });
            }
            else {
                std::cerr << "Content-Length not found in headers." << std::endl;
            }
        }
        else {
            std::cerr << "Error in async_head: " << error.message() << std::endl;
        }
    }

    void start_async_download(std::shared_ptr<boost::asio::io_context> ioc, boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket, const std::string& host, const std::string& path, std::shared_ptr<std::string> headers) {
        // Create an HTTP HEAD request to retrieve headers and determine the file size
        std::cout << "start async" << std::endl;
        std::string request = "HEAD " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: keep - alive\r\n\r\n";

        // Start the asynchronous write of the HTTP request
        boost::asio::async_write(socket, boost::asio::buffer(request), [ioc, headers, &socket, host, path](const boost::system::error_code& write_error, std::size_t) {
            std::cout << "headreq" << std::endl;
            if (!write_error) {
                // Start the asynchronous read of the response headers
                boost::asio::async_read_until(socket, boost::asio::dynamic_buffer(*headers), "\r\n\r\n", [ioc, headers, &socket, host, path](const boost::system::error_code& read_error, std::size_t) {
                    handle_head(ioc, read_error, headers, socket, host, path);
                    });
            }
            else {
                std::cerr << "Error in async_write: " << write_error.message() << std::endl;
            }
            });
    }


    std::shared_ptr<void> HTTPLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc)
    {
        //Create ASIO Context
        //boost::asio::io_context ioc;
        //auto work = make_work_guard(ioc);

            //Parse hostname and path
        std::string http_host;
        std::string http_path;
        parseHTTPUrl(filename,http_host,http_path);

            //Get DNS result for hostname
        boost::asio::ip::tcp::resolver resolver(*ioc);
        boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(http_host, "https");
        //for (const auto& endpoint : results) {
        //    std::cout << "Resolved endpoint: " << endpoint.endpoint() << std::endl;
        //}
        boost::asio::ip::tcp::endpoint endpoint = *results.begin();
            //Create SSL Context
        //boost::asio::ssl::context ssl_context = create_ssl_context();

        auto ssl_context = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        ssl_context->set_default_verify_paths();
        //boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(*ioc, ssl_context);
        auto socket = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ssl_context);

            //Connect socket
        socket->lowest_layer().connect(endpoint);

        socket->async_handshake(boost::asio::ssl::stream_base::client, [ioc, socket, ssl_context, http_host, http_path](const boost::system::error_code& handshake_error) {
            std::cout << "handshake" << std::endl;
            if (!handshake_error) {
                // Create a shared pointer to a string buffer for the response headers
                auto headers = std::make_shared<std::string>();
               
                // Start the asynchronous download for a specific path
                start_async_download(ioc, *socket, http_host, http_path, headers);
                std::cout << "inhere" << std::endl;
                // Run the IO service to start asynchronous operations
                //work.reset();
                //ioc.run();
            }
            else {
                std::cerr << "Handshake error: " << handshake_error.message() << std::endl;
            }
            });

        //work.reset();
        //ioc.run();
        std::cout << "return?" << std::endl;
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
