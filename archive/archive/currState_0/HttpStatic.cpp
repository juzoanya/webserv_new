/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 12:12:22 by mberline          #+#    #+#             */
/*   Updated: 2024/01/21 22:45:03 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpStatic::HttpStatic( ConfigHandler& configHandler ) : dataPtr(NULL), dataSize(0),  _request(configHandler), _configHandler(&configHandler)
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
    std::cout << "set Header\n";
    this->_fileData.clear();
    this->_fileData.reserve(4096);
    std::stringstream ss;
    ss << "HTTP/1.1" << ws_http::statuscodes.at(status) << CRLF;
    ss << "connection: close" << CRLF;
    // ss << "connection: keep-alive" << CRLF;
    // ss << "keep-alive: timeout=30 max=100" << CRLF;
    if (!location.empty())
        ss << "location: " << location << CRLF;
    if (!mimeType.empty())
        ss << "content-type: " << mimeType << CRLF;
    ss << "content-length: " << fileSize << CRLF;
    ss << "date: " << getDateString(0, "%d.%m.%Y %H:%M:%S") << CRLF;
    ss << CRLF;
    std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
    return (status);
}

ws_http::statuscodes_t  HttpStatic::makeRedirect( Redirect& redirect )
{
    std::cout << "make Redirect\n";
    this->setHeader(redirect.redirectStatus, "", 0, redirect.location);
    this->dataPtr = this->_fileData.data();
    this->dataSize = this->_fileData.size();
    return (redirect.redirectStatus);    
}

HttpStatic::file_t  HttpStatic::checkFileStat( std::string const & filePath, struct stat* custFileStat )
{
    std::cout << "check FileStat\n";
    struct stat fileStat;
    if (!custFileStat)
        custFileStat = &fileStat;
    if (stat(filePath.c_str(), custFileStat) == -1 || access(filePath.c_str(), R_OK) == -1) {
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
    std::cout << "handle Data\n";
    ws_http::statuscodes_t status = this->_request.parseRequest(it_start, it_end);
    if (status == ws_http::STATUS_UNDEFINED)
        return (ws_http::STATUS_UNDEFINED);
    if (status == ws_http::STATUS_200_OK)
        status = this->processResponse(ws_http::STATUS_200_OK, this->_request.method, this->_request.pathDecoded);
    else
        status = this->processResponse(this->_request.status, "GET", this->_request.pathDecoded);
    this->_request = HttpRequest(*this->_configHandler);
    return (status);
}

ws_http::statuscodes_t    HttpStatic::processResponse(ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestUri )
{
    std::cout << "process Response" << std::endl;
    // HttpConfig config = this->_configHandler->getHttpConfig(requestUri, this->_request.hostHeader);
    this->_httpConfig = this->_configHandler->getHttpConfig(requestUri, this->_request.hostHeader);
    std::cout << "config received, filePath: " << this->_httpConfig.getFilePath() << std::endl;
    Redirect redirect = this->_httpConfig.getRedirection();
    
    std::cout << "---------------------\n";
    
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
        std::cout << "make redirect\n";
        return (this->makeRedirect(redirect));
    }
    std::cout << "---------------------\n";
    if (!this->_httpConfig.checkAllowedMethod(method)) {
        std::cout << "method not allowed\n";
        return (this->setError(ws_http::STATUS_405_METHOD_NOT_ALLOWED));
    }

    std::cout << "---------------------\n";

    HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath(), NULL);
    if (fileStat == WS_NO_ACCESS) {
        std::cout << "fileStat == WS_NO_ACCESS\n";
        return (this->setError(ws_http::STATUS_403_FORBIDDEN));
    }
    else if (fileStat == WS_REGULAR && this->setFile(statusCode)) {
        std::cout << "fileStat == WS_REGULAR && this->setFile(statusCode)\n";
        return (ws_http::STATUS_200_OK);
    }
    else if (fileStat == WS_DIR) {
        std::cout << "fileStat == WS_DIR\n";
        return (this->checkDirectory(this->_request.pathDecoded));
    }
    std::cout << "---------------------\n";
    return (this->setError(ws_http::STATUS_404_NOT_FOUND));
}

ws_http::statuscodes_t    HttpStatic::setError( ws_http::statuscodes_t errorStatusCode )
{
    std::cout << "set Error - " << errorStatusCode << std::endl;
    std::string const & errorPagePath = this->_httpConfig.getErrorPage(errorStatusCode);
    if (!errorPagePath.empty()) {
        std::cout << "error Page found -> new config\n";
        return (this->processResponse(errorStatusCode, "GET", errorPagePath));
    }
    std::stringstream ss;
    ss << "<!doctype html>" CRLF;
    ss << "<html lang=\"en\">" CRLF;
    ss << "<head>" CRLF;
    ss << "<meta charset=\"utf-8\">" CRLF;
    ss << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" << CRLF;
    ss << "</head>" CRLF;
    ss << "<body>" CRLF;
    ss << "<div style=\"text-align: center;\">" CRLF ;
    ss << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>" << CRLF;
    ss << "<hr>" CRLF;
    ss << "<p>" WEBSERV_VERSION "</p>" CRLF;
    ss << "</div>" CRLF;
    ss << "</body>" CRLF;
    ss << "</html>" CRLF;
    this->setHeader(errorStatusCode, "text/html", getStingStreamSize(ss));
    std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
    this->dataPtr = this->_fileData.data();
    this->dataSize = this->_fileData.size();
    return (errorStatusCode);

    // std::stringstream ss;
    // ss << errorStatus;
    // std::string const & errorPagePath = this->_httpConfig.getErrorPage(ss.str());
    // if (!errorPagePath.empty()) {
    //     HttpConfig errorConfig = configHandler.getHttpConfig(errorPagePath, this->_request.hostHeader, "");
    //     if (HttpStatic::checkFileStat(errorConfig.getFilePath(), NULL) == WS_REGULAR
    //         && this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorStatus, errorConfig.getFilePath())) {
    //         return (errorStatus);
    //     }
    // }
    // std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
    // + ws_http::statuscodes.at(errorStatus) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
    // + ws_http::statuscodes.at(errorStatus) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
}

bool    HttpStatic::setFile( ws_http::statuscodes_t statusCode )
{
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

ws_http::statuscodes_t    HttpStatic::checkDirectory( std::string const & requestUri )
{
    std::cout << "check Directory\n";
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
                return (this->processResponse(ws_http::STATUS_200_OK, this->_request.method, requestUri + "/" + filename));
            }
            HttpStatic::file_t currFile = HttpStatic::checkFileStat(filePath + "/" + filename, &fileStat);
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
    return (this->setError(ws_http::STATUS_403_FORBIDDEN));
}

void    HttpStatic::setDirListing( std::string requestUri, std::vector<DirListingInfos>& dirEntries )
{
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
        std::cout << "rootpart: " << rootPart << std::endl;
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



// ws_http::statuscodes_t    HttpStatic::checkDirectory( HttpConfig& config )
// {
//     std::vector<std::string> const & indexFiles = config.getIndexFile();
//     std::string const & filePath = config.getFilePath();
//     DIR* currDir = opendir(filePath.c_str());
//     if (currDir != NULL) {
//         std::vector< std::pair<std::string, struct stat> > dirEntries;
//         struct dirent *dirElem = NULL;
//         while ((dirElem = readdir(currDir))) {
//             struct stat fileStat;
//             std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
//             if (filename == "." || filename == "..")
//                 continue;
//             if (std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end())
//                 return (this->processResponse(ws_http::STATUS_200_OK, this->_request.method, filePath + "/" + filename));
//             HttpStatic::file_t currFile = HttpStatic::checkFileStat(filePath + "/" + filename, &fileStat);
//             if (currFile == WS_REGULAR || currFile == WS_DIR)
//                 dirEntries.push_back(std::make_pair(filename, fileStat));
//         }
//         closedir(currDir);
//         if (config.hasDirectoryListing()) {
//             this->setDirListing(this->_request.pathDecoded, dirEntries);
//         }
//     }
// }


// ws_http::statuscodes_t    HttpStatic::checkDirectory( void )
// {
//     std::string const & filePath = this->_httpConfig.getFilePath();
//     DIR* currDir = opendir(filePath.c_str());
//     if (currDir != NULL) {
//         std::vector< std::pair<std::string, struct stat> > dirEntries;
//         struct dirent *dirElem = NULL;
//         while ((dirElem = readdir(currDir))) {
//             struct stat fileStat;
//             std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
//             if (filename == "." || filename == "..")
//                 continue;
//             std::string filePathFull = filePath + "/" + filename;
//             if (stat(filePathFull.c_str(), &fileStat) == -1 || access(filePathFull.c_str(), R_OK) == -1)
//                 continue;
//             std::vector<std::string> const & indexFiles = this->_httpConfig.getIndexFile();
//             if (S_ISREG(fileStat.st_mode)
//             && std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end()) {
//                 this->setFile( filePathFull);
//                 return ;
//             }
//             dirEntries.push_back(std::make_pair(filename, fileStat));
//         }
//         closedir(currDir);
//         if (this->_httpConfig.hasDirectoryListing()) {
//             this->setDirListing(this->_request.pathDecoded, dirEntries);
//         } else {
//             this->_status = ws_http::STATUS_403_FORBIDDEN;
//         }
//     }
// }



// ws_http::statuscodes_t    HttpStatic::setError( ConfigHandler& configHandler, ws_http::statuscodes_t errorStatus )
// {
//    std::stringstream ss;
//     ss << errorStatus;
//     std::string const & errorPagePath = this->_httpConfig.getErrorPage(ss.str());
//     if (!errorPagePath.empty()) {
//         HttpConfig errorConfig = configHandler.getHttpConfig(errorPagePath, this->_request.hostHeader, "");
//         if (HttpStatic::checkFileStat(errorConfig.getFilePath(), NULL) == WS_REGULAR
//             && this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorStatus, errorConfig.getFilePath())) {
//             return (errorStatus);
//         }
//     }

//     std::stringstream ss;
//     ss << "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>";
//     ss << ws_http::statuscodes.at(errorStatus);
//     ss << "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>";
//     ss << ws_http::statuscodes.at(errorStatus);
//     ss << "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;

//     std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));


//     std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
//     + ws_http::statuscodes.at(errorStatus) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
//     + ws_http::statuscodes.at(errorStatus) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
//     this->_fileData = std::vector<char>(errPage.begin(), errPage.end());
// }















/* newer version */
// HttpStatic::HttpStatic( void ) : _status(ws_http::STATUS_404_NOT_FOUND), _isDirectory(false)
// { }

// HttpStatic::~HttpStatic( void )
// { }

// // int HttpStatic::setHeader( ws_http::statuscodes_t status )
// // {
// //     std::stringstream headerstream;
// //     headerstream << "HTTP/1.1" << ws_http::statuscodes.at(status) << CRLF;
// //     headerstream << "connection: close" << CRLF;
// //     headerstream << "content-type: " << this->_httpConfig.getMimeType() << CRLF;
// //     headerstream << "content-length: " << this->_fileData.size() << CRLF;
// //     std::time_t t = std::time(0);
// //     std::tm* now = std::gmtime(&t);
// //     char buff[1000];
// //     std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
// //     buff[size] = 0;
// //     headerstream << "date: " << buff << CRLF << CRLF;
// //     headerstream << CRLF;
// //     this->_httpHeader = headerstream.str();
// //     // this->_headerContent = Content(this->_httpHeader.data(), this->_httpHeader.size());
// //     // this->_bodyContent = Content(this->_staticContent.fileData.data(), this->_staticContent.fileData.size());
// //     return (0);
// // }

// std::string getDateString(std::time_t time, const char* format)
// {
//     std::time_t t = time;
//     if (t == 0)
//         t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, format, now);
//     return (std::string(buff, size));
// }

// ws_http::statuscodes_t HttpStatic::setHeader( ws_http::statuscodes_t status )
// {
//     this->_fileData.clear();
//     this->_fileData.reserve(1024);
//     std::stringstream ss;
//     ss << "HTTP/1.1" << ws_http::statuscodes.at(status) << CRLF;
//     ss << "connection: close" << CRLF;
//     ss << "content-type: " << this->_httpConfig.getMimeType() << CRLF;
//     ss << "content-length: " << this->_fileData.size() << CRLF;
//     ss << "date: " << getDateString(0, "%d.%m.%Y %H:%M:%S") << CRLF;
//     ss << CRLF;
//     std::copy(std::istreambuf_iterator<char>(ss), std::istreambuf_iterator<char>(), std::back_inserter(this->_fileData));
//     return (status);
// }

// // void    HttpStatic::handleData( ConfigHandler& configHandler, buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// // {
// //     HttpConfig  currHttpConfig;
// //     this->_request.parseRequest(it_start, it_end);
// //     if (this->_request.status == ws_http::STATUS_UNDEFINED)
// //         return ;
// //     this->_httpConfig = configHandler.getHttpConfig(this->_request.pathDecoded, this->_request.hostHeader, "/var/www");
// //     if (this->_request.status != ws_http::STATUS_200_OK)
// //         return (this->setError(configHandler, this->_request.status));
// //     Redirect redirect = this->_httpConfig.getRedirection();
// //     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
// //         this->setHeader(redirect.redirectStatus);
// //     if (!this->_httpConfig.checkAllowedMethod(this->_request.method))
// //         return (this->setError(configHandler, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
// //     this->setContentByPath(this->_httpConfig.getFilePath(), this->_request.pathDecoded, this->_httpConfig.getIndexFile(), this->_httpConfig.hasDirectoryListing());
// //     this->_request = HttpRequest();
// // }

// HttpStatic::file_t  HttpStatic::checkFileStat( std::string const & filePath )
// {
//     struct stat fileStat;
//     if (stat(filePath.c_str(), &fileStat) == -1 || access(filePath.c_str(), R_OK) == -1) {
//         if (errno == EACCES)
//             return (WS_NO_ACCESS);
//     } else if (S_ISREG(fileStat.st_mode)) {
//         return (WS_REGULAR);
//     } else if (S_ISDIR(fileStat.st_mode)) {
//         return (WS_DIR);
//     } 
//     return (WS_NOT_FOUND);
// }

// ws_http::statuscodes_t    HttpStatic::handleData( ConfigHandler& configHandler, buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// {
//     HttpConfig  currHttpConfig;
//     this->_request.parseRequest(it_start, it_end);
//     if (this->_request.status == ws_http::STATUS_UNDEFINED)
//         return ;
//     this->_httpConfig = configHandler.getHttpConfig(this->_request.pathDecoded, this->_request.hostHeader, "/var/www");
//     if (this->_request.status != ws_http::STATUS_200_OK)
//         return (this->setError(configHandler, this->_request.status));
//     Redirect redirect = this->_httpConfig.getRedirection();
//     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
//         this->setHeader(redirect.redirectStatus);
//     if (!this->_httpConfig.checkAllowedMethod(this->_request.method))
//         return (this->setError(configHandler, ws_http::STATUS_405_METHOD_NOT_ALLOWED));

//     HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath());
//     if (fileStat == WS_NO_ACCESS)
//         return (this->setError(configHandler, ws_http::STATUS_403_FORBIDDEN));
//     else if (fileStat == WS_NOT_FOUND)
//         return (this->setError(configHandler, ws_http::STATUS_404_NOT_FOUND));
//     else if (fileStat == WS_REGULAR && this->setFile(this->_httpConfig.getFilePath()))
//         this->setHeader(ws_http::STATUS_200_OK);
//     else
//         this->checkDirectory();
        

//     // struct stat fileStat;
//     // const char* filePath = this->_httpConfig.getFilePath().c_str();
//     // if ((stat(filePath, &fileStat) == -1 || access(filePath, R_OK) == -1)) {
//     //     if (errno == EACCES)
//     //         return (this->setError(configHandler, ws_http::STATUS_403_FORBIDDEN));
//     //     if (errno == ENOENT)
//     //         return (this->setError(configHandler, ws_http::STATUS_404_NOT_FOUND));
//     // } else if (S_ISREG(fileStat.st_mode)) {
//     //     if (!this->setFile(filePath))
//     //         return (this->setError(configHandler, ws_http::STATUS_404_NOT_FOUND));
//     // } else if (S_ISDIR(fileStat.st_mode)) {
//     //     this->checkDirectory();
//     // }
//     this->_request = HttpRequest();
// }

// void    HttpStatic::setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing )
// {
//     // this->isDirectory = false;
//     // this->status = ws_http::STATUS_404_NOT_FOUND;
//     struct stat fileStat;
//     if ((stat(filePath.c_str(), &fileStat) == -1 || access(filePath.c_str(), R_OK) == -1) && errno == EACCES) {
//         this->_status = ws_http::STATUS_403_FORBIDDEN;
//     } else if (S_ISREG(fileStat.st_mode)) {
//         this->setFile(filePath);
//     } else if (S_ISDIR(fileStat.st_mode)) {
//         this->checkDirectory();
//     }
// }

// ws_http::statuscodes_t    HttpStatic::setError( ConfigHandler& configHandler, ws_http::statuscodes_t errorStatus )
// {
//    std::stringstream ss;
//     ss << errorStatus;
//     std::string const & errorPagePath = this->_httpConfig.getErrorPage(ss.str());
//     if (!errorPagePath.empty()) {
//     //     HttpConfig errorConfig = configHandler.getHttpConfig(errorPagePath, this->_request.headerMap.at("host"), "");
//     //     struct stat fileStat;
//     //     const char* errorFilePath = errorConfig.getFilePath().c_str();
//     //     if (stat(errorFilePath, &fileStat) == 0 && access(errorFilePath, R_OK) == 0
//     //     && S_ISREG(fileStat.st_mode) && this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorFilePath)) {
//     //         this->setHeader(errorStatus);
//     //         return ;
//     //     }
//     // }
//         HttpConfig errorConfig = configHandler.getHttpConfig(errorPagePath, this->_request.headerMap.at("host"), "");
//         if (HttpStatic::checkFileStat(errorConfig.getFilePath()) == WS_REGULAR
//             && this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorConfig.getFilePath())) {
//             this->setHeader(errorStatus);
//             return ;
//         }
//     }
//     std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
//     + ws_http::statuscodes.at(errorStatus) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
//     + ws_http::statuscodes.at(errorStatus) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
//     this->_fileData = std::vector<char>(errPage.begin(), errPage.end());
// }

// // bool    HttpStatic::setFile( std::string const & filePath )
// // {
// //     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
// //     if (ifs.is_open()) {
// //         std::size_t filesize = ifs.tellg();
// //         ifs.seekg(0, std::ios::beg);
// //         this->_fileData.clear();
// //         this->_fileData.resize(filesize);
// //         ifs.read(this->_fileData.data(), filesize);
// //         ifs.close();
// //         this->_status = ws_http::STATUS_200_OK;
// //         return (true);
// //     }
// //     return (false);
// // }

// bool    HttpStatic::setFile( std::string const & filePath )
// {
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (ifs.is_open()) {
//         std::size_t filesize = ifs.tellg();
//         ifs.seekg(0, std::ios::beg);
//         this->_fileData.clear();
//         this->setHeader(ws_http::STATUS_200_OK);
//         this->_fileData.resize(filesize);
//         ifs.read(this->_fileData.data(), filesize);
//         ifs.close();
//         this->_status = ws_http::STATUS_200_OK;
//         return (true);
//     }
//     return (false);
// }

// ws_http::statuscodes_t    HttpStatic::checkDirectory( void )
// {
//     std::string const & filePath = this->_httpConfig.getFilePath();
//     DIR* currDir = opendir(filePath.c_str());
//     if (currDir != NULL) {
//         std::vector< std::pair<std::string, struct stat> > dirEntries;
//         struct dirent *dirElem = NULL;
//         while ((dirElem = readdir(currDir))) {
//             struct stat fileStat;
//             std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
//             if (filename == "." || filename == "..")
//                 continue;
//             std::string filePathFull = filePath + "/" + filename;
//             if (stat(filePathFull.c_str(), &fileStat) == -1 || access(filePathFull.c_str(), R_OK) == -1)
//                 continue;
//             std::vector<std::string> const & indexFiles = this->_httpConfig.getIndexFile();
//             if (S_ISREG(fileStat.st_mode)
//             && std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end()) {
//                 this->setFile(filePathFull);
//                 return ;
//             }
//             dirEntries.push_back(std::make_pair(filename, fileStat));
//         }
//         closedir(currDir);
//         if (this->_httpConfig.hasDirectoryListing()) {
//             this->setDirListing(this->_request.pathDecoded, dirEntries);
//         } else {
//             this->_status = ws_http::STATUS_403_FORBIDDEN;
//         }
//     }
// }






// // void    HttpStatic::setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries )
// // {
// //     std::stringstream ss;
// //     ss << "<!DOCTYPE html>";
// //     ss << "<html lang=\"en\">";
// //     ss << "<head>";
// //     ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
// //     ss << "<title>Directory Listing</title>";
// //     ss << "<style>"
// //             "* {font-family: sans-serif;}\
// //             table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
// //             tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
// //             thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
// //             th, td {padding: 1em 2em;}\
// //             tbody tr:hover {background-color: #d9d7d7;}\
// //             tbody tr {border-bottom: 1px solid #dddddd;}\
// //             tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
// //             tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
// //         "</style>";
// //     ss << "</head>";
// //     ss << "<body>";

// //     ss << "<h1>";
// //     std::string dirstrs, rootPart;
// //     std::stringstream dirstream(requestUri);
// //     while (std::getline(dirstream, dirstrs, '/')) {
// //         rootPart += dirstrs + "/";
// //         std::cout << "rootpart: " << rootPart << std::endl;
// //         if (dirstrs.empty()) dirstrs = "~";
// //         std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
// //         ss << a;
// //     }
// //     ss << "</h1>";

// //     ss << "<table>";
// //     ss << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
// //     const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
// //     for (std::size_t i = 0; i != dirEntries.size(); ++i) {
// //         ss << "<tr>";
// //         ss << "<td><a href=\"" << requestUri << dirEntries[i].first << "\">" << dirEntries[i].first << "</a></td>";
// //         double dSize = static_cast<double>(dirEntries[i].second.st_size);
// //         std::size_t k = 0;
// //         for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
// //         ss << "<td>" << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i] << "</td>";
// //         std::tm* now = std::localtime(&dirEntries[i].second.st_mtime);
// //         char dateBuff[100];
// //         std::size_t dateStrSize = strftime(dateBuff, 1000, "%d.%m.%Y %H:%M:%S", now);
// //         dateBuff[dateStrSize = 0];
// //         ss << "<td>" << dateBuff << "</td>";
// //         ss << "</tr>";
// //     }
// //     ss << "</table>";

// //     ss << "</body>";
// //     ss << "</html>";
// //     this->isDirectory = true;
// //     this->status = ws_http::STATUS_200_OK;
// //     std::string dirstr = ss.str();
// //     this->fileData = std::vector<char>(dirstr.begin(), dirstr.end());
// // }










































/* mostly original */
// HttpStatic::HttpStatic( void ) : _status(ws_http::STATUS_404_NOT_FOUND), _isDirectory(false)
// { }

// HttpStatic::~HttpStatic( void )
// { }

// int HttpStatic::setHeader( ws_http::statuscodes_t status )
// {
//     std::stringstream headerstream;
//     headerstream << "HTTP/1.1" << ws_http::statuscodes.at(status) << CRLF;
//     headerstream << "connection: close" << CRLF;
//     headerstream << "content-type: " << this->_httpConfig.getMimeType() << CRLF;
//     headerstream << "content-length: " << this->_fileData.size() << CRLF;
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
//     buff[size] = 0;
//     headerstream << "date: " << buff << CRLF << CRLF;
//     headerstream << CRLF;
//     this->_httpHeader = headerstream.str();
//     // this->_headerContent = Content(this->_httpHeader.data(), this->_httpHeader.size());
//     // this->_bodyContent = Content(this->_staticContent.fileData.data(), this->_staticContent.fileData.size());
//     return (0);
// }

// // void    HttpStatic::handleData( ConfigHandler& configHandler, buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// // {
// //     HttpConfig  currHttpConfig;
// //     this->_request.parseRequest(it_start, it_end);
// //     if (this->_request.status == ws_http::STATUS_UNDEFINED)
// //         return ;
// //     this->_httpConfig = configHandler.getHttpConfig(this->_request.pathDecoded, this->_request.hostHeader, "/var/www");
// //     if (this->_request.status != ws_http::STATUS_200_OK)
// //         return (this->setError(configHandler, this->_request.status));
// //     Redirect redirect = this->_httpConfig.getRedirection();
// //     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
// //         this->setHeader(redirect.redirectStatus);
// //     if (!this->_httpConfig.checkAllowedMethod(this->_request.method))
// //         return (this->setError(configHandler, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
// //     this->setContentByPath(this->_httpConfig.getFilePath(), this->_request.pathDecoded, this->_httpConfig.getIndexFile(), this->_httpConfig.hasDirectoryListing());
// //     this->_request = HttpRequest();
// // }

// void    HttpStatic::handleData( ConfigHandler& configHandler, buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// {
//     HttpConfig  currHttpConfig;
//     this->_request.parseRequest(it_start, it_end);
//     if (this->_request.status == ws_http::STATUS_UNDEFINED)
//         return ;
//     this->_httpConfig = configHandler.getHttpConfig(this->_request.pathDecoded, this->_request.hostHeader, "/var/www");
//     if (this->_request.status != ws_http::STATUS_200_OK)
//         return (this->setError(configHandler, this->_request.status));
//     Redirect redirect = this->_httpConfig.getRedirection();
//     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
//         this->setHeader(redirect.redirectStatus);
//     if (!this->_httpConfig.checkAllowedMethod(this->_request.method))
//         return (this->setError(configHandler, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
//     struct stat fileStat;
//     const char* filePath = this->_httpConfig.getFilePath().c_str();
//     if ((stat(filePath, &fileStat) == -1 || access(filePath, R_OK) == -1)) {
//         if (errno == EACCES)
//             return (this->setError(configHandler, ws_http::STATUS_403_FORBIDDEN));
//         if (errno == ENOENT)
//             return (this->setError(configHandler, ws_http::STATUS_404_NOT_FOUND));
//     } else if (S_ISREG(fileStat.st_mode)) {
//         if (!this->setFile(filePath))
//             return (this->setError(configHandler, ws_http::STATUS_404_NOT_FOUND));
//     } else if (S_ISDIR(fileStat.st_mode)) {
//         this->checkDirectory();
//     }
//     this->_request = HttpRequest();
// }

// void    HttpStatic::setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing )
// {
//     // this->isDirectory = false;
//     // this->status = ws_http::STATUS_404_NOT_FOUND;
//     struct stat fileStat;
//     if ((stat(filePath.c_str(), &fileStat) == -1 || access(filePath.c_str(), R_OK) == -1) && errno == EACCES) {
//         this->_status = ws_http::STATUS_403_FORBIDDEN;
//     } else if (S_ISREG(fileStat.st_mode)) {
//         this->setFile(filePath);
//     } else if (S_ISDIR(fileStat.st_mode)) {
//         this->checkDirectory();
//     }
// }

// void    HttpStatic::setError( ConfigHandler& configHandler, ws_http::statuscodes_t errorStatus )
// {
//    std::stringstream ss;
//     ss << errorStatus;
//     std::string const & errorPagePath = this->_httpConfig.getErrorPage(ss.str());
//     if (!errorPagePath.empty()) {
//         HttpConfig errorConfig = configHandler.getHttpConfig(errorPagePath, this->_request.headerMap.at("host"), "");
//         struct stat fileStat;
//         const char* errorFilePath = errorConfig.getFilePath().c_str();
//         if (stat(errorFilePath, &fileStat) == 0 && access(errorFilePath, R_OK) == 0
//         && S_ISREG(fileStat.st_mode) && this->_httpConfig.getMimeType() == "text/html" && this->setFile(errorFilePath)) {
//             this->setHeader(errorStatus);
//             return ;
//         }
//     }
//     std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
//     + ws_http::statuscodes.at(errorStatus) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
//     + ws_http::statuscodes.at(errorStatus) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
//     this->_fileData = std::vector<char>(errPage.begin(), errPage.end());
// }

// bool    HttpStatic::setFile( std::string const & filePath )
// {
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (ifs.is_open()) {
//         std::size_t filesize = ifs.tellg();
//         ifs.seekg(0, std::ios::beg);
//         this->_fileData.clear();
//         this->_fileData.resize(filesize);
//         ifs.read(this->_fileData.data(), filesize);
//         ifs.close();
//         this->_status = ws_http::STATUS_200_OK;
//         return (true);
//     }
//     return (false);
// }

// void    HttpStatic::checkDirectory( void )
// {
//     std::string const & filePath = this->_httpConfig.getFilePath();
//     DIR* currDir = opendir(filePath.c_str());
//     if (currDir != NULL) {
//         std::vector< std::pair<std::string, struct stat> > dirEntries;
//         struct dirent *dirElem = NULL;
//         while ((dirElem = readdir(currDir))) {
//             struct stat fileStat;
//             std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
//             if (filename == "." || filename == "..")
//                 continue;
//             std::string filePathFull = filePath + "/" + filename;
//             if (stat(filePathFull.c_str(), &fileStat) == -1 || access(filePathFull.c_str(), R_OK) == -1)
//                 continue;
//             std::vector<std::string> const & indexFiles = this->_httpConfig.getIndexFile();
//             if (S_ISREG(fileStat.st_mode)
//             && std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end()) {
//                 this->setFile(filePathFull);
//                 return ;
//             }
//             dirEntries.push_back(std::make_pair(filename, fileStat));
//         }
//         closedir(currDir);
//         if (this->_httpConfig.hasDirectoryListing()) {
//             this->setDirListing(this->_request.pathDecoded, dirEntries);
//         } else {
//             this->_status = ws_http::STATUS_403_FORBIDDEN;
//         }
//     }
// }






// // void    HttpStatic::setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries )
// // {
// //     std::stringstream ss;
// //     ss << "<!DOCTYPE html>";
// //     ss << "<html lang=\"en\">";
// //     ss << "<head>";
// //     ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
// //     ss << "<title>Directory Listing</title>";
// //     ss << "<style>"
// //             "* {font-family: sans-serif;}\
// //             table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
// //             tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
// //             thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
// //             th, td {padding: 1em 2em;}\
// //             tbody tr:hover {background-color: #d9d7d7;}\
// //             tbody tr {border-bottom: 1px solid #dddddd;}\
// //             tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
// //             tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
// //         "</style>";
// //     ss << "</head>";
// //     ss << "<body>";

// //     ss << "<h1>";
// //     std::string dirstrs, rootPart;
// //     std::stringstream dirstream(requestUri);
// //     while (std::getline(dirstream, dirstrs, '/')) {
// //         rootPart += dirstrs + "/";
// //         std::cout << "rootpart: " << rootPart << std::endl;
// //         if (dirstrs.empty()) dirstrs = "~";
// //         std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
// //         ss << a;
// //     }
// //     ss << "</h1>";

// //     ss << "<table>";
// //     ss << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
// //     const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
// //     for (std::size_t i = 0; i != dirEntries.size(); ++i) {
// //         ss << "<tr>";
// //         ss << "<td><a href=\"" << requestUri << dirEntries[i].first << "\">" << dirEntries[i].first << "</a></td>";
// //         double dSize = static_cast<double>(dirEntries[i].second.st_size);
// //         std::size_t k = 0;
// //         for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
// //         ss << "<td>" << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i] << "</td>";
// //         std::tm* now = std::localtime(&dirEntries[i].second.st_mtime);
// //         char dateBuff[100];
// //         std::size_t dateStrSize = strftime(dateBuff, 1000, "%d.%m.%Y %H:%M:%S", now);
// //         dateBuff[dateStrSize = 0];
// //         ss << "<td>" << dateBuff << "</td>";
// //         ss << "</tr>";
// //     }
// //     ss << "</table>";

// //     ss << "</body>";
// //     ss << "</html>";
// //     this->isDirectory = true;
// //     this->status = ws_http::STATUS_200_OK;
// //     std::string dirstr = ss.str();
// //     this->fileData = std::vector<char>(dirstr.begin(), dirstr.end());
// // }
