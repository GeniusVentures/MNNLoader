
#include "FileManager.hpp"
#include "HTTPLoader.hpp"
#include "HTTPCommon.hpp"


namespace sgns
{
    HTTPLoader* HTTPLoader::_instance = nullptr;
    void HTTPLoader::InitializeSingleton(std::shared_ptr<boost::asio::io_context> ioc) {
        if (_instance == nullptr) {
            _instance = new HTTPLoader(ioc);  
        }
    }
    HTTPLoader::HTTPLoader(std::shared_ptr<boost::asio::io_context> ioc) 
    : ssl_context_(std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls)), ioc_(ioc)
    {
        //FileManager::GetInstance().RegisterLoader("http", this);
        FileManager::GetInstance().RegisterLoader("https", this);
        ssl_context_->set_options(
        boost::asio::ssl::context::default_workarounds | 
        boost::asio::ssl::context::no_sslv2 | 
        boost::asio::ssl::context::no_sslv3);
        ssl_context_->set_default_verify_paths();
    }

    std::shared_ptr<void> HTTPLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the HTTPLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }


    std::shared_ptr<void> HTTPLoader::LoadASync(std::string filename, bool parse, bool save, CompletionCallback handle_read, StatusCallback status)
    {
        //Parse hostname and path
        std::string http_host;
        std::string http_path;
        std::string http_port;
        parseHTTPUrl(filename, http_host, http_path, http_port);

        auto httpDevice = std::make_shared<HTTPDevice>(http_host, http_path, http_port, parse, save);
        httpDevice->StartHTTPDownload(ioc_, ssl_context_, handle_read, status);
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
