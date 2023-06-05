/**
 * Header file for the MNNLoader
 */

#ifndef INCLUDE_MNNLOADER_HPP_
#define INCLUDE_MNNLOADER_HPP_

#include <memory>
#include <string>

#include "MNNCommon.hpp"

namespace sgns
{

    /**
     * This class is for parsing the information in an MNN model file.
     * If you want to use this class, we can inheritance from this class
     * and implement logic based on model info
     */
    class MNNLoader
    {
        public:
            /**
             * Constructor function
             * @param mnn_file_name - The path of the mnn file
             * @param num_thread - Number of threads want to use.
             */
            MNNLoader(const std::string &mnn_file_name,
                    unsigned int num_thread = 1);

            /**
             * Destruction function
             */
            virtual ~MNNLoader();

            /**
             * Get information on the MNN file
             * @return information on the MNN file
             */
            std::string get_info();
        protected:
            std::shared_ptr<MNN::Interpreter> m_mnn_interpreter; /* The holder of the model data */
            MNN::Session *m_mnn_session = nullptr; /* The holder of inference data */
            MNN::Tensor *m_input_tensor = nullptr;
            MNN::ScheduleConfig m_schedule_config;
            const char *m_log_id = nullptr;
            const char *m_mnn_file_path = nullptr;
            unsigned int m_num_threads; /* Configuration for multi-threading input */
            int m_input_batch;
            int m_input_channel;
            int m_input_height;
            int m_input_width;
            MNN::Tensor::DimensionType m_dimension_type;
            int m_num_output;
        private:
            MNNLoader(const MNNLoader&) = delete;
            MNNLoader(MNNLoader&&) = delete;
            MNNLoader& operator=(const MNNLoader&) = delete;
            MNNLoader& operator=(MNNLoader&&) = delete;

            /**
             * Helper function using for initialize data
             */
            void initialize();
    };

} // End namespace sgns

#endif /* INCLUDE_MNNLOADER_HPP_ */
