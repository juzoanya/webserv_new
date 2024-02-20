// #include <string>
// #include <iostream>
// #include <vector>
// #include <sstream>

// #define CRLF "\r\n"

#include "headers.hpp"

std::string getDateString(std::time_t time, const char* format)
{
    std::time_t t = time;
    if (t == 0)
        t = std::time(0);
    std::tm* now = std::gmtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, format, now);
    return (std::string(buff, size));
}

int main( int argc, char** argv )
{
    if (argc != 2)
        return (1);
    ConfigParser    config;
    ConfigHandler   handler("127.0.0.1", "8080");
    HttpConfig      httpConfig;
    try
    {
        config.configParser(argv[1]);
        handler.addServerConfig(&config.serverConfigs[0]);
        httpConfig = handler.getHttpConfig("/", "localhost", "/var");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
    
    ws_http::statuscodes_t status = ws_http::STATUS_200_OK;
    std::vector<char> fileData;
    fileData.clear();
    fileData.reserve(1024);
    std::stringstream headerstream;
    headerstream << "HTTP/1.1 " << ws_http::statuscodes.at(status) << CRLF;
    headerstream << "connection: close" << CRLF;
    headerstream << "content-type: " << httpConfig.getMimeType() << CRLF;
    headerstream << "content-length: " << fileData.size() << CRLF;
    headerstream << "date: " << getDateString(0, "%d.%m.%Y %H:%M:%S") << CRLF;
    headerstream << CRLF;
    std::copy(std::istreambuf_iterator<char>(headerstream), std::istreambuf_iterator<char>(), std::back_inserter(fileData));
    std::cout << "header by fileData:\n" << std::string(fileData.begin(), fileData.end()) << std::endl;


    // std::cout << "filePath: " << httpConfig.getFilePath() << std::endl;
    // std::copy(std::istream_iterator(headerstream))
    // this->_httpHeader = headerstream.str();
    // this->_headerContent = Content(this->_httpHeader.data(), this->_httpHeader.size());
    // this->_bodyContent = Content(this->_staticContent.fileData.data(), this->_staticContent.fileData.size());
    return (0);
}