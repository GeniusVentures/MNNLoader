#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include "FileManager.hpp"
#include "WSLoader.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "URLStringUtil.h"


namespace sgns
{
    SINGLETON_PTR_INIT(WSLoader);
    WSLoader::WSLoader()
    {
        FileManager::GetInstance().RegisterLoader("wss", this);
        FileManager::GetInstance().RegisterLoader("ws", this);
    }

    std::shared_ptr<void> WSLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the WSLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }

    void handle_websocket_read(const boost::system::error_code& error, std::size_t bytes_transferred, std::shared_ptr<std::vector<char>> buffer) {
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

    std::shared_ptr<void> WSLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc)
    {
        //Parse hostname and path
        std::string ws_host;
        std::string ws_path;
        parseHTTPUrl(filename, ws_host, ws_path);
        std::cout << "host " << ws_host << std::endl;
        std::cout << "path " << ws_path << std::endl;

        //Create Socket
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws(*ioc);
        //auto ws = std::make_shared<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(*ioc);

        // Resolve the host and port
        boost::asio::ip::tcp::resolver resolver(*ioc);
        auto const results = resolver.resolve(ws_host, "80");
        std::cout << "Resolved Endpoints:" << std::endl;
        for (const auto& endpoint : results) {
            std::cout << endpoint.endpoint() << std::endl;
        }
        // Connect to the WebSocket server
        boost::asio::async_connect(ws.next_layer(), results, [ioc, &ws, ws_host, ws_path](const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint&) {
            if (!ec) {
                // Perform the WebSocket handshake
                ws.async_handshake(ws_host,ws_path, [ioc, &ws](const boost::system::error_code& ec) {
                    if (!ec) {
                        // Create a shared buffer for each WebSocket operation
                        auto buffer = std::make_shared<std::vector<char>>(1024);

                        // Start the asynchronous WebSocket operation (e.g., reading from the WebSocket)
                        ws.async_read(boost::asio::dynamic_buffer(*buffer), [ioc, &ws, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
                            std::cout << "Websocket Read" << std::endl;
                            handle_websocket_read(error, bytes_transferred, buffer);

                            // Continue reading or perform cleanup
                            });
                    }
                    else {
                        std::cerr << "WebSocket handshake error: " << ec.message() << std::endl;
                    }
                    });
            }
            else {
                std::cerr << "Connect error: " << ec.message() << std::endl;
            }
            });
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
