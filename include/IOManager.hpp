#ifndef SGNS_IOMANAGER_HPP
#define SGNS_IOMANAGER_HPP

#include <string>
#include <map>
#include <memory>
#include "IODevice.hpp"
#include "MediaParser.hpp"
#include "MediaFilter.hpp"

using namespace std;

namespace sgns::io {

    class IOManager {


    public:
	using MediaType = MediaParser::MediaType;
	using FilterType = MediaFilter::FilterType;
	using StreamDirection = IOStream::StreamDirection;
	using StreamFlags = IOStream::StreamFlags;

        virtual ~IOManager() {};
        /// @brief Register a IO Device to handle a schema
        /// @param url schema "https", "ipfs", "file", etc from https://xxxxx
        /// @param device that handles particular schema streams 
        void registerDevice(const string& schema, IODevice *device);

        /// @brief Register a synchronous Parser class to handle a specific extension suffix
        /// @param handlerParser Handler class object that can parse the data
        void registerParser(const MediaParser::MediaType& type, MediaParser *parser);

	// register Media Filter
	void registerFilter(const MediaFilter::FilterType& type, MediaFilter *filter);
    
        /// @brief Load a file given a filePath and optional parse the data
        /// @param url the full path and filename to load
        /// @param parse bool on weather to parse the file or not
        /// @return shared pointer to void * of the data loaded
	bool getFile(const std::string& srcPath, const std::string& fpath);

        /// @brief Save Data to a file via some system, throws exception on error
        /// @param url URL prefix filename and extension
        /// @param data shared pointer to void * of the data to save
        bool putFile(const std::string& fpath, const std::string& dstPath);

	/// move file
	bool moveFile(const string& , const string&);
    
        /// @brief Parse Data from a previously loaded file
        /// @param suffix the extension/suffix to know how to parse the data
        /// @param data
        /// @return shared pointer to void * of the data parsed
	shared_ptr<void> ParseData(const MediaParser::MediaType&,  shared_ptr<void> );
    

    protected:
	std::map<string, IODevice *> io_devices_;
	std::map<MediaParser::MediaType, MediaParser *> media_parsers_;

    private:
	bool moveMedia_(const string&, const string&);
    };

}

#endif // SGNS_IOMANAGER_HPP 
