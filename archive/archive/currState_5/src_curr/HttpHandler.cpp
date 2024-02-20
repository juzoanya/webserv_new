/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 09:36:27 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

HttpContent::HttpContent( void )
 : headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
{ }

HttpContent::HttpContent( const buff_t* headerBuffer, const buff_t* bodyBuffer )
{
    this->headerPtr = headerBuffer ? headerBuffer->data() : NULL;
    this->headerSize = headerBuffer ? headerBuffer->size() : 0;
    this->bodyPtr = bodyBuffer ? bodyBuffer->data() : NULL;
    this->bodySize = bodyBuffer ? bodyBuffer->size() : 0;
}

HttpContent::~HttpContent( void )
{ }

int HttpContent::sendDataToSocket( int socketfd, int flags )
{
    int writeByte = 0;
    if (this->headerSize > 0) {
        writeByte = send(socketfd, this->headerPtr, this->headerSize, flags);
        if (writeByte > 0) {
            this->headerPtr += writeByte;
            this->headerSize -= writeByte;
        }
    } else if (this->bodySize > 0) {
        writeByte = send(socketfd, this->bodyPtr, this->bodySize, flags);
        if (writeByte > 0) {
            this->bodyPtr += writeByte;
            this->bodySize -= writeByte;
        }
    } 
    return (writeByte);
}

bool    HttpContent::empty( void ) const
{
    return (this->bodySize == 0 && this->headerSize == 0);
}


ws_http::statuscodes_t HttpContent::setContent( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType )
{
    std::cout << "-- setHeader\n";
    this->_bodyBuffer.reserve(4096);
    std::stringstream ss;
    ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
    ss << "connection: close" << ws_http::crlf;
    std::size_t httpBodyStreamSize = getStreamSize(httpBodyStream);
    ss << "content-length: " << httpBodyStreamSize << ws_http::crlf;
    ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
    ss << ws_http::crlf;

    this->_headerBuffer.clear();
    this->_bodyBuffer.clear();
    addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
    addStreamToBuff(this->_bodyBuffer, httpBodyStream, httpBodyStreamSize);
    this->headerPtr = this->_headerBuffer.data();
    this->headerSize = this->_headerBuffer.size();
    this->bodyPtr = this->_bodyBuffer.data();
    this->bodySize = this->_bodyBuffer.size();
}

ws_http::statuscodes_t  HttpContent::setDefaultError( ws_http::statuscodes_t errorStatusCode )
{
    std::stringstream ss;
    ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
        << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" "</head>" 
        << "<body>" "<div style=\"text-align: center;\">" 
        << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>"
        << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
        << "</body>" "</html>";
    return(this->setContent(errorStatusCode, ss, "text/html"));
}

ws_http::statuscodes_t  HttpContent::setRedirection( ws_http::statuscodes_t status, std::string const & location )
{
   std::cout << "-- setRedirection\n";
    std::stringstream ss;
    ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
    ss << "connection: close" << ws_http::crlf;
    ss << "location: " << location << ws_http::crlf;
    ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
    ss << ws_http::crlf;

    this->_headerBuffer.clear();
    addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
    this->headerPtr = this->_headerBuffer.data();
    this->headerSize = this->_headerBuffer.size();
}


AHttpHandler::AHttpHandler( void )
{ }

AHttpHandler::AHttpHandler( HttpContent const & data ) 
// : dataContent(data)
{ }

AHttpHandler::~AHttpHandler( void )
{ }

HttpHandler::HttpHandler( ConfigHandler& configHandler, WsIpPort const & ipPort )
 : ipPortData(ipPort),  _request(&configHandler, true), _configHandler(&configHandler), _intRedirectNbr(0)
{ }

HttpHandler::~HttpHandler( void )
{ }

// ws_http::statuscodes_t  HttpHandler::setHeader( ws_http::statuscodes_t status, std::istream* httpBodyStream, std::string const & mimeType, std::string const & redirectLocation )
// {
//     std::cout << "-- setHeader\n";
//     this->_fileData.reserve(4096);
//     std::stringstream ss;
//     std::size_t httpBodyStreamSize = 0;
//     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
//     ss << "connection: close" << ws_http::crlf;
//     if (!redirectLocation.empty())
//         ss << "location: " << redirectLocation << ws_http::crlf;
//     if (!mimeType.empty())
//         ss << "content-type: " << mimeType << ws_http::crlf;
//     if (httpBodyStream) {
//         httpBodyStreamSize = getStreamSize(*httpBodyStream);
//         if (httpBodyStreamSize > 0)
//             ss << "content-length: " << httpBodyStreamSize << ws_http::crlf;
//     }
//     ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
//     ss << ws_http::crlf;

//     this->_headerBuffer.clear();
//     this->_fileData.clear();
//     addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
//     if (httpBodyStream && httpBodyStreamSize > 0)
//         addStreamToBuff(this->_fileData, *httpBodyStream, httpBodyStreamSize);
//     this->dataContent = HttpContent(&this->_headerBuffer, &this->_fileData);
//     // this->dataPtr = this->_fileData.data();
//     // this->dataSize = this->_fileData.size();
//     return (status);
// }

HttpHandler::CgiHandler *    HttpHandler::getCgiProcessInstance( void )
{
    return (new HttpHandler::CgiHandler(*this, this->_request, this->_fileData, this->_headerBuffer));
}

ws_http::statuscodes_t    HttpHandler::handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    ws_http::statuscodes_t status = this->_request.parseRequest(it_start, it_end);
    std::cout << "HttpHandler::handleData, statusCode parsing the Request: " << ws_http::statuscodes.at(status) << std::endl;
    if (status == ws_http::STATUS_UNDEFINED)
        return (ws_http::STATUS_UNDEFINED);
    this->_request.printRequest();
    if (status == ws_http::STATUS_200_OK)
        status = this->processResponse(ws_http::STATUS_200_OK, this->_request.method, this->_request.pathDecoded, false);
    else
        status = this->setError(this->_request.status, false);
    this->_request = HttpRequest(this->_configHandler, true);
    return (status);
}


ws_http::statuscodes_t    HttpHandler::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, bool internal )
{
    std::cout << "\n---------------- HttpHandler::processResponse ----------------\n"
        << "status: " << ws_http::statuscodes.at(statusCode)
        << " | method: " << method << " | requestPathDecoded: "
        << requestPathDecoded << " | internal: "
        << (internal ? "true" : "false")
        << std::endl;

    if (internal)
        this->_intRedirectNbr++;
    if (this->_intRedirectNbr == 10) {
        this->_intRedirectNbr = 0;
        return (this->setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, internal));
    }

    HttpConfig config = this->_configHandler->getHttpConfig(requestPathDecoded, this->_request.hostHeader);
    
    Redirect redirect = this->_httpConfig.getRedirection();
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
        return (this->setHeader(redirect.redirectStatus, NULL, "", redirect.location));
    if (!this->_httpConfig.checkAllowedMethod(method))
        return (this->setError(ws_http::STATUS_405_METHOD_NOT_ALLOWED, internal));
    std::string const & cgiExecutablePath = this->_httpConfig.getCgiExecutable();
    if (!cgiExecutablePath.empty())
        return (ws_http::STATUS_UNDEFINED);

    FileInfos file(this->_httpConfig.getFilePath(), this->_request.pathDecoded);
    if (file.getError() == 0 && S_ISREG(file.getMode())) {
        std::cout << "----> is a regular file" << std::endl;
        std::ifstream   ifs(this->_httpConfig.getFilePath().c_str(), std::ios::binary | std::ios::ate);
        if (ifs.is_open()) {
            this->setHeader(statusCode, &ifs, this->_httpConfig.getMimeType());
            ifs.close();
            return (statusCode);
        }
    } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
        std::cout << "----> is a directory" << std::endl;
        std::string const & indexFileName = file.checkContainedFile(this->_httpConfig.getIndexFile());
        std::cout << "----> indexFileName:" << indexFileName << std::endl;
        if (!indexFileName.empty())
            return (this->processResponse(statusCode, method, requestPathDecoded + "/" + indexFileName, true));
        if (!this->_httpConfig.hasDirectoryListing())
            return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
        std::stringstream ss;
        file.setDirListing(ss);
        return (this->setHeader(statusCode, &ss, "text/html"));
    } else if (file.getError() == EACCES) {
        std::cout << "----> permission denied" << std::endl;
        return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
    }
    std::cout << "----> not found" << std::endl;
    return (this->setError(ws_http::STATUS_404_NOT_FOUND, internal));
}


ws_http::statuscodes_t    HttpHandler::setError( ws_http::statuscodes_t errorStatusCode, bool internal )
{
    std::cout << "-- setError" << std::endl;
    std::string const & errorPagePath = this->_httpConfig.getErrorPage(errorStatusCode);
    if (!internal && !errorPagePath.empty())
        return (this->processResponse(errorStatusCode, "GET", errorPagePath, true));
    std::stringstream ss;
    ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
        << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" "</head>" 
        << "<body>" "<div style=\"text-align: center;\">" 
        << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>"
        << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
        << "</body>" "</html>";
    // ss << "<!doctype html>" << ws_http::crlf << "<html lang=\"en\">" << ws_http::crlf
    //     << "<head>" << ws_http::crlf << "<meta charset=\"utf-8\">" << ws_http::crlf
    //     << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" << ws_http::crlf << "</head>" << ws_http::crlf
    //     << "<body>" << ws_http::crlf << "<div style=\"text-align: center;\">" << ws_http::crlf 
    //     << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>" << ws_http::crlf
    //     << "<hr>" << ws_http::crlf << "<p>" << ws_http::webservVersion << "</p>" << ws_http::crlf << "</div>" << ws_http::crlf
    //     << "</body>" << ws_http::crlf << "</html>" << ws_http::crlf;
    this->setHeader(errorStatusCode, &ss, "text/html");
    return (errorStatusCode);
}



int HttpHandler::CgiHandler::parentsock = 0;
int HttpHandler::CgiHandler::childsock = 1;

int charToupperUnderscore( int c )
{
    if (c == '-')
        return ('_');
    return (::toupper(c));
}


HttpHandler::CgiHandler::CgiHandler( HttpHandler & httpHandler, HttpRequest & request, buff_t & resultBodyBuffer, buff_t & resultHeaderBuffer )
 : AHttpHandler(HttpContent(NULL, &request.requestBody.bodyBuffer)), _requestHelper(NULL, false), _httpHandler(httpHandler), _resultHeaderBuffer(resultHeaderBuffer), _resultBodyBuffer(resultBodyBuffer)
{
    this->_cgiArgs.push_back(httpHandler._httpConfig.getCgiExecutable());
    this->_cgiArgs.push_back(httpHandler._httpConfig.getFilePath());

    http_header_map_t::const_iterator headerIt;
    this->_cgiEnv.push_back(std::string("QUERY_STRING=") + request.query);
    this->_cgiEnv.push_back(std::string("REQUEST_METHOD=") + request.method);
    headerIt = request.headerMap.find("content-type");
    if (headerIt == request.headerMap.end())
        this->_cgiEnv.push_back(std::string("CONTENT_TYPE=") + headerIt->second);
    else
        this->_cgiEnv.push_back(std::string("CONTENT_TYPE="));
    this->_cgiEnv.push_back(std::string("CONTENT_LENGTH=") + toStr(request.requestBody.bodyBuffer.size()));
    this->_cgiEnv.push_back(std::string("SCRIPT_FILE_NAME=") + httpHandler._httpConfig.getFilePath());
    this->_cgiEnv.push_back(std::string("SCRIPT_NAME=") + request.pathDecoded);
    this->_cgiEnv.push_back(std::string("PATH_INFO="));
    this->_cgiEnv.push_back(std::string("PATH_TRANSLATED=") + httpHandler._httpConfig.getRootPath());
    this->_cgiEnv.push_back(std::string("REQUEST_URI=") + request.fullUri);
    this->_cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    this->_cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
    this->_cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
    this->_cgiEnv.push_back(std::string("REMOTE_ADDR=") + httpHandler.ipPortData.getIpStr());
    this->_cgiEnv.push_back(std::string("REMOTE_PORT=") + httpHandler.ipPortData.getPortStr());
    this->_cgiEnv.push_back(std::string("SERVER_ADDR=") + httpHandler._configHandler->ipPortData.getIpStr());
    this->_cgiEnv.push_back(std::string("SERVER_PORT=") + httpHandler._configHandler->ipPortData.getPortStr());
    this->_cgiEnv.push_back(std::string("SERVER_NAME=") + httpHandler._httpConfig.getServerName());
    this->_cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));

    for (http_header_map_t::const_iterator it = request.headerMap.begin(); it != request.headerMap.end(); ++it) {
        std::string cgiHeader = "HTTP_";
        std::transform(it->first.begin(), it->first.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
        this->_cgiEnv.push_back(cgiHeader + "=" + it->second);
    }



    // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
    //     this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
    // this->_enviroment.push_back(NULL);
    // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
    //     this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
    // this->_arguments.push_back(NULL);
}

HttpHandler::CgiHandler::~CgiHandler( void )
{

}

ws_http::statuscodes_t    HttpHandler::CgiHandler::handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    // if (it_start == it_end) {
    //     std::stringstream ss;
    //     http_header_map_t::const_iterator itStatus = this->_requestHelper.headerMap.find("status");
    //     http_header_map_t::const_iterator itDate = this->_requestHelper.headerMap.find("date");
    //     http_header_map_t::const_iterator itContType = this->_requestHelper.headerMap.find("content-type");
    //     if (itStatus == this->_requestHelper.headerMap.end() || itContType == this->_requestHelper.headerMap.end())
    //         return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
    //     std::map<const std::string, ws_http::statuscodes_t>::const_iterator status = ws_http::statuscodes_rev.find(itStatus->second);
    //     if (status == ws_http::statuscodes_rev.end())
    //         return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
    //     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status->second) << ws_http::crlf;
    //     ss << "connection: close" << ws_http::crlf;
    //     ss << "content-type: " << itContType->second << ws_http::crlf;
    //     if (this->_resultBodyBuffer.size() > 0)
    //         ss << "content-length: " << this->_resultBodyBuffer.size() << ws_http::crlf;
    //     if (itDate ==  this->_requestHelper.headerMap.end())
    //         ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;

    //     http_header_map_t::const_iterator it;
    //     for (it = this->_requestHelper.headerMap.begin(); it != this->_requestHelper.headerMap.end(); ++it) {
    //         if (it != itStatus && it != itDate && it != itContType)
    //             ss << it->first << ": " << it->second << ws_http::crlf;
    //     }
    //     ss << ws_http::crlf;

    //     this->_resultHeaderBuffer.clear();
    //     this->_resultBodyBuffer.clear();
    //     addStreamToBuff(this->_resultHeaderBuffer, ss, getStreamSize(ss));
    //     this->_httpHandler.dataContent = HttpContent(&this->_resultHeaderBuffer, &this->_resultBodyBuffer);
    //     return (status->second);
    // }
    // if (this->_requestHelper.status == ws_http::STATUS_200_OK) {
    //     std::copy(this->_requestHelper.endPos, it_end, std::back_inserter(this->_resultBodyBuffer));
    // } else {
    //     this->_requestHelper.parseRequest(it_start, it_end);
    //     if (this->_requestHelper.status == ws_http::STATUS_200_OK) {
    //         std::copy(this->_requestHelper.endPos, it_end, std::back_inserter(this->_resultBodyBuffer));
    //     } else if (this->_requestHelper.status != ws_http::STATUS_UNDEFINED) {
    //         return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
    //     }
    // }
    // return (ws_http::STATUS_UNDEFINED);

    bool headerStatus = false, headerContType = false, headerDate = false;
    for (buff_t::const_iterator pos = it_start; it_start < it_end; it_start = pos + ws_http::crlf.size()) {
        pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        std::string headerField = std::string(it_start, pos);
        std::transform(headerField.begin(), headerField.end(), headerField.begin(), ::tolower);
    }
    while (true) {
        buff_t::const_iterator pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    }
}

int    HttpHandler::CgiHandler::createChildProcess( void )
{
    // char *envArr[this->_cgiEnv.size() + 1];
    // char *argArr[this->_cgiArgs.size() + 1];
    
    // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
    //     envArr[i] = const_cast<char*>(this->_cgiEnv[i].c_str());
    // envArr[this->_cgiEnv.size()] = NULL;
    // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
    //     argArr[i] = const_cast<char*>(this->_cgiArgs[i].c_str());
    // argArr[this->_cgiArgs.size()] = NULL;

    int sock_pair[2];
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
        std::cout << "error: socketpair: " << strerror(errno) << std::endl;
        return (-1);
    }
    if (fcntl(sock_pair[HttpHandler::CgiHandler::childsock], F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
        close(sock_pair[this->childsock]);
        close(sock_pair[HttpHandler::CgiHandler::parentsock]);
        return (-1);
    }
    if (fcntl(sock_pair[HttpHandler::CgiHandler::parentsock], F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
        close(sock_pair[HttpHandler::CgiHandler::childsock]);
        close(sock_pair[HttpHandler::CgiHandler::parentsock]);
        return (-1);
    }
    pid_t pid = fork();
    if (pid == 1) {
        std::cout << "error: fork: " << strerror(errno) << std::endl;
        close(sock_pair[HttpHandler::CgiHandler::childsock]);
        close(sock_pair[HttpHandler::CgiHandler::parentsock]);
        return (-1);
    } else if (pid == 0) {
        close(sock_pair[HttpHandler::CgiHandler::parentsock]);
        dup2(sock_pair[HttpHandler::CgiHandler::childsock], STDOUT_FILENO);
        dup2(sock_pair[HttpHandler::CgiHandler::childsock], STDIN_FILENO);
        // if (chdir(docRoot.c_str()) == -1) {
        //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
        //     exit(1);
        // }

        char *envArr[this->_cgiEnv.size() + 1];
        char *argArr[this->_cgiArgs.size() + 1];
        
        for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
            envArr[i] = const_cast<char*>(this->_cgiEnv[i].c_str());
        envArr[this->_cgiEnv.size()] = NULL;
        for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
            argArr[i] = const_cast<char*>(this->_cgiArgs[i].c_str());
        argArr[this->_cgiArgs.size()] = NULL;
        execve(argArr[0], argArr, envArr);
        std::cout << "error: execve: " << strerror(errno) << std::endl;
        close(sock_pair[HttpHandler::CgiHandler::childsock]);
        exit(1);
    } else {
        close(sock_pair[HttpHandler::CgiHandler::childsock]);
    }
    return (sock_pair[HttpHandler::CgiHandler::parentsock]);
}


// ws_http::statuscodes_t    HttpHandler::CgiHandler::handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// {
//     if (it_start == it_end) {
//         std::stringstream ss;
//         http_header_map_t::const_iterator itStatus = this->_requestHelper.headerMap.find("status");
//         http_header_map_t::const_iterator itDate = this->_requestHelper.headerMap.find("date");
//         http_header_map_t::const_iterator itContType = this->_requestHelper.headerMap.find("content-type");
//         if (itStatus == this->_requestHelper.headerMap.end() || itContType == this->_requestHelper.headerMap.end())
//             return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
//         std::map<const std::string, ws_http::statuscodes_t>::const_iterator status = ws_http::statuscodes_rev.find(itStatus->second);
//         if (status == ws_http::statuscodes_rev.end())
//             return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
//         ss << "HTTP/1.1 " << ws_http::statuscodes.at(status->second) << ws_http::crlf;
//         ss << "connection: close" << ws_http::crlf;
//         ss << "content-type: " << itContType->second << ws_http::crlf;
//         if (this->_resultBodyBuffer.size() > 0)
//             ss << "content-length: " << this->_resultBodyBuffer.size() << ws_http::crlf;
//         if (itDate ==  this->_requestHelper.headerMap.end())
//             ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;

//         http_header_map_t::const_iterator it;
//         for (it = this->_requestHelper.headerMap.begin(); it != this->_requestHelper.headerMap.end(); ++it) {
//             if (it != itStatus && it != itDate && it != itContType)
//                 ss << it->first << ": " << it->second << ws_http::crlf;
//         }
//         ss << ws_http::crlf;

//         this->_resultHeaderBuffer.clear();
//         this->_resultBodyBuffer.clear();
//         addStreamToBuff(this->_resultHeaderBuffer, ss, getStreamSize(ss));
//         this->_httpHandler.dataContent = HttpContent(&this->_resultHeaderBuffer, &this->_resultBodyBuffer);
//         return (status->second);
//     }
//     if (this->_requestHelper.status == ws_http::STATUS_200_OK) {
//         std::copy(this->_requestHelper.endPos, it_end, std::back_inserter(this->_resultBodyBuffer));
//     } else {
//         this->_requestHelper.parseRequest(it_start, it_end);
//         if (this->_requestHelper.status == ws_http::STATUS_200_OK) {
//             std::copy(this->_requestHelper.endPos, it_end, std::back_inserter(this->_resultBodyBuffer));
//         } else if (this->_requestHelper.status != ws_http::STATUS_UNDEFINED) {
//             return (this->_httpHandler.setError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, false));
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }



// HttpStaticHandler::HttpStaticHandler( void )
// { }

// HttpStaticHandler::~HttpStaticHandler( void )
// { }


// ws_http::statuscodes_t  HttpHandler::handleGetHead( ws_http::statuscodes_t statusCode, std::string const & method, bool internal )
// {
//     (void)method;
//     HttpStatic::file_t fileStat = HttpStatic::checkFileStat(this->_httpConfig.getFilePath(), NULL, R_OK);
//     if (fileStat == WS_NO_ACCESS) {
//         return (this->setError(ws_http::STATUS_403_FORBIDDEN, internal));
//     } else if (fileStat == WS_REGULAR && this->setFile(statusCode)) {
//         return (ws_http::STATUS_200_OK);
//     } else if (fileStat == WS_DIR) {
//         return (this->checkDirectory(this->_request.pathDecoded, internal));
//     }
//     return (this->setError(ws_http::STATUS_404_NOT_FOUND, internal));
// }
