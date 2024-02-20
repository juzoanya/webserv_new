/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 22:17:41 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpContent::HttpContent( void )
 : headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
{ }

HttpContent::~HttpContent( void )
{ }

int HttpContent::sendDataToSocket( int socketfd, int flags )
{
    std::cout << "send data to socket" << std::endl;
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

void    HttpContent::setContentExternalBuffer( buff_t const & buffer )
{
    this->bodyPtr = buffer.data();
    this->bodySize = buffer.size();
}


ws_http::statuscodes_t HttpContent::setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType )
{
    std::cout << "-- setHeader\n";
    this->_bodyBuffer.reserve(4096);
    std::stringstream ss;
    ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
    ss << "connection: close" << ws_http::crlf;
    std::size_t httpBodyStreamSize = getStreamSize(httpBodyStream);
    ss << "content-type: " << mimeType << ws_http::crlf;
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
    return (status);
}

ws_http::statuscodes_t  HttpContent::setResponseDefaultError( ws_http::statuscodes_t errorStatusCode )
{
    std::stringstream ss;
    ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
        << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" "</head>" 
        << "<body>" "<div style=\"text-align: center;\">" 
        << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>"
        << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
        << "</body>" "</html>";
    return(this->setResponseStatic(errorStatusCode, ss, "text/html"));
}

ws_http::statuscodes_t  HttpContent::setResponseRedirection( ws_http::statuscodes_t status, std::string const & location )
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
    return (status);
}

ws_http::statuscodes_t     HttpContent::appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
{
    std::copy(itStart, itEnd, std::back_inserter(this->_headerBuffer));
    if (flush) {
        this->headerPtr = this->_headerBuffer.data();
        this->headerSize = this->_headerBuffer.size();
    }
    return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t     HttpContent::appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
{
    std::copy(itStart, itEnd, std::back_inserter(this->_bodyBuffer));
    if (flush) {
        this->bodyPtr = this->_bodyBuffer.data();
        this->bodySize = this->_bodyBuffer.size();
    }
    return (ws_http::STATUS_UNDEFINED);
}



HttpHandler::HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server )
 : APollEventHandler(polling, true), ipPortData(ipPort), _childProcessHandler(NULL), _server(server), _request(&server, true, NULL)
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
//     this->httpContent = HttpContent(&this->_headerBuffer, &this->_fileData);
//     return (status);
// }

void    HttpHandler::handleEvent( struct pollfd & pollfd )
{
    if (pollfd.revents & POLLHUP) {
        std::cout << "HttpHandler POLLHUP\n";
        return (_polling.stopMonitoringFd(this));
    }
    if (pollfd.revents & POLLIN) {
        std::cout << "HttpHandler POLLIN\n";
        int readByte = recv(pollfd.fd, _polling.readBuffer.data(), _polling.readBuffer.size(), 0);
        std::cout << "HttpHandler readByte: " << readByte << std::endl;
        std::cout << "HttpHandler read: \n" << std::string(_polling.readBuffer.begin(), _polling.readBuffer.begin() + readByte) << std::endl;
        if (readByte == -1) {
            return (_polling.stopMonitoringFd(this));
        } else if (readByte == 0) {

        } else {
            ws_http::statuscodes_t status = _request.parseRequest(_polling.readBuffer.begin(), _polling.readBuffer.begin() + readByte);
            std::cout << "HttpHandler::handleData, statusCode parsing the Request: " << ws_http::statuscodes.at(status) << std::endl;
            _request.printRequest();
            if (status == ws_http::STATUS_200_OK)
                processResponse(ws_http::STATUS_200_OK, _request.getHeader(HttpRequest::method), _request.getHeader(HttpRequest::pathDecoded), false);
            else if (status != ws_http::STATUS_UNDEFINED)
                _content.setResponseDefaultError(_request.getStatus());
            if (status != ws_http::STATUS_UNDEFINED)
                _request = HttpRequest(&_server, true, NULL);
        }
    }
    if (pollfd.revents & POLLOUT && !_content.empty()) {
        std::cout << "HttpHandler POLLOUT\n";
        int sendBytes = _content.sendDataToSocket(pollfd.fd, 0);
        std::cout << "HttpHandler sendBytes: " << sendBytes << std::endl;
        if (sendBytes == -1 || sendBytes == 0) {
            if (_childProcessHandler) {
                _polling.stopMonitoringFd(_childProcessHandler);
            }
            return (_polling.stopMonitoringFd(this));
        }
    }
}

void    HttpHandler::closeChildProcess( int bytesSend )
{
    if (!_childProcessHandler)
        return ;
    if (bytesSend == 0) {
        _content.setResponseDefaultError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR);
    } else if (bytesSend > 0) {
        delete _childProcessHandler;
        _childProcessHandler = NULL;
        return (_polling.stopMonitoringFd(this));
    }
}

void    HttpHandler::readFromChildProcess( int socketfd )
{
    int readByte = recv(socketfd, _polling.readBuffer.data(), _polling.readBuffer.size(), 0);
    if (readByte == -1) {
        return (_polling.stopMonitoringFd(this));
    } else if (readByte == 0) {

    } else {
        ws_http::statuscodes_t status = _request.parseRequest(_polling.readBuffer.begin(), _polling.readBuffer.begin() + readByte);
        _request.printRequest();
        if (status == ws_http::STATUS_200_OK)
            processResponse(ws_http::STATUS_200_OK, _request.getHeader(HttpRequest::method), _request.getHeader(HttpRequest::pathDecoded), false);
        else if (status != ws_http::STATUS_UNDEFINED)
            _content.setResponseDefaultError(_request.getStatus());
        if (status != ws_http::STATUS_UNDEFINED)
            _request = HttpRequest(&_server, true, NULL);
    }
}

int charToupperUnderscore( int c )
{
    if (c == '-')
        return ('_');
    return (::toupper(c));
}

void    HttpHandler::handleCgi( HttpConfig const & config )
{
    _childProcessHandler = new ChildProcessHandler(_polling, _request.getBody().data(), _request.getBody().size());

    // _request = HttpRequest(NULL, false, )

    _childProcessHandler->addArgument(config.getFilePath());
    _childProcessHandler->addEnvVariable(std::string("QUERY_STRING=") + _request.getHeader(HttpRequest::query));
    _childProcessHandler->addEnvVariable(std::string("REQUEST_METHOD=") + _request.getHeader(HttpRequest::method));
    _childProcessHandler->addEnvVariable(std::string("CONTENT_TYPE=") + _request.getHeader("content-length"));
    _childProcessHandler->addEnvVariable(std::string("CONTENT_LENGTH=") + toStr(_request.getBody().size()));
    _childProcessHandler->addEnvVariable(std::string("SCRIPT_FILE_NAME=") + config.getFilePath());
    _childProcessHandler->addEnvVariable(std::string("SCRIPT_NAME=") + _request.getHeader(HttpRequest::pathDecoded));
    _childProcessHandler->addEnvVariable(std::string("PATH_INFO="));
    _childProcessHandler->addEnvVariable(std::string("PATH_TRANSLATED=") + config.getRootPath());
    _childProcessHandler->addEnvVariable(std::string("REQUEST_URI=") + _request.getHeader(HttpRequest::fullUri));
    _childProcessHandler->addEnvVariable(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    _childProcessHandler->addEnvVariable(std::string("GATEWAY_INTERFACE=CGI/1.1"));
    _childProcessHandler->addEnvVariable(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
    _childProcessHandler->addEnvVariable(std::string("REMOTE_ADDR=") + ipPortData.getIpStr());
    _childProcessHandler->addEnvVariable(std::string("REMOTE_PORT=") + ipPortData.getPortStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_ADDR=") + _server.serverIpPort.getIpStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_PORT=") + _server.serverIpPort.getPortStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_NAME=") + config.getServerName());
    _childProcessHandler->addEnvVariable(std::string("REDIRECT_STATUS=200"));

    HttpRequest::headers_t  const & headers = _request.getHeaderVector();
    for (HttpRequest::headers_t::const_iterator it; it != headers.end(); ++it) {
        std::string cgiHeader = "HTTP_";
        std::transform(it->first.begin(), it->first.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
        _childProcessHandler->addEnvVariable(cgiHeader + "=" + it->second);
    }
    int cgiSocketFd = _childProcessHandler->createChildProcess(config.getCgiExecutable());
    if (cgiSocketFd == -1) {
        delete _childProcessHandler;
        _childProcessHandler = NULL;
    } else {
        _polling.startMonitoringFd(cgiSocketFd, POLLIN | POLLOUT, _childProcessHandler);
    }
}

ws_http::statuscodes_t  HttpHandler::processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, 
                                                            std::string const & method, std::string const & requestPathDecoded, int redirNo )
{
    std::cout << "FILEPLATH: " << config.getFilePath() << std::endl;
    
    FileInfos file(config.getFilePath(), _request.getHeader(HttpRequest::pathDecoded));
    if (file.getError() == 0 && S_ISREG(file.getMode())) {
        std::cout << "----> is a regular file" << std::endl;
        std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary | std::ios::ate);
        if (ifs.is_open()) {
            _content.setResponseStatic(statusCode, ifs, config.getMimeType());
            ifs.close();
            return (statusCode);
        }
    } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
        std::cout << "----> is a directory" << std::endl;
        std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
        std::cout << "----> indexFileName:" << indexFileName << std::endl;
        if (!indexFileName.empty())
            return (processResponse(statusCode, method, requestPathDecoded + "/" + indexFileName, true));
        if (!config.hasDirectoryListing())
            return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
        std::stringstream ss;
        file.setDirListing(ss);
        return (_content.setResponseStatic(statusCode, ss, "text/html"));
    } else if (file.getError() == EACCES) {
        std::cout << "----> permission denied" << std::endl;
        return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
    }
    std::cout << "----> not found" << std::endl;
    return (processResponse(ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404"), ++redirNo));
}

ws_http::statuscodes_t    HttpHandler::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
{
    std::cout << "\n---------------- HttpHandler::processResponse ----------------\n"
        << "status: " << ws_http::statuscodes.at(statusCode)
        << " | method: " << method << " | requestPathDecoded: "
        << requestPathDecoded << " | redirNo: "
        << redirNo
        << std::endl;

    if (statusCode >= ws_http::STATUS_400_BAD_REQUEST && redirNo > 0)
        return (_content.setResponseDefaultError(statusCode));
    if (redirNo >= 10)
        return (_content.setResponseDefaultError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR));

    HttpConfig config = _server.getHttpConfig(requestPathDecoded, _request.getHeader("host"));
    std::cout << "rootPath: " << config.getRootPath() << std::endl;
    std::cout << "filePath: " << config.getFilePath() << std::endl;
    Redirect redirect = config.getRedirection();
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
        return (_content.setResponseRedirection(redirect.redirectStatus, redirect.location));
    if (!config.checkAllowedMethod(method))
        return ( processResponse(ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), ++redirNo));
    std::string const & cgiExecutablePath = config.getCgiExecutable();
    std::cout << "cgi executable: " << cgiExecutablePath << std::endl;
    if (!cgiExecutablePath.empty()) {
        handleCgi(config);
        return (ws_http::STATUS_1001_INTERNAL_PROCESS_CGI);
    }

    return (processResponseFile(config, statusCode, method, requestPathDecoded, redirNo));
    
}
