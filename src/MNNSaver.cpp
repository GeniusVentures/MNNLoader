/*
 * MNNSaver.cpp
 */

#include <iostream>
#include <fstream>
#include <streambuf>
#include "FileManager.hpp"
#include "MNNSaver.hpp"
namespace sgns
{
    SINGLETON_PTR_INIT(MNNSaver);

    MNNSaver::MNNSaver()
    {
        FileManager::GetInstance().RegisterSaver("file", this);
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

} // End namespace sgns
