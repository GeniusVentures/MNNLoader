
#ifndef INCLUDE_MNNLOADER_HPP_
#define INCLUDE_MNNLOADER_HPP_

#include <memory>
#include <string>

#include "MNNCommon.hpp"

namespace sgns
{

  /*
   * This is class parsing all information of MNN model file.
   * If you want to use this class, we can inheritance from this class
   * and implement logic base on model info
   */
  class MNNLoader {
    public:
      MNNLoader(const std::string &mnn_file_name, unsigned int num_thread = 1);
      virtual ~MNNLoader();
      std::string get_info();
    protected:
      // MNN information
      std::shared_ptr<MNN::Interpreter> mnn_interpreter_; // The holder of the model data
      MNN::Session *mnn_session_ = nullptr; // The holder of inference data
      MNN::Tensor *input_tensor_ = nullptr;
      MNN::ScheduleConfig schedule_config_;
      const char *log_id_ = nullptr;
      const char *mnn_file_path_ = nullptr;
      // Configuration for multi threading input
      unsigned int num_threads_;
      int input_batch_;
      int input_channel_;
      int input_height_;
      int input_width_;
      MNN::Tensor::DimensionType dimension_type_;
      int num_output_;
    private:
      // Non copy
      MNNLoader(const MNNLoader&) = delete;
      MNNLoader(MNNLoader&&) = delete;
      MNNLoader& operator=(const MNNLoader&) = delete;
      MNNLoader& operator=(MNNLoader&&) = delete;
      // Helper function
      void initialize();
      // TOTO: Consider spdloger for print any log unexpected.
  };

} // End namespace sgns





#endif /* INCLUDE_MNNLOADER_HPP_ */
