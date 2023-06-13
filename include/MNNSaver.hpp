/*
 * MNNSaver.hpp
 */

#ifndef INCLUDE_MNNSAVER_HPP_
#define INCLUDE_MNNSAVER_HPP_

#include "FileSaver.hpp"
#include "Singleton.hpp"

namespace sgns
{
/// @brief class to handle "ipfs://" prefix in a filename to save to ipfs
    class MNNSaver: public FileSaver
    {
        SINGLETON_PTR(MNNSaver)
            ;
        public:
            /// @brief save a file to ipfs, throws on error
            /// @param filename filename to save the file as
            virtual void SaveFile(std::string filename,
                    std::shared_ptr<void> data) override;

    };
} // End namespace sgns

#endif /* INCLUDE_MNNSAVER_HPP_ */
