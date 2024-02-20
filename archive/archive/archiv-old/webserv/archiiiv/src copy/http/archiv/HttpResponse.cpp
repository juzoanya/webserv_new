/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/26 10:51:18 by mberline          #+#    #+#             */
/*   Updated: 2023/12/07 11:50:03 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include "HttpResponse.hpp"
#include "directoryListing/FileInfo.hpp"
#include "directoryListing/HtmlElements.hpp"


int makeDirListing(std::string const & rootDir, std::string const & currDir, std::vector<char>& buffer )
{

    std::string headStyle =
    "\
        * {\
            font-family: sans-serif;\
        }\
        table {\
            border-collapse: collapse;\
            margin: 1em 0;\
            font-size: 1em;\
            min-width: 400px;\
            box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);\
        }\
        tr td:first-child {\
            width: 20px;\
            white-space: nowrap;\
            overflow: hidden;\
            text-overflow: ellipsis;\
        }\
        thead tr {\
            background-color: #009879;\
            color: #ffffff;\
            text-align: left;\
        }\
        th, td {\
            padding: 1em 2em;\
        }\
        tbody tr:hover {\
              background-color: #d9d7d7;\
        }\
        tbody tr {\
            border-bottom: 1px solid #dddddd;\
        }\
        tbody tr:nth-of-type(even) {\
            background-color: #f3f3f3;\
        }\
        tr:nth-of-type(even):hover {\
            background-color: #d9d7d7;\
        }\
    ";

    std::string relPath = currDir;
    if (relPath.back() != '/')
        relPath.push_back('/');
    FileInfo dir(rootDir + relPath);
    // std::cout << "Dir to read: " << rootDir + currDir << std::endl;
    if (!dir.readDirectory())
        return (dir.getError());
    // for (std::size_t i = 0; i != dir.getDirectoryElemCount(); ++i) {
    //     std::cout << "\nname: " << dir[i].getName() << std::endl;
    //     std::cout << "size: " << dir[i].getSizeStr() << std::endl;
    //     std::cout << "mod: " << dir[i].getModDate() << std::endl;
    //     std::cout << "dir?: " << dir[i].isDir() << std::endl;
    // }

    HtmlRoot root;
    root.addStyle(headStyle);
    root.addMeta("viewport", "width=device-width, initial-scale=1.0");
    root.title = "Directory Listing";
    HtmlElem* headl = HtmlElem::createt("h1", "Index of: ");
    root.body().addChild(headl);
    
    std::string dirstrs;
    std::string rootPart;
    std::stringstream ss(currDir);
    while (std::getline(ss, dirstrs, '/')) {
        // std::cout << "dirstr: " << dirstrs << std::endl;
        rootPart += dirstrs + "/";
        // std::cout << "rootpart: " << rootPart << std::endl;
        if (dirstrs.empty())
            dirstrs = "~";
        HtmlElem *anchor = headl->addChild(HtmlElem::createt("a", dirstrs));
        anchor->addAttribute("href", rootPart);
        headl->addChild(HtmlElem::createt("span", " / "));
    }
    
    Table* dirTable = Table::create(dir.getDirectoryElemCount() -1, 3, true);
    root.body().addChild(dirTable);
    (*dirTable)(0, 0).setText("Name");
    (*dirTable)(0, 1).setText("Size");
    (*dirTable)(0, 2).setText("Last Modified");

    for (std::size_t i = 0, k = 1; i != dir.getDirectoryElemCount(); ++i) {
        if (dir[i].getName() == "." || dir[i].getName() == "..")
            continue;
        HtmlElem *anchor = HtmlElem::createt("a", dir[i].getName());
        anchor->addAttribute("href", relPath + dir[i].getName());
        (*dirTable)(k, 0).addChild(anchor);
        (*dirTable)(k, 1).setText(dir[i].getSizeStr());
        (*dirTable)(k, 2).setText(dir[i].getModDate());
        k++;
    }
    std::string res = root.createDocument();
    std::copy(res.begin(), res.end(), std::back_inserter(buffer));
    return (0);
}

std::string getDateString( void )
{
    std::time_t t = std::time(0);
    std::tm* now = std::gmtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now);
    return (std::string(buff, size));
}

HttpResponse::HttpResponse( void ) : _isFlushed(true)
{ }

HttpResponse::HttpResponse( ClientSock& client, std::size_t timeoutS, std::size_t maxrequests) :
    statuscode(ws_http::STATUS_200_OK),
    _httpVersion(ws_http::VERSION_1_1),
    _client(&client), _isFlushed(false)
{
    this->setContentTypeLength("", 0);
    if (timeoutS == 0) {
        this->_headers.setHeader(ws_http::HEADER_CONNECTION, "close");
    } else {
        std::stringstream ss;
        ss << "timeout=" << timeoutS << ", max=" << maxrequests;
        this->_headers.setHeader(ws_http::HEADER_CONNECTION, ss.str());
    }
}

HttpResponse::~HttpResponse( void )
{ }

HttpHeaderParser&   HttpResponse::getHeaders( void ) { return (this->_headers); }

void    HttpResponse::setContentTypeLength( std::string const & mimeType, std::size_t len )
{
    if (!mimeType.empty())
        this->_headers.setHeader(ws_http::HEADER_CONTENT_TYPE, mimeType);
    std::stringstream ss;
    ss << len;
    this->_headers.setHeader(ws_http::HEADER_CONTENT_LENGTH, ss.str());
}

void    HttpResponse::findSetContentTypeLength( std::string const & fileExtension, std::size_t len )
{
    std::map<const std::string, const std::string>::const_iterator it;
    it = ws_http::mimetypes.find(fileExtension);
    if (it == ws_http::mimetypes.end())
        this->setContentTypeLength("application/octet-stream", len);
    else
        this->setContentTypeLength(it->second, len);
}

int HttpResponse::flushResponse( void )
{
    if (this->_isFlushed)
        return (-1);
    
    this->_resHeader.reserve(WS_HTTP_RES_DEFAULT_HEADER_SIZE);

    this->_headers.setHeader(ws_http::HEADER_DATE, getDateString());

    ws_http::writeStatusLine(ws_http::versions_rev.at(this->_httpVersion), ws_http::statuscodes.at(this->statuscode), this->_resHeader);
    this->_headers.writeHeadersToBuffer(this->_resHeader);
    
    ws_http::writeString(ws_http::crlf, this->_resHeader);
    this->_client->writeBuff(static_cast<void *>(this->_resHeader.data()), this->_resHeader.size());
    if (!this->_resBody.empty())
        this->_client->writeBuff(static_cast<void *>(this->_resBody.data()), this->_resBody.size());
    return (0);
}

int HttpResponse::sendDirectory(std::string const & rootPath, std::string const & dirPath)
{
    int ret = makeDirListing(rootPath, dirPath, this->_resBody);
    if (ret != 0)
        return (ret);
    this->setContentTypeLength("text/html", this->_resBody.size());
    this->flushResponse();
    return (0);
}

int HttpResponse::sendFile( std::string const & filePath )
{
    if (this->_isFlushed)
        return (-1);
    FileInfo currFile(filePath);
    if (!currFile.readStat())
        return (currFile.getError());
    if (currFile.isDir())
        return (EISDIR);
    else if (!currFile.isRegular() || !currFile.checkRealPermissions(R_OK))
        return (EACCES);
    std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open())
        return (errno);
    std::size_t filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    this->_resBody.clear();
    this->_resBody.resize(filesize);
    ifs.read(this->_resBody.data(), filesize);
    ifs.close();
    this->findSetContentTypeLength(ws_http::getFileExtension(filePath), filesize);
    this->flushResponse();
    return (0);
}

void    HttpResponse::sendText( std::string const & str )
{
    std::copy(str.begin(), str.end(), std::back_inserter(this->_resBody));
    this->setContentTypeLength("text/plain", str.size());
    this->flushResponse();
}

void    HttpResponse::sendBufferRange( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    std::cout << "send Buffer Ranger" << std::endl;
    std::cout << "distance start - end: " << std::distance(it_start, it_end) << std::endl;
    if (it_start != it_end)
        std::copy(it_start, it_end, std::back_inserter(this->_resBody));
    this->setContentTypeLength("", it_end - it_start);
    this->flushResponse();
}

void    HttpResponse::sendRedirect( ws_http::statuscodes_t redirectCode, std::string const & newLocation )
{
    this->statuscode = redirectCode;
    this->_headers.setHeader(ws_http::HEADER_LOCATION, newLocation);
    this->flushResponse();
}

int HttpResponse::sendError( ws_http::statuscodes_t errorCode, std::string const & errorPagePath )
{
    if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
        return (-1);
    this->statuscode = errorCode;

    int ret = 0;
    if (!errorPagePath.empty()) {
        ret = this->sendFile(errorPagePath);
        if (ret == 0)
            return (0);
        else if (ret == EACCES)
            errorCode = ws_http::STATUS_403_FORBIDDEN;
        else
            errorCode = ws_http::STATUS_404_NOT_FOUND;
    }
    const std::string & defaultError = ws_http::defaultErrorPages.at(errorCode);
    this->_resBody.clear();
    std::copy(defaultError.begin(), defaultError.end(), std::back_inserter(this->_resBody));
    this->setContentTypeLength("text/html", defaultError.size());
    this->flushResponse();
    return (ret);
}









// int checkPath(std::string const & filePath)
// {
//     struct stat fileStat;
//     if (stat(filePath.c_str(), &fileStat) == -1)
//         return (-1);
//     if (S_ISDIR(fileStat.st_mode) != 0)
//         return (0);
//     else if (S_ISREG(fileStat.st_mode) != 0)
//         return (1);
//     return (-1);
// }


// int HttpResponse::setFileToSend( std::string const & filePath )
// {
//     if (this->_isFlushed)
//         return (-1);

//     int type = checkPath(filePath);
//     if (type == -1)
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     else if (type == 0)
//         return (WS_HTTP_RESPONSE_ERROR_IS_DIR);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open()) {
//         if (errno == EACCES)
//             return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//         else
//             return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }

//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();

//     this->findSetContentTypeLength(getFileExtension(filePath), filesize);
//     return (0);
// }


// int HttpResponse::sendFile( std::string const & rootPath, std::string const & fileName )
// {
//     if (this->_isFlushed)
//         return (-1);
//     int retval = this->setFileToSend(rootPath + fileName);
//     if (retval == WS_HTTP_RESPONSE_ERROR_FILE_OK) {
//         this->flushResponse();
//         return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
//     }
//     if (retval == WS_HTTP_RESPONSE_ERROR_IS_DIR) {
//         if (!makeDirListing2(rootPath, fileName, this->_resBody)) {
//             this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         } else {
//             this->setContentTypeLength("text/html", this->_resBody.size());
//             this->flushResponse();
//         }
//     }
//     else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION) {
//         this->sendError(ws_http::STATUS_403_FORBIDDEN);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//     } else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND) {
//         this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }
//     return (-1);
// }





// int HttpResponse::sendDefaultError( ws_http::statuscodes_t errorCode )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     const std::string & defaultError = ws_http::defaultErrorPages.at(errorCode);
//     this->_resBody.clear();
//     std::copy(defaultError.begin(), defaultError.end(), std::back_inserter(this->_resBody));
//     this->setContentTypeLength("text/html", defaultError.size());
//     this->flushResponse();
//     return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
// }

// int HttpResponse::sendError( ws_http::statuscodes_t errorCode, std::string const & errorPagePath )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     int setFileRet = WS_HTTP_RESPONSE_ERROR_FILE_OK;
//     if (!errorPagePath.empty()) {
//         setFileRet = this->setFileToSend(errorPagePath);
//         if (setFileRet == WS_HTTP_RESPONSE_ERROR_FILE_OK)
//             this->flushResponse();
//         else {
//             this->sendDefaultError(errorCode);
//         }
//     } else {
//         this->sendDefaultError(errorCode);
//     }
//     return (setFileRet);
// }









// std::string getDateString( void )
// {
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now);
//     return (std::string(buff, size));
// }

// HttpResponse::HttpResponse( ClientSock& client ) :
//     statuscode(ws_http::STATUS_200_OK),
//     _httpVersion(ws_http::VERSION_1_1),
//     _client(&client), _isFlushed(false)
// {
//     this->setContentTypeLength("", 0);
// }

// HttpResponse::~HttpResponse( void )
// { }

// HttpHeaderParser&   HttpResponse::getHeaders( void )
// {
//     return (this->_headers);
// }

// std::string getFileExtension( const std::string& filePath ) {
//     size_t lastDotPosition = filePath.find_last_of(".");
//     if (lastDotPosition != std::string::npos) {
//         return (filePath.substr(lastDotPosition + 1));
//     }
//     return ("");
// }



// void    HttpResponse::setContentTypeLength( std::string const & mimeType, std::size_t len )
// {
//     if (!mimeType.empty())
//         this->_headers.setHeader(ws_http::HEADER_CONTENT_TYPE, mimeType);
//     std::stringstream ss;
//     ss << len;
//     this->_headers.setHeader(ws_http::HEADER_CONTENT_LENGTH, ss.str());
// }

// void    HttpResponse::findSetContentTypeLength( std::string const & fileExtension, std::size_t len )
// {
//     std::map<const std::string, const std::string>::const_iterator it;
//     it = ws_http::mimetypes.find(fileExtension);
//     if (it == ws_http::mimetypes.end())
//         this->setContentTypeLength("application/octet-stream", len);
//     else
//         this->setContentTypeLength(it->second, len);
// }

// int checkPath(std::string const & filePath)
// {
//     struct stat fileStat;
//     if (stat(filePath.c_str(), &fileStat) == -1)
//         return (-1);
//     if (S_ISDIR(fileStat.st_mode) != 0)
//         return (0);
//     else if (S_ISREG(fileStat.st_mode) != 0)
//         return (1);
//     return (-1);
// }

// int HttpResponse::setFileToSend( std::string const & filePath )
// {
//     if (this->_isFlushed)
//         return (-1);

//     int type = checkPath(filePath);
//     if (type == -1)
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     else if (type == 0)
//         return (WS_HTTP_RESPONSE_ERROR_IS_DIR);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open()) {
//         if (errno == EACCES)
//             return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//         else
//             return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }

//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();

//     this->findSetContentTypeLength(getFileExtension(filePath), filesize);
//     return (0);
// }

// int HttpResponse::flushResponse( void )
// {
//     if (this->_isFlushed)
//         return (-1);
    
//     this->_resHeader.reserve(WS_HTTP_RES_DEFAULT_HEADER_SIZE);

//     this->_headers.setHeader(ws_http::HEADER_DATE, getDateString());
//     this->_headers.setHeader(ws_http::HEADER_CONNECTION, "timeout=10, max=1000");

//     ws_http::writeStatusLine(ws_http::versions_rev.at(this->_httpVersion), ws_http::statuscodes.at(this->statuscode), this->_resHeader);
//     this->_headers.writeHeadersToBuffer(this->_resHeader);
    
//     ws_http::writeString(ws_http::crlf, this->_resHeader);
//     this->_client->writeBuff(static_cast<void *>(this->_resHeader.data()), this->_resHeader.size());
//     if (!this->_resBody.empty())
//         this->_client->writeBuff(static_cast<void *>(this->_resBody.data()), this->_resBody.size());
//     return (0);
// }

// std::string    makeDirListing(std::string const & rootDir, std::string const & currDir );

// int HttpResponse::sendFile( std::string const & rootPath, std::string const & fileName )
// {
//     if (this->_isFlushed)
//         return (-1);
//     int retval = this->setFileToSend(rootPath + fileName);
//     if (retval == WS_HTTP_RESPONSE_ERROR_FILE_OK) {
//         this->flushResponse();
//         return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
//     }
//     if (retval == WS_HTTP_RESPONSE_ERROR_IS_DIR) {
//         std::string doc = makeDirListing(rootPath, fileName);
//         if (doc.empty()) {
//             this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         } else {
//             this->_resBody.clear();
//             std::copy(doc.begin(), doc.end(), std::back_inserter(this->_resBody));
//             this->setContentTypeLength("text/html", doc.size());
//             this->flushResponse();
//         }
//     }
//     else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION) {
//         this->sendError(ws_http::STATUS_403_FORBIDDEN);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//     } else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND) {
//         this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }
//     return (-1);
// }

// int HttpResponse::sendDefaultError( ws_http::statuscodes_t errorCode )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     const std::string & defaultError = ws_http::defaultErrorPages.at(errorCode);
//     this->_resBody.clear();
//     std::copy(defaultError.begin(), defaultError.end(), std::back_inserter(this->_resBody));
//     this->setContentTypeLength("text/html", defaultError.size());
//     this->flushResponse();
//     return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
// }

// int HttpResponse::sendError( ws_http::statuscodes_t errorCode, std::string const & errorPagePath )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     int setFileRet = WS_HTTP_RESPONSE_ERROR_FILE_OK;
//     if (!errorPagePath.empty()) {
//         setFileRet = this->setFileToSend(errorPagePath);
//         if (setFileRet == WS_HTTP_RESPONSE_ERROR_FILE_OK)
//             this->flushResponse();
//         else {
//             this->sendDefaultError(errorCode);
//         }
//     } else {
//         this->sendDefaultError(errorCode);
//     }
//     return (setFileRet);
// }

// int HttpResponse::sendText( std::string const & str )
// {
//     std::copy(str.begin(), str.end(), std::back_inserter(this->_resBody));
//     this->setContentTypeLength("text/plain", str.size());
//     this->flushResponse();
//     return (0);
// }












































// std::string getDateString( void )
// {
//     std::string wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//     std::string mon[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
    
//     std::stringstream   ss;
//     ss << std::setfill('0')
//     << wdays[now->tm_wday] << ", "
//     << std::setw(2) << now->tm_mday << " "
//     << mon[now->tm_mon] << " "
//     << (now->tm_year + 1900) << " "
//     << std::setw(2) << now->tm_hour << ":"
//     << std::setw(2) << now->tm_min << ":" 
//     << std::setw(2) << now->tm_sec << " "
//     << "GMT";
//     return (ss.str());
// }

// std::string getDateString( void )
// {
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now);
//     return (std::string(buff, size));
// }

// HttpResponse::HttpResponse( ClientSock& client ) :
//     statuscode(ws_http::STATUS_200_OK),
//     _httpVersion(ws_http::VERSION_1_1),
//     _client(&client), _isFlushed(false), _contentLength(0)
// { }

// HttpResponse::~HttpResponse( void )
// { }

// void HttpResponse::setHeader( ws_http::header_field_t headerField, std::string headerValue )
// {
//     if (this->_isFlushed)
//         return ;
//     this->_resHeaderFields.insert(std::make_pair(headerField, headerValue));
// }

// void HttpResponse::setHeader( std::string headerField, std::string headerValue )
// {
//     if (this->_isFlushed)
//         return ;
//     this->_resCustomHeaderFields.insert(std::make_pair(headerField, headerValue));
// }

// bool    HttpResponse::removeHeader( ws_http::header_field_t headerField )
// {
//     if (this->_isFlushed)
//         return (false);
//     std::map<ws_http::header_field_t, std::string>::iterator it;
//     it = this->_resHeaderFields.find(headerField);
//     if (it == this->_resHeaderFields.end())
//         return (false);
//     this->_resHeaderFields.erase(it);
//     return (true);
// }

// bool    HttpResponse::removeHeader( std::string headerField )
// {
//     if (this->_isFlushed)
//         return (false);
//     std::map<std::string, std::string, ws_http::CaInCmp>::iterator it;
//     it = this->_resCustomHeaderFields.find(headerField);
//     if (it == this->_resCustomHeaderFields.end())
//         return (false);
//     this->_resCustomHeaderFields.erase(it);        
//     return (true);
// }

// const std::string&  HttpResponse::getHeader( ws_http::header_field_t headerField ) const
// {
//     std::map<ws_http::header_field_t, std::string>::const_iterator it;
//     it = this->_resHeaderFields.find(headerField);
//     if (it == this->_resHeaderFields.end())
//         return (this->_dummyEmptyHeader);
//     return (it->second);
// }

// const std::string&  HttpResponse::getHeader( std::string headerField ) const
// {
//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator it;
//     it = this->_resCustomHeaderFields.find(headerField);
//     if (it == this->_resCustomHeaderFields.end())
//         return (this->_dummyEmptyHeader);
//     return (it->second);
// }

// bool    HttpResponse::headerIsSet( ws_http::header_field_t headerField ) const
// {
//     return (!this->getHeader(headerField).empty());
// }

// bool    HttpResponse::headerIsSet( std::string headerField  ) const
// {
//     return (!this->getHeader(headerField).empty());
// }


// std::string getFileExtension( const std::string& filePath ) {
//     size_t lastDotPosition = filePath.find_last_of(".");
//     if (lastDotPosition != std::string::npos) {
//         return (filePath.substr(lastDotPosition + 1));
//     }
//     return ("");
// }

// int checkPath(std::string const & filePath)
// {
//     struct stat fileStat;
//     if (stat(filePath.c_str(), &fileStat) == -1)
//         return (-1);
//     if (S_ISDIR(fileStat.st_mode) != 0)
//         return (0);
//     else if (S_ISREG(fileStat.st_mode) != 0)
//         return (1);
//     return (-1);
// }

// int HttpResponse::setFileToSend( std::string const & filePath )
// {
//     if (this->_isFlushed)
//         return (-1);

//     int type = checkPath(filePath);
//     if (type == -1)
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     else if (type == 0)
//         return (WS_HTTP_RESPONSE_ERROR_IS_DIR);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open()) {
//         if (errno == EACCES)
//             return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//         else
//             return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }

//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();

//     this->_contentLength = filesize;
//     std::map<const std::string, const std::string>::const_iterator it;
//     it = ws_http::mimetypes.find(getFileExtension(filePath));
//     if (it == ws_http::mimetypes.end())
//         this->setHeader(ws_http::HEADER_CONTENT_TYPE, "application/octet-stream");
//     this->setHeader(ws_http::HEADER_CONTENT_TYPE, it->second);
//     return (0);
// }

// int writeString( std::string const & str, buff_t& writeto )
// {
//     std::copy(str.begin(), str.end(), std::back_inserter(writeto));
//     return (0);
// }

// int HttpResponse::flushResponse( void )
// {
//     if (this->_isFlushed)
//         return (-1);
    
//     this->_resHeader.reserve(WS_HTTP_RES_DEFAULT_HEADER_SIZE);
//     std::stringstream ss;
//     ss << this->_contentLength;
//     this->setHeader(ws_http::HEADER_CONTENT_LENGTH, ss.str());
//     this->setHeader(ws_http::HEADER_DATE, getDateString());
//     // this->setHeader(ws_http::HEADER_CONNECTION, "close");
//     this->setHeader(ws_http::HEADER_CONNECTION, "timeout=10, max=1000");

//     writeString(ws_http::versions_rev.at(this->_httpVersion), this->_resHeader);
//     this->_resHeader.push_back(' ');
//     writeString(ws_http::statuscodes.at(this->statuscode), this->_resHeader);
//     writeString(ws_http::crlf, this->_resHeader);

//     std::map<ws_http::header_field_t, std::string>::const_iterator it_header;
//     for (it_header = this->_resHeaderFields.begin(); it_header != this->_resHeaderFields.end(); ++it_header) {
//         writeString(ws_http::headers_rev.at(it_header->first), this->_resHeader);
//         this->_resHeader.push_back(':');
//         this->_resHeader.push_back(' ');
//         writeString(it_header->second, this->_resHeader);
//         writeString(ws_http::crlf, this->_resHeader);   
//     }

//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator it_header_custom;
//     for (it_header_custom = this->_resCustomHeaderFields.begin(); it_header_custom != this->_resCustomHeaderFields.end(); ++it_header_custom) {
//         writeString(it_header_custom->first, this->_resHeader);
//         this->_resHeader.push_back(':');
//         this->_resHeader.push_back(' ');
//         writeString(it_header_custom->second, this->_resHeader);
//         writeString(ws_http::crlf, this->_resHeader);   
//     }

//     writeString(ws_http::crlf, this->_resHeader);
//     this->_client->writeBuff(static_cast<void *>(this->_resHeader.data()), this->_resHeader.size());
//     if (!this->_resBody.empty())
//         this->_client->writeBuff(static_cast<void *>(this->_resBody.data()), this->_resBody.size());
//     // this->_client->closeConnect();
//     return (0);
// }

// std::string    makeDirListing(std::string const & rootDir, std::string const & currDir );

// int HttpResponse::sendFile( std::string const & rootPath, std::string const & fileName )
// {
//     if (this->_isFlushed)
//         return (-1);
//     int retval = this->setFileToSend(rootPath + fileName);
//     if (retval == WS_HTTP_RESPONSE_ERROR_FILE_OK) {
//         this->flushResponse();
//         return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
//     }
//     if (retval == WS_HTTP_RESPONSE_ERROR_IS_DIR) {
//         std::string doc = makeDirListing(rootPath, fileName);
//         if (doc.empty()) {
//             this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         } else {
//             this->_resBody.clear();
//             std::copy(doc.begin(), doc.end(), std::back_inserter(this->_resBody));
//             this->setHeader(ws_http::HEADER_CONTENT_TYPE, "text/html");
//             this->_contentLength = doc.size();
//             this->flushResponse();
//         }
//     }
//     else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION) {
//         this->sendError(ws_http::STATUS_403_FORBIDDEN);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION);
//     } else if (retval == WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND) {
//         this->sendError(ws_http::STATUS_404_NOT_FOUND);
//         return (WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND);
//     }
//     return (-1);
// }

// int HttpResponse::sendDefaultError( ws_http::statuscodes_t errorCode )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     const std::string & defaultError = ws_http::defaultErrorPages.at(errorCode);
//     this->setHeader(ws_http::HEADER_CONTENT_TYPE, "text/html");
//     this->_contentLength = defaultError.size();
//     this->_resBody.clear();
//     std::copy(defaultError.begin(), defaultError.end(), std::back_inserter(this->_resBody));
//     this->flushResponse();
//     return (WS_HTTP_RESPONSE_ERROR_FILE_OK);
// }

// int HttpResponse::sendError( ws_http::statuscodes_t errorCode, std::string const & errorPagePath )
// {
//     if (this->_isFlushed || errorCode < ws_http::STATUS_400_BAD_REQUEST)
//         return (-1);
//     this->statuscode = errorCode;
//     int setFileRet = WS_HTTP_RESPONSE_ERROR_FILE_OK;
//     if (!errorPagePath.empty()) {
//         setFileRet = this->setFileToSend(errorPagePath);
//         if (setFileRet == WS_HTTP_RESPONSE_ERROR_FILE_OK)
//             this->flushResponse();
//         else {
//             this->sendDefaultError(errorCode);
//         }
//     } else {
//         this->sendDefaultError(errorCode);
//     }
//     return (setFileRet);
// }

// int HttpResponse::sendText( std::string const & str )
// {
//     std::copy(str.begin(), str.end(), std::back_inserter(this->_resBody));
//     this->setHeader(ws_http::HEADER_CONTENT_TYPE, "text/plain");
//     this->_contentLength = str.size();
//     this->flushResponse();
//     return (0);
// }
