#include <sstream>
#include "MNNLoader.hpp"

namespace sgns
{

  MNNLoader::MNNLoader(const std::string &mnn_file_name, unsigned int num_thread) : log_id_(mnn_file_name.data()), mnn_file_path_(
      mnn_file_name.data()), num_threads_(num_thread)
  {
    initialize();
  } // End constructor MNNLoader

  MNNLoader::~MNNLoader()
  {
    // Release model
    if (mnn_interpreter_)
    {
      mnn_interpreter_->releaseModel();
    }
    // Release all session
    if (mnn_session_)
    {
      mnn_interpreter_->releaseSession(mnn_session_);
    }
  } // End ~MNNLoader

  void MNNLoader::initialize()
  {
    // Initialize the interpreter
    mnn_interpreter_ = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(mnn_file_path_));
    if (mnn_interpreter_ == nullptr)
    {
      return;
      // TODO: Need to print out system error. SPDLOG for writing syslog
    }

    // Schedule config
    schedule_config_.numThread = (int) num_threads_;
    MNN::BackendConfig backend_config;
    backend_config.precision = MNN::BackendConfig::Precision_High;
    schedule_config_.backendConfig = &backend_config;
    // Create session for reading model
    mnn_session_ = mnn_interpreter_->createSession(schedule_config_);
    // Tensor input and input dims
    input_tensor_ = mnn_interpreter_->getSessionInput(mnn_session_, nullptr);
    input_batch_ = input_tensor_->batch();
    input_channel_ = input_tensor_->channel();
    input_height_ = input_tensor_->height();
    input_width_ = input_tensor_->width();
    dimension_type_ = input_tensor_->getDimensionType();
    // Adapt interpreter base on dims
    switch (dimension_type_)
    {
      case MNN::Tensor::CAFFE:
        // caffe net type
        mnn_interpreter_->resizeTensor(input_tensor_, { input_channel_, input_height_, input_width_ });
        mnn_interpreter_->resizeSession(mnn_session_);
        break;
      case MNN::Tensor::TENSORFLOW:
        // tensorflow net type
        mnn_interpreter_->resizeTensor(input_tensor_, { input_batch_, input_height_, input_width_, input_channel_ });
        mnn_interpreter_->releaseSession(mnn_session_);
        break;
      case MNN::Tensor::CAFFE_C4:
        //C4HW4 as data format
        break;
        // Any new type MNN support, we can adapt it later
      default:
        break;
    }
    num_output_ = mnn_interpreter_->getSessionOutputAll(mnn_session_).size();
  } // End initialize()

  std::string MNNLoader::get_info()
  {
    std::ostringstream output;
    if (mnn_interpreter_)
    {
      output << "Log ID: " << log_id_ << std::endl;
      output << "==================INPUT-DIMS================" << std::endl;
      if (input_tensor_)
      {
        input_tensor_->printShape();
        // TODO: How to get info to the output stream?????
      }
      switch (dimension_type_)
      {
        case MNN::Tensor::CAFFE:
          output << "Dimension Type: (CAFE/PyTorch/ONNX) uses NCHW as data format" << std::endl;
          break;
        case MNN::Tensor::TENSORFLOW:
          output << "Dimension Type: (TENSORFLOW) uses NHWC as data format" << std::endl;
          break;
        case MNN::Tensor::CAFFE_C4:
          output << "Dimension Type: (CAFE_C4) uses NC4HW4 as data format" << std::endl;
          break;
        default:
          output << "Dimension Type: UNKNOWN" << std::endl;
          break;
      }
      output << "==================OUTPUT-DIMS================" << std::endl;
      auto output_map = mnn_interpreter_->getSessionOutputAll(mnn_session_);
      for (auto it = output_map.cbegin(); it != output_map.cend(); it++)
      {
        output << "Output : " << it->first << std::endl;
        it->second->printShape();
      }
      output << "=============================================" << std::endl;
    }
    return output.str();
  } // End print_info()
} // End sgns namespace

