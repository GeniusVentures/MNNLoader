#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include "FileManager.hpp"
#include "WSLoader.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast/websocket/ssl.hpp"
#include "boost/beast.hpp"
#include "URLStringUtil.h"


namespace sgns
{
    SINGLETON_PTR_INIT(WSLoader);
    WSLoader::WSLoader()
    {
        FileManager::GetInstance().RegisterLoader("wss", this);
        //FileManager::GetInstance().RegisterLoader("ws", this);
    }

    std::shared_ptr<void> WSLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the WSLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }

    void StreamWS(std::shared_ptr<boost::asio::io_context> ioc,
        std::shared_ptr<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>> ws,
        std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse)> handle_read,
        bool parse,
        const std::string& host, const std::string& path) {
        // Perform the WebSocket asynchronous handshake
        ws->async_handshake(host, path, [ioc, ws, handle_read, parse](const boost::system::error_code& handshakeError) {
            if (!handshakeError) {
                //Request the file
                std::string request = "GET_FILE";
                ws->async_write(boost::asio::buffer(request), [ioc, ws, handle_read, parse](const boost::system::error_code& write_error, std::size_t bytes_transferred) {
                    if (!write_error) {
                        //Read until WSEOF
                        auto buffer = std::make_shared<boost::asio::streambuf>();
                        boost::asio::async_read_until(*ws, *buffer, "WSEOF", [ioc, ws, handle_read, parse, buffer](const boost::system::error_code& read_error, std::size_t bytes_transferred) {
                            if (!read_error)
                            {
                                auto outbuf = std::make_shared<std::vector<char>>(boost::asio::buffers_begin(buffer->data()), boost::asio::buffers_end(buffer->data()) - 5);
                                handle_read(ioc, outbuf,parse);
                            }
                            else {
                                std::cerr << "File request read error: " << read_error.message() << std::endl;
                            }
                            });
                    }
                    else {
                        std::cerr << "File request write error: " << write_error.message() << std::endl;
                    }
                    });
            }
            else {
                std::cerr << "WebSocket handshake error: " << handshakeError.message() << std::endl;
            }
            });
    }

    std::shared_ptr<void> WSLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read)
    {
        //Parse hostname and path
        std::string ws_host;
        std::string ws_path;
        std::string ws_port;
        parseHTTPUrl(filename, ws_host, ws_path, ws_port);
        std::cout << "host " << ws_host << std::endl;
        std::cout << "path " << ws_path << std::endl;
        std::cout << "port " << ws_port << std::endl;
        //Resolve Address
        boost::asio::ip::tcp::resolver resolver(*ioc);
        auto const results = resolver.resolve(ws_host, ws_port);

        //Create SSL Context, using context::tls to accept the highest version client/server can deal with
        auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls);

        //Disclude certain older insecure options
        ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3);

        //Default trusted authority definitions
        ctx->set_default_verify_paths();

        //Consider setting verify callback to check whether domain name matches cert
        // ctx->set_verify_callback(...);

        //Create Socket
        auto ws = std::make_shared<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>>(*ioc, *ctx);

        //Connect to server
        boost::asio::async_connect(ws->next_layer().next_layer(), results.begin(), results.end(), [ioc, ws, handle_read, parse, ws_host, ws_path](const boost::system::error_code& error, const auto&) {
            if (!error) {
                // Perform the SSL asynchronous handshake
                ws->next_layer().async_handshake(boost::asio::ssl::stream_base::client, [ioc, ws, handle_read, parse, ws_host, ws_path](const boost::system::error_code& handshakeError) {
                    if (!handshakeError) {
                        // Perform the WebSocket asynchronous handshake
                        StreamWS(ioc,ws,handle_read,parse,ws_host,ws_path);
                    }
                    else {
                        std::cerr << "SSL handshake error: " << handshakeError.message() << std::endl;
                    }
                    });
            }
            else {
                std::cerr << "Connect error: " << error.message() << std::endl;
            }
            });
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
