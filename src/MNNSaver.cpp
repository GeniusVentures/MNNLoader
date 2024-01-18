/*
 * MNNSaver.cpp
 */

#include <iostream>
#include <fstream>
#include <streambuf>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "FileManager.hpp"
#include "MNNSaver.hpp"

namespace sgns
{
    SINGLETON_PTR_INIT(MNNSaver);

    MNNSaver::MNNSaver()
    {
        FileManager::GetInstance().RegisterSaver("file", this);
        FileManager::GetInstance().RegisterSaver("mnn", this);
    }

    void MNNSaver::SaveFile(std::string filename, std::shared_ptr<void> data)
    {
        if (data == nullptr)
        {
            throw range_error("Can not save with null data");
        }
        std::shared_ptr<string> fileContent =
                std::static_pointer_cast<string>(data);
        ofstream outputFile(filename, std::ios_base::binary);
        if (!outputFile.is_open())
        {
            throw range_error("Can not create file for save");
        }
        outputFile.write(fileContent.get()->c_str(),
                fileContent.get()->size());
        outputFile.close();
    }

    //void handle_write()
    //{
    //    std::cout << "handle write" << std::endl;
    //}


    void MNNSaver::SaveASync(std::shared_ptr<boost::asio::io_context> ioc, 
        std::function<void(std::shared_ptr<boost::asio::io_context> ioc)> handle_write,
        std::string filename, std::shared_ptr<std::vector<char>> data)
    {
        if (data == nullptr)
        {
            throw range_error("Can not save with null data");
        }
        if (filename.empty()) {
            filename = boost::lexical_cast<string>((boost::uuids::random_generator())()) + ".mnn";
        }

        std::ofstream file(filename, std::ios::binary);
//#if defined(BOOST_ASIO_WINDOWS)
        auto file_stream = std::make_shared<boost::asio::stream_file>(*ioc, filename, boost::asio::stream_file::flags::write_only);
//#else
//        auto file_stream = std::make_shared<boost::asio::posix::stream_descriptor>(*ioc);
//        int fileDescriptor = open(filename, O_RDONLY);
//        file_stream->assign(fileDescriptor);
//#endif
        async_write(*file_stream, boost::asio::buffer(data->data(), data->size()), boost::asio::transfer_exactly(data->size()), [ioc, handle_write, file_stream, data](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
                handle_write(ioc);
        });
    }

} // End namespace sgns
