#include <sstream>
#include "../include/MNNStream.hpp"

namespace sgns::io
{

    MNNStream::MNNStream(unsigned int num_thread) : m_num_threads(num_thread)
    {

    } // End constructor MNNstream

    MNNStream::~MNNStream()
    {
        // Release model
        if (m_mnn_interpreter)
        {
            m_mnn_interpreter->releaseModel();
        }
        // Release all session
        if (m_mnn_session)
        {
            m_mnn_interpreter->releaseSession(m_mnn_session);
        }
    } // End ~MNNStream

    /*
     * Read from the disk file
     */
    size_t MNNStream::read(const char* path, size_t bytes)
    {
        m_mnn_interpreter = std::shared_ptr<MNN::Interpreter>(
                MNN::Interpreter::createFromFile(path));
        if (m_mnn_interpreter == nullptr)
        {
            return 0;
        }
	return load_(m_mnn_interpreter);
    }	    

    size_t MNNStream::write(const char *buf, size_t bytes) {
        m_mnn_interpreter = std::shared_ptr<MNN::Interpreter>(
                MNN::Interpreter::createFromBuffer(buf, bytes));
        if (m_mnn_interpreter == nullptr)
        {
            return 0;
        }
	return load_(m_mnn_interpreter);
    }

    size_t MNNStream::load_(std::shared_ptr<MNN::Interpreter> interpreter)
    {
        // Initialize the interpreter
        // Schedule config
        m_schedule_config.numThread = (int) m_num_threads;
        MNN::BackendConfig backend_config;
        backend_config.precision = MNN::BackendConfig::Precision_High;
        // Create session for reading model
        m_mnn_session = interpreter->createSession(m_schedule_config);
        // Tensor input and input dims
        m_input_tensor = interpreter->getSessionInput(m_mnn_session,
                nullptr);
        m_input_batch = m_input_tensor->batch();
        m_input_channel = m_input_tensor->channel();
        m_input_height = m_input_tensor->height();
        m_input_width = m_input_tensor->width();
        m_dimension_type = m_input_tensor->getDimensionType();
        // Adapt interpreter base on dims
        switch (m_dimension_type)
        {
            // caffe net type
            case MNN::Tensor::CAFFE:
                interpreter->resizeTensor(m_input_tensor, {
                        m_input_channel, m_input_height, m_input_width });
                interpreter->resizeSession(m_mnn_session);
                break;
                // Tensorflow net type
            case MNN::Tensor::TENSORFLOW:
                interpreter->resizeTensor(m_input_tensor, {
                        m_input_batch, m_input_height, m_input_width,
                        m_input_channel });
                interpreter->releaseSession(m_mnn_session);
                break;
                //C4HW4 as data format
            case MNN::Tensor::CAFFE_C4:
                break;
                // Any new type MNN support, we can adapt it later
            default:
                break;
        }
        m_num_output =
                interpreter->getSessionOutputAll(m_mnn_session).size();

	return m_num_output;
    } // End load

    std::string MNNStream::get_info()
    {
        std::ostringstream output;
        if (m_mnn_interpreter)
        {
            output << "==================INPUT-DIMS================"
                    << std::endl;
            if (m_input_tensor)
            {
                m_input_tensor->printShape();
                // TODO: How to get info to the output stream?????
            }
            switch (m_dimension_type)
            {
                case MNN::Tensor::CAFFE:
                    output
                            << "Dimension Type: (CAFE/PyTorch/ONNX) uses NCHW as data format"
                            << std::endl;
                    break;
                case MNN::Tensor::TENSORFLOW:
                    output
                            << "Dimension Type: (TENSORFLOW) uses NHWC as data format"
                            << std::endl;
                    break;
                case MNN::Tensor::CAFFE_C4:
                    output
                            << "Dimension Type: (CAFE_C4) uses NC4HW4 as data format"
                            << std::endl;
                    break;
                default:
                    output << "Dimension Type: UNKNOWN" << std::endl;
                    break;
            }
            output << "==================OUTPUT-DIMS================"
                    << std::endl;
            auto output_map = m_mnn_interpreter->getSessionOutputAll(
                    m_mnn_session);
            for (auto it = output_map.cbegin(); it != output_map.cend(); it++)
            {
                output << "Output : " << it->first << std::endl;
                it->second->printShape();
            }
            output << "============================================="
                    << std::endl;
        }
        return output.str();
    } // End print_info()
} // End sgns namespace
 


