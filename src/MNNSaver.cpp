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
#include "FILECommon.hpp"

namespace sgns
{
    MNNSaver* MNNSaver::_instance = nullptr;
    void MNNSaver::InitializeSingleton() {
        if (_instance == nullptr) {
            _instance = new MNNSaver();
        }
    }
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


    void MNNSaver::SaveASync(std::shared_ptr<boost::asio::io_context> ioc, 
        std::function<void(std::shared_ptr<boost::asio::io_context> ioc)> handle_write,
        std::string filename, std::shared_ptr<std::vector<char>> data, std::string suffix)
    {
        if (data == nullptr)
        {
            throw range_error("Can not save with null data");
        }
        if (filename.empty()) {
            filename = boost::lexical_cast<string>((boost::uuids::random_generator())()) + "." + suffix;
        }

        std::ofstream file(filename, std::ios::binary);

        auto fileDevice = std::make_shared<FILEDevice>(ioc, filename, 1);

        async_write(fileDevice->getFile(), boost::asio::buffer(data->data(), data->size()), boost::asio::transfer_exactly(data->size()), [fileDevice, ioc, handle_write, data](const boost::system::error_code& error, std::size_t bytes_transferred)
        {
                std::cout << "wrote" << std::endl;
                handle_write(ioc);
        });
    }

} // End namespace sgns
