/**
 * Header file for the MNNLoader
 */
#ifndef SGNS_MNNSTREAM_HPP
#define SGNS_MNNSTREAM_HPP

#include <memory>
#include <string>

#include <MNN/MNNDefine.h>
#include <MNN/Interpreter.hpp>
#include <MNN/Tensor.hpp>

#include "FileStream.hpp"

namespace sgns::io
{

    class MNNStream : public FileStream
    {
        public:
            /**
             * Constructor function
             * @param num_thread - Number of threads want to use.
             */
            MNNStream(unsigned int num_thread = 1);		
            /**
             * Destruction function
             */
            virtual ~MNNStream();

	    /**
	     * IO Operations
	     */
	    size_t read(const char*, size_t bytes) override;
            size_t write(const char *buf, size_t bytes) override;

	    // Async operations
            // void readAsync(char*, size_t bytes, OnGetCompleteCallback handler);
            // size_t writeAsync(char*, size_t bytes, OnWriteCompleteCallback handler);

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
            MNN::Tensor::DimensionType m_dimension_type;
            const char *m_log_id = nullptr;
            const char *m_mnn_file_path = nullptr;
            unsigned int m_num_threads; /* Configuration for multi-threading input */
            int m_input_batch;
            int m_input_channel;
            int m_input_height;
            int m_input_width;
            int m_num_output;
        private:
            MNNStream(const MNNStream&) = delete;
            MNNStream(MNNStream&&) = delete;
            MNNStream& operator=(const MNNStream&) = delete;
            MNNStream& operator=(MNNStream&&) = delete;

	    size_t load_(std::shared_ptr<MNN::Interpreter> interpreter);

    };

} // End namespace sgns

#endif /* SGNS_MNNSTREAM_HPP */
