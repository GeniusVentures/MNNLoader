//SFTPCommon.hpp
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include <memory>
#include "FileManager.hpp"
#include "SFTPLoader.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "URLStringUtil.h"
#include "libssh2.h"
#include "libssh2_sftp.h"
#include <thread>

#ifndef SFTPCOMMON_HPP
#define SFTPCOMMON_HPP
namespace sgns
{
	using namespace boost::asio;

	class SFTPDevice : public std::enable_shared_from_this<SFTPDevice> {
	public:
		using CompletionCallback = std::function<void(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<std::vector<char>> buffer, bool parse, bool save)>;
		using StatusCallback = std::function<void(const int&)>;

		SFTPDevice(
			std::string sftp_host,
			std::string sftp_path,
			std::string sftp_user,
			std::string sftp_pass,
			std::string sftp_pubkeyfile,
			std::string sftp_privkeyfile,
			std::string sftp_privkeypass,
			CompletionCallback handle_read,
			StatusCallback status,
			bool parse, bool save);
		~SFTPDevice() {
			// Cleanup resources if needed
			//libssh2_sftp_close_handle(sftpHandle_);
			//libssh2_sftp_shutdown(sftp_);
			//libssh2_session_disconnect(sftp2session_, "Normal Shutdown");
			//libssh2_session_free(sftp2session_);
			//libssh2_exit();
		}
		void StartSFTPDownload(std::shared_ptr<boost::asio::io_context> ioc, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, LIBSSH2_SESSION* sftp2session, CompletionCallback handle_read, StatusCallback status);
		void StartSFTPHandshake(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, basic_socket<ip::tcp, any_io_executor>::native_handle_type sock, CompletionCallback handle_read, StatusCallback status);
		void StartSFTPAuth(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, CompletionCallback handle_read, StatusCallback status);
		void StartCreateSFTP(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, CompletionCallback handle_read, StatusCallback status);
		void StartSFTPOpen(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket,LIBSSH2_SFTP* sftp, CompletionCallback handle_read, StatusCallback status);
		void StartSFTPGetSize(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, LIBSSH2_SFTP* sftp, LIBSSH2_SFTP_HANDLE* sftpHandle, CompletionCallback handle_read, StatusCallback status);
		void StartSFTPGetBlocks(std::shared_ptr<boost::asio::io_context> ioc, LIBSSH2_SESSION* sftp2session, std::shared_ptr<boost::asio::ip::tcp::socket> tcpSocket, LIBSSH2_SFTP_HANDLE* sftpHandle, std::shared_ptr<std::vector<char>> buffer, size_t totalBytesRead, CompletionCallback handle_read, StatusCallback status);
	private:


		static std::shared_ptr<SFTPDevice> instance_;

		//Vars
		std::size_t file_size_;
		std::string sftp_host_;
		std::string sftp_path_;
		std::string sftp_user_;
		std::string sftp_pass_;
		std::string sftp_pubkeyfile_;
		std::string sftp_privkeyfile_;
		std::string sftp_privkeypass_;
		bool parse_;
		bool save_;
		bool downloading_ = false;
	};
}

#endif