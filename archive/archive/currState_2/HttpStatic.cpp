/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 12:12:22 by mberline          #+#    #+#             */
/*   Updated: 2024/01/24 08:20:21 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpStatic::HttpStatic( ConfigHandler& configHandler, std::string const & clientIpStr, std::string const & clientPortStr, int clientSockFd )
 :  clientSocket(clientSockFd), clientIp(clientIpStr), clientPort(clientPortStr),
    dataPtr(NULL), dataSize(0),  _request(configHandler), _configHandler(&configHandler), _intRedirectNbr(0)
{ }

HttpStatic::~HttpStatic( void )
{ }

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

std::size_t getStingStreamSize(std::stringstream & ss)
{
    ss.seekg(0, std::ios::end);
    std::size_t pageSize = ss.tellg();
    ss.seekg(0, std::ios::beg);
    return (pageSize);
}

ws_http::statuscodes_t HttpStatic::setHeader( ws_http::statuscodes_t status, std::string const & mimeType, std::size_t fileSize, std::string const & location )
{
    // std::cout << "deb\n";
    std::cout << "STATIC: set Header: status: " << ws_http::statuscodes.at(status) << " | mimeType: " << mimeType << " | fileSize: " << fileSize << " | location: " << location << std::endl;
    this->_fileData.clear();
    // std::cout << "deb1\n";
    this->_fileData.reserve(4096);
    // std::cout << "deb2\n";
    std::stringstream ss;
    ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << CRLF;
    ss << "connection: close" << CRLF;
    // ss << "connection: keep-alive" << CRLF;
    // ss << "keep-alive: timeout=30 max=100" << CRLF;
    // std::cout << "deb3\n";
    if (!location.empty())
        ss << "location: " << location << CRLF;
    if (!mimeType.empty())
        ss << "content-type: " << mimeType << CRLF;
    ss << "content-length: " << fileSize << CRLF;
    ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << CRLF;
    ss << CRLF;
    // std::cout << "deb4\n";
    std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
    // std::cout << "deb5\n";
    return (status);
}

ws_http::statuscodes_t  HttpStatic::makeRedirect( Redirect& redirect )
{
    std::cout << "STATIC: make Redirect\n";
    this->setHeader(redirect.redirectStatus, "", 0, redirect.location);
    this->dataPtr = this->_fileData.data();
    this->dataSize = this->_fileData.size();
    return (redirect.redirectStatus);    
}

HttpStatic::file_t  HttpStatic::checkFileStat( std::string const & filePath, struct stat* custFileStat, int accessFlags )
{
    struct stat fileStat;
    if (!custFileStat)
        custFileStat = &fileStat;
    if (stat(filePath.c_str(), custFileStat) == -1 || access(filePath.c_str(), accessFlags) == -1) {
        if (errno == EACCES)
            return (WS_NO_ACCESS);
    } else if (S_ISREG(custFileStat->st_mode)) {
        return (WS_REGULAR);
    } else if (S_ISDIR(custFileStat->st_mode)) {
        return (WS_DIR);
    } 
    return (WS_NOT_FOUND);
}

ws_http::statuscodes_t    HttpStatic::handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    ws_http::statuscodes_t status = this->_request.parseRequest(it_start, it_end);
    std::cout << "HttpStatic::handleData, statusCode parsing the Request: " << ws_http::statuscodes.at(status) << std::endl;
    // this->_request.printRequest();
    if (status == ws_http::STATUS_UNDEFINED)
        return (ws_http::STATUS_UNDEFINED);
    this->_request.printRequest();
    if (status == ws_http::STATUS_200_OK)
        status = this->processResponse(ws_http::STATUS_200_OK, this->_request.method, this->_request.pathDecoded, false);
    else
        status = this->setError(this->_request.status, false);
    // status = this->processResponse(this->_request.status, "GET", this->_request.pathDecoded);
    this->_request = HttpRequest(*this->_configHandler);
    if (this->_httpConfig.getMimeType() == "text/html")
        std::cout << "rsponse is:\n" << std::string(this->_fileData.begin(), this->_fileData.end()) << std::endl;
    return (status);
}

ws_http::statuscodes_t    HttpStatic::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestUri, bool internal )
{
    if (internal) {
        std::cout << "IS INTERNAL\n";
        this->_intRedirectNbr++;
    }
    if (this->_intRedirectNbr == 10) {
        std::cout << "INTERNAL REDIRECTIONS LIMIT REACHED\n";
        this->_intRedirectNbr = 0;
        return (this->setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, internal));
    }
    this->_httpConfig = this->_configHandler->getHttpConfig(requestUri, this->_request.hostHeader);
    std::cout << "STATIC: process Response:\n\t" << "status: " << statusCode << " | method: " << method << " | requestUri: " << requestUri << " |  filePath: " << this->_httpConfig.getFilePath() << std::endl;
    Redirect redirect = this->_httpConfig.getRedirection();
    
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
        return (this->makeRedirect(redirect));
    }
    if (!this->_httpConfig.checkAllowedMethod(method)) {
        return (this->setError(ws_http::STATUS_405_METHOD_NOT_ALLOWED, internal));
    }

    std::string const & cgiExecutablePath = this->_httpConfig.getCgiExecutable();
    if (!cgiExecutablePath.empty()) {
        
    }

    if (method == "GET" || method == "HEAD")
        return (this->handleGetHead(statusCode, method, internal));
    else if (method == "POST")
        return (this->handlePost());
    // HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath(), NULL);
    // if (fileStat == WS_NO_ACCESS) {
    //     return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
    // } else if (fileStat == WS_REGULAR && this->setFile(statusCode)) {
    //     return (ws_http::STATUS_200_OK);
    // } else if (fileStat == WS_DIR) {
    //     return (this->checkDirectory(this->_request.pathDecoded, internal));
    // }
    return (this->setError(ws_http::STATUS_404_NOT_FOUND, internal));
}

ws_http::statuscodes_t  HttpStatic::handleGetHead( ws_http::statuscodes_t statusCode, std::string const & method, bool internal )
{
    (void)method;
    HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath(), NULL, R_OK);
    if (fileStat == WS_NO_ACCESS) {
        return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
    } else if (fileStat == WS_REGULAR && this->setFile(statusCode)) {
        return (ws_http::STATUS_200_OK);
    } else if (fileStat == WS_DIR) {
        return (this->checkDirectory(this->_request.pathDecoded, internal));
    }
    return (this->setError(ws_http::STATUS_404_NOT_FOUND, internal));
}

ws_http::statuscodes_t  HttpStatic::handlePost( void )
{
    // HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath(), NULL, R_OK | W_OK);
    // if (fileStat != WS_DIR)
    //     return (this->setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, true));
    // http_header_map_t::const_iterator it = this->_request.headerMap.find("content-type");
    // if (it == this->_request.headerMap.end() || it->second.substr(0, it->second.find(';')) != "multipart/form-data")
    //     return (ws_http::STATUS_501_NOT_IMPLEMENTED);
    // http_header_map_t   MultipartFileHeaders;
    // http_header_map_t   MultipartFileInfos;
    // std::stringstream ss(it->second);
    // std::string contType, boundary;
    // ss >> contType;
    // ss >> boundary;
    
    // // std::ofstream ofs(this->_httpConfig.getFilePath().c_str());
    // // if (!ofs.is_open())
    // //     return (this->setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, true));
    return (this->setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, true));
}

ws_http::statuscodes_t    HttpStatic::setError( ws_http::statuscodes_t errorStatusCode, bool internal )
{
    std::cout << "STATIC: set Error: status: " << ws_http::statuscodes.at(errorStatusCode) << std::endl;
    std::string const & errorPagePath = this->_httpConfig.getErrorPage(errorStatusCode);
    std::cout << "errnroPagePath: " << errorPagePath << " | for: " << this->_httpConfig.getFilePath() << std::endl;
    if (!internal && !errorPagePath.empty()) {
        std::cout << "error Page found -> new config\n";
        // this->_httpConfig = this->_configHandler->getHttpConfig(errorPagePath, this->_request.hostHeader);
        // if (this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorStatusCode))
        //     return (errorStatusCode);
        return (this->processResponse(errorStatusCode, "GET", errorPagePath, true));
    }
    std::stringstream ss;
    ss << "<!doctype html>" CRLF
        << "<html lang=\"en\">" CRLF
        << "<head>" CRLF
        << "<meta charset=\"utf-8\">" CRLF
        << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" << CRLF
        << "</head>" CRLF
        << "<body>" CRLF
        << "<div style=\"text-align: center;\">" CRLF 
        << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>" << CRLF
        << "<hr>" CRLF
        << "<p>" WEBSERV_VERSION "</p>" CRLF
        << "</div>" CRLF
        << "</body>" CRLF
        << "</html>" CRLF
        ;
    this->setHeader(errorStatusCode, "text/html", getStingStreamSize(ss));
    // std::cout << "deb6\n";
    std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
    // std::cout << "deb7\n";
    this->dataPtr = this->_fileData.data();
    // std::cout << "deb8\n";
    this->dataSize = this->_fileData.size();
    // std::cout << "deb9\n";
    return (errorStatusCode);
}

bool    HttpStatic::setFile( ws_http::statuscodes_t statusCode )
{
    std::cout << "STATIC: set File: status: " << ws_http::statuscodes.at(statusCode) << std::endl;
    std::ifstream   ifs(this->_httpConfig.getFilePath().c_str(), std::ios::binary | std::ios::ate);
    if (ifs.is_open()) {
        std::size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        this->_fileData.clear();
        this->setHeader(statusCode, this->_httpConfig.getMimeType(), fileSize);
        std::size_t currBufferSize = this->_fileData.size();
        this->_fileData.resize(fileSize + currBufferSize);
        ifs.read(this->_fileData.data() + currBufferSize, fileSize);
        ifs.close();
        this->dataPtr = this->_fileData.data();
        this->dataSize = this->_fileData.size();
        return (true);
    }
    return (false);
}

DirListingInfos::DirListingInfos(    std::string const & name, struct stat & fileStat )
 : fileName(name)
{
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    double dSize = static_cast<double>(fileStat.st_size);
    std::size_t k = 0;
    for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
    this->fileSize = (std::ostringstream() << std::fixed << std::setprecision(1) << dSize << " " << suffixes[k]).str();
    this->fileModDate = getDateString(fileStat.st_mtime, "%d.%m.%Y %H:%M:%S");
}

DirListingInfos::~DirListingInfos( void )
{ }

ws_http::statuscodes_t    HttpStatic::checkDirectory( std::string const & requestUri, bool internal )
{
    std::cout << "STATIC: check Directory for: " << requestUri << std::endl;
    std::vector<std::string> const & indexFiles = this->_httpConfig.getIndexFile();
    std::string const & filePath = this->_httpConfig.getFilePath();
    DIR* currDir = opendir(filePath.c_str());
    if (currDir != NULL) {
        std::vector< DirListingInfos > dirEntries;
        struct dirent *dirElem = NULL;
        while ((dirElem = readdir(currDir))) {
            struct stat fileStat;
            std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
            if (filename == "." || filename == "..")
                continue;
            if (std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end()) {
                std::cout << "index file found at file name: " << filename << std::endl;
                return (this->processResponse(ws_http::STATUS_200_OK, this->_request.method, requestUri + "/" + filename, true));
            }
            HttpStatic::file_t currFile = HttpStatic::checkFileStat(filePath + "/" + filename, &fileStat, R_OK);
            if (currFile == WS_REGULAR || currFile == WS_DIR) {
                dirEntries.push_back(DirListingInfos(filename, fileStat));
            }
        }
        closedir(currDir);
        if (this->_httpConfig.hasDirectoryListing()) {
            this->setDirListing(requestUri, dirEntries);
            return (ws_http::STATUS_200_OK);
        }
    }
    return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
}

void    HttpStatic::setDirListing( std::string requestUri, std::vector<DirListingInfos>& dirEntries )
{
    std::cout << "STATIC: set DirListing for: " << requestUri << std::endl;
    if (requestUri[requestUri.size() - 1] != '/')
        requestUri.push_back('/');
    std::stringstream ss;
    ss << "<!DOCTYPE html>";
    ss << "<html lang=\"en\">";
    ss << "<head>";
    ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    ss << "<title>Directory Listing</title>";
    ss << "<style>"
            "* { font-family: sans-serif; }\
            table { border-collapse: collapse; margin: 1em 0; font-size: 1em; min-width: 400px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.15); }\
            tr td:first-child { width: 20px; white-space: nowrap;overflow: hidden; text-overflow: ellipsis; }\
            thead tr { background-color: #009879; color: #ffffff; text-align: left; }\
            th, td { padding: 1em 2em; }\
            tbody tr:hover { background-color: #d9d7d7; }\
            tbody tr { border-bottom: 1px solid #dddddd; }\
            tbody tr:nth-of-type(even) { background-color: #f3f3f3; }\
            tr:nth-of-type(even):hover { background-color: #d9d7d7; }"
        "</style>";
    ss << "</head>";
    ss << "<body>";
    ss << "<h1>";
    std::string dirstrs, rootPart;
    std::stringstream dirstream(requestUri);
    while (std::getline(dirstream, dirstrs, '/')) {
        rootPart += dirstrs + "/";
        // std::cout << "rootpart: " << rootPart << std::endl;
        if (dirstrs.empty()) dirstrs = "~";
        std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
        ss << a;
    }
    ss << "</h1>";
    ss << "<table>";
    ss << "<thead><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr></thead>";
    ss << "<tbody>";
    for (std::size_t i = 0; i != dirEntries.size(); ++i) {
        ss << "<tr>";
        ss << "<td><a href=\"" << requestUri << dirEntries[i].fileName << "\">" << dirEntries[i].fileName << "</a></td>";
        ss << "<td>" << dirEntries[i].fileSize << "</td>";
        ss << "<td>" << dirEntries[i].fileModDate << "</td>";
        ss << "</tr>";
    }
    ss << "</tbody>";
    ss << "</table>";
    ss << "</body>";
    ss << "</html>";
    this->setHeader(ws_http::STATUS_200_OK, "text/html",getStingStreamSize(ss));
    std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
    this->dataPtr = this->_fileData.data();
    this->dataSize = this->_fileData.size();
}
