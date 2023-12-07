#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include "FileManager.hpp"
#include "MNNLoader.hpp"

namespace sgns
{
    SINGLETON_PTR_INIT(MNNLoader);
    MNNLoader::MNNLoader()
    {
        FileManager::GetInstance().RegisterLoader("file", this);
    }

    std::shared_ptr<void> MNNLoader::LoadFile(std::string filename)
    {
        if (!std::filesystem::exists(filename))
        {
            throw std::range_error("File was not exist in system");
        }
        std::ifstream inputFile(filename, std::ios_base::binary);
        if (!inputFile.is_open())
        {
            throw std::range_error("Can not open file");
        }
        // Read all file to string
        std::string dataContent((std::istreambuf_iterator<char>(inputFile)),
                std::istreambuf_iterator<char>());
        inputFile.close();
        std::shared_ptr<string> result = std::make_shared<string>(
                dataContent);
        return result;
    }
    bool callback(boost::system::error_code ec, std::size_t n, std::vector<char>& buffer)
    {
        if (!ec) {
            std::cout << "Received data: ";
            std::cout << std::endl;
            std::cout << "Handler Out Size:" << n << std::endl;
        }
        else {
            std::cerr << "Error in async_read: " << ec.message() << std::endl;
        }
        return true;
    }
    std::shared_ptr<void> MNNLoader::LoadASync(std::string filename,bool parse)
    {
        //Create ASIO Context
        boost::asio::io_context ioc;
        auto work = make_work_guard(ioc);
        //Define Streamed Files
        boost::asio::stream_file file(ioc, filename, boost::asio::stream_file::flags::read_only);
        //Make a Buffer
        std::vector<char> buffer(file.size());
        //Async Read.
        boost::asio::async_read(file,
            boost::asio::buffer(buffer),
            boost::asio::transfer_exactly(buffer.size()),
            [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
                callback(error, bytes_transferred, buffer);
            });
        work.reset();
        ioc.run();
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
