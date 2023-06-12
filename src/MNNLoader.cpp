#include <sstream>
#include "FileManager.hpp"
#include "MNNLoader.hpp"

namespace sgns
{
    SINGLETON_PTR_INIT(MNNLoader);
    MNNLoader::MNNLoader()
    {
        FileManager::GetInstance().RegisterLoader("file:", this);
    }

    std::shared_ptr<void> MNNLoader::LoadFile(std::string filename)
    {
        m_log_id = filename.data();
        m_mnn_file_path = filename.data();
        // Initialize the interpreter
        m_mnn_interpreter = std::shared_ptr<MNN::Interpreter>(
                MNN::Interpreter::createFromFile(m_mnn_file_path));
        return m_mnn_interpreter;
    }

} // End namespace sgns
