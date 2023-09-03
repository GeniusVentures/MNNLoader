#include "../include/IPFSStream.hpp"

namespace sgns::io {

        IPFSStream::IPFSStream(std::shared_ptr<IpfsBlockService> block_service)
            : block_service_(std::move(block_service)), current_read_pos_(0), current_write_pos_(0) {}

        std::streamsize IPFSStream::read(char* s, std::streamsize n) {
            std::lock_guard<std::mutex> lock(mtx_);
            
            if (read_buffer_.empty() && !cid_queue_.empty()) {
                auto res = block_service_->get(cid_queue_.front());
                if (res.has_error()) {
                    return -1;  // Error
                }

                read_buffer_ = res.value().toVector();
                cid_queue_.pop();
                current_read_pos_ = 0;
            }

            auto available_bytes = read_buffer_.size() - current_read_pos_;
            auto bytes_to_read = std::min(n, static_cast<std::streamsize>(available_bytes));

            std::copy(read_buffer_.begin() + current_read_pos_,
                    read_buffer_.begin() + current_read_pos_ + bytes_to_read,
                    s);

            current_read_pos_ += bytes_to_read;

            return bytes_to_read;
        }

        std::streamsize IPFSStream::write(const char* s, std::streamsize n) {
            std::lock_guard<std::mutex> lock(mtx_);

            write_buffer_.insert(write_buffer_.end(), s, s + n);
            current_write_pos_ += n;

            if (write_buffer_.size() >= SOME_CHUNK_SIZE) {  // Replace SOME_CHUNK_SIZE with desired chunk size
                auto cid = CID::generate(write_buffer_);
                auto res = block_service_->set(cid, common::Buffer(write_buffer_));
                
                if (res.has_error()) {
                    return 0;  // Error
                }

                cid_queue_.push(cid);
                write_buffer_.clear();
                current_write_pos_ = 0;
            }

            return n;
        }
    
    }  // namespace sgns::ipfs_lite::ipfs