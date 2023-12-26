#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include "FileManager.hpp"
#include "SFTPLoader.hpp"
#include "boost/asio/ssl.hpp"
#include "URLStringUtil.h"
#include "libssh2.h"
#include "libssh2_sftp.h"

#ifdef _WIN32
    // Windows platform
    using StreamDescriptor = boost::asio::windows::stream_handle;
#else
    // Assume POSIX-like platform
    using StreamDescriptor = boost::asio::posix::stream_descriptor;
#endif

namespace sgns
{
    SINGLETON_PTR_INIT(SFTPTLoader);
    SFTPTLoader::SFTPTLoader()
    {
        FileManager::GetInstance().RegisterLoader("sftp", this);
    }

    std::shared_ptr<void> SFTPTLoader::LoadFile(std::string filename)
    {
        const char* dummyValue = "Inside the SFTPTLoader::LoadFile Function";
        // for this test, we don't need to delete the shared_ptr as the data is static, so pass null lambda delete function
        return { (void*)dummyValue, [](void*) {} };
        /* TODO: scorpioluck20 - Need to implement this. How we load file base on format file?*/
    }

    void handle_read2(const boost::system::error_code& error, std::size_t bytes_transferred, std::shared_ptr<std::vector<char>> buffer) {
        if (!error) {
            // Handle the read data in 'buffer'
            std::cout << "Received data: ";
            //std::cout.write(buffer->data(), bytes_transferred);
            std::cout << bytes_transferred;
            std::cout << std::endl;
        }
        else {
            std::cerr << "Error in async_read: " << error.message() << ":" << bytes_transferred << std::endl;
        }
    }

    std::shared_ptr<void> SFTPTLoader::LoadASync(std::string filename, bool parse, std::shared_ptr<boost::asio::io_context> ioc, CompletionCallback handle_read)
    {
            //Parse hostname and path
        std::string sftp_host;
        std::string sftp_path;
        std::string sftp_user;
        std::string sftp_pass;
        parseSFTPUrl(filename, sftp_host, sftp_path, sftp_user, sftp_pass);
        std::cout << "host " << sftp_host << std::endl;
        std::cout << "path " << sftp_path << std::endl;
        std::cout << "user " << sftp_user << std::endl;
        std::cout << "pass " << sftp_pass << std::endl;
        //// Initialize libssh2
        //libssh2_init(0);

        //// Create an SSH session
        //LIBSSH2_SESSION* session = libssh2_session_init();

        //// Connect to the SFTP server
        //libssh2_socket_t sock;
        //sock = boost::asio::ip::tcp::socket(*ioc).native_handle();
        //libssh2_session_startup(session, sock);
        //libssh2_session_set_blocking(session, 0);

        //// Authenticate with username and password
        //libssh2_userauth_password(session, sftp_user.c_str(), sftp_pass.c_str());

        //// Combine sftpHost and sftpPath to form the full path
        //std::string fullPath = sftp_host + sftp_path;

        //// Open an SFTP channel

        //LIBSSH2_SFTP* sftp = libssh2_sftp_init(session);
        //LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_open(sftp, fullPath.c_str(), LIBSSH2_FXF_READ, 0);

        //// Get the size of the file (use libssh2_sftp_stat for more robust handling)
        //LIBSSH2_SFTP_ATTRIBUTES sftpAttrs;
        //libssh2_sftp_stat(sftp, fullPath.c_str(), &sftpAttrs);
        //std::size_t file_size = sftpAttrs.filesize;

        ////StreamDescriptor streamer(*ioc, sock);
        ////boost::asio::windows::stream_handle(*ioc, sock);
        ////boost::asio::posix::stream_descriptor(*ioc, sock);
        //// Create an asio::stream_file for the SFTP file
        //boost::asio::stream_file file_stream(*ioc);
        //file_stream.open(fullPath, boost::asio::stream_file::flags::read_only);
        //// Create a shared buffer for each SFTP operation
        //auto buffer = std::make_shared<std::vector<char>>(file_size);


        //// Start the asynchronous SFTP operation
        //boost::asio::async_read(file_stream, boost::asio::buffer(*buffer), [ioc, sftp, sftpHandle, buffer](const boost::system::error_code& error, std::size_t bytes_transferred) {
        //    //handle_sftp_read(error, bytes_transferred, buffer);

        //    // Cleanup libssh2 resources
        //    libssh2_sftp_close_handle(sftpHandle);
        //    libssh2_sftp_shutdown(sftp);
        //    libssh2_exit();
        //    });
        std::shared_ptr<string> result = std::make_shared < string>("test");
        return result;
    }

} // End namespace sgns
