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
        //FileManager::GetInstance().RegisterLoader("http", this);
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

    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred, std::vector<char> buffer) {
        std::cerr << "async_read result: " << error.message() << ":" << bytes_transferred << std::endl;
        //TODO: If not EOF, do something else
        std::ofstream file("httpoutput.txt", std::ios::binary);
        file.write(buffer.data(), buffer.size());
        file.close();
    }

    void start_async_download(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket, std::shared_ptr<boost::asio::ssl::context> ssl_context, const std::string& host, const std::string& path) {
        std::string get_request = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
        boost::asio::async_write(*socket, boost::asio::buffer(get_request), [ioc, socket, ssl_context, host, path](const boost::system::error_code& write_error, std::size_t) {
            if (!write_error) {
                auto headerbuff = std::make_shared<boost::asio::streambuf>();
                boost::asio::async_read(*socket, *headerbuff, boost::asio::transfer_all(), [headerbuff, socket, ssl_context](const boost::system::error_code& read_error, std::size_t bytes_transferred) {
                    //Make a vector buffer from data
                    auto buffer = std::make_shared<std::vector<char>>(boost::asio::buffers_begin(headerbuff->data()), boost::asio::buffers_end(headerbuff->data()));
                    //Copy to a string
                    std::string bufferStr(buffer->begin(), buffer->end());
                    //Find end of header 
                    size_t headerEnd = bufferStr.find("\r\n\r\n");
                    //Check if we found an end
                    if (headerEnd != std::string::npos) {
                        //Create vector of binary data by cutting off the header.
                        std::vector<char> binaryData(buffer->begin() + headerEnd + 4, buffer->end());
                        //Send this to handler to be processed.
                        handle_read(read_error, bytes_transferred, binaryData);
                    }
                    else {
                        std::cerr << "Data does not contain header" << std::endl;
                    }
                    });
                }
            else {
                std::cerr << "Error in async_write: " << write_error.message() << std::endl;
            }
            });
    }
    std::shared_ptr<void> HTTPLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc)
    {
            //Parse hostname and path
        std::string http_host;
        std::string http_path;
        parseHTTPUrl(filename,http_host,http_path);

            //Get DNS result for hostname
        boost::asio::ip::tcp::resolver resolver(*ioc);
        boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(http_host, "https");
        boost::asio::ip::tcp::endpoint endpoint = *results.begin();
            //Create SSL Context
        //boost::asio::ssl::context ssl_context = create_ssl_context();

        auto ssl_context = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        ssl_context->set_default_verify_paths();
        //boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket(*ioc, ssl_context);
        auto socket = std::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ssl_context);

            //Connect socket
        //socket->lowest_layer().connect(endpoint);
        socket->lowest_layer().connect(endpoint);

        socket->async_handshake(boost::asio::ssl::stream_base::client, [ioc, socket, ssl_context, http_host, http_path](const boost::system::error_code& handshake_error) {
            if (!handshake_error) {
                // Create a shared pointer to a string buffer for the response headers
                //auto headers = std::make_shared<std::string>();
               
                // Start the asynchronous download for a specific path
                start_async_download(ioc, socket, ssl_context, http_host, http_path);

            }
            else {
                std::cerr << "Handshake error: " << handshake_error.message() << std::endl;
            }
            });

        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
