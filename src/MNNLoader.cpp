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

    std::shared_ptr<void> MNNLoader::LoadASync(std::string filename,bool parse,bool save,std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read, StatusCallback status)
    {
        std::shared_ptr<string> result = std::make_shared < string>("init");

        auto file = std::make_shared<boost::asio::stream_file>(*ioc);
        //Make a stream_file which should work multi-platform
        try {
            file->open(filename, boost::asio::stream_file::flags::read_only);
        }
        catch(const boost::system::system_error& er){
            std::cerr << "Error: " << er.what() << std::endl;
            status(-12);
            handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
            return result;
        }

        //Make a Buffer
        auto buffer = std::make_shared<boost::asio::streambuf>();
        status(7);
        //Async Read.
        boost::asio::async_read(*file,
            *buffer,
            boost::asio::transfer_all(),
            [file, ioc, handle_read, status, parse, save, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
                //if (!error)
                //{
                    std::cout << "LOCAL Finish" << std::endl;
                    auto finalbuffer = std::make_shared<std::vector<char>>(boost::asio::buffers_begin(buffer->data()), boost::asio::buffers_end(buffer->data()));
                    status(0);
                    handle_read(ioc, finalbuffer, parse, save);
                //}
                //else {
                    //std::cerr << "File read error: " << error.message() << std::endl;
                    //status(-7);
                    //handle_read(ioc, std::make_shared<std::vector<char>>(), false, false);
                //}
            });
       
        return result;
    }

} // End namespace sgns
