/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/20 00:18:51 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpHandler::HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server )
 : APollEventHandler(polling, true), ipPortData(ipPort), _childProcessHandler(NULL), _server(server), _httpMessage(&server)
{ }

HttpHandler::~HttpHandler( void )
{ 
    quitCgiProcess();
}

void    HttpHandler::quitCgiProcess( void )
{
    if (_childProcessHandler) {
        std::cout << "\n -------- quitCgiProcess -------- " << std::endl;
        _polling.stopMonitoringFd(_childProcessHandler);
        _childProcessHandler = NULL;
    }
}

void    HttpHandler::quit( void )
{
    // if (_childProcessHandler) {
    //     _polling.stopMonitoringFd(_childProcessHandler);
    //     _childProcessHandler = NULL;
    // }
    quitCgiProcess();
    std::cout << "\n -------- quitClientConnection -------- " << std::endl;
    return (_polling.stopMonitoringFd(this));
}

void    HttpHandler::handleEvent( struct pollfd & pollfd )
{
    if (pollfd.revents & POLLERR)
        std::cout << "POLLERR CLIENT" << std::endl;

    if (pollfd.revents & POLLHUP) {
        std::cout << "\n -------- handleEvent - quit - POLLHUP " << std::endl;
        return (quit());
    }
    if (pollfd.revents & POLLIN && !_httpMessage.responseSet() && !_httpMessage.isCgi()) {
        int readBytes = _httpMessage.readFromSocketAndParseHttp(pollfd.fd, 0);
        if (readBytes == -1 || readBytes == 0) {
            std::cout << "\n -------- handleEvent - quit - readBytes: " << readBytes << std::endl;
            return (quit());
        }
        if (_httpMessage.getStatus() != ws_http::STATUS_UNDEFINED)
            processResponse(_httpMessage.getStatus());
    }
    if (pollfd.revents & POLLOUT && _httpMessage.responseSet() && !_httpMessage.isCgi()) {
        int sendBytes = _httpMessage.sendDataToSocket(pollfd.fd, 0);
        if (sendBytes == -1) {
            std::cout << "\n -------- handleEvent - quit - sendBytes: " << sendBytes << std::endl;
            return (quit());
        } else if (sendBytes == 0) {
            std::cout << "\n ------- RESPONSE SENT, RESET HTTP MESSAGE ------- " << std::endl;
            _httpMessage = HttpMessage(&_server);
        }
    }
}

void    HttpHandler::handleChildEvent( struct pollfd & pollfd )
{
    if (pollfd.revents & POLLERR)
        std::cout << "POLLERR CHILD" << std::endl;


    if (pollfd.revents & POLLHUP) {
        _httpMessage.setCgiResponseDone();
        quitCgiProcess();
        return ;
    }
    if (pollfd.revents & POLLIN && !_httpMessage.responseSet()) {
       int readBytes = _httpMessage.readFromSocketAndParseHttp(pollfd.fd, 0);
        if (readBytes == -1) {
            std::cout << "\n -------- handleChildEvent - quit - readBytes: " << readBytes << std::endl;
            return (quit());
        }
    }
    if (pollfd.revents & POLLOUT && _httpMessage.responseSet()) {
        int sendBytes = _httpMessage.sendDataToSocket(pollfd.fd, 0);
        if (sendBytes == -1) {
            std::cout << "\n -------- handleChildEvent - quit - sendBytes: " << sendBytes << std::endl;
            return (quit());
        }
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

    _childProcessHandler = new ChildProcessHandler(_polling, *this);

    _childProcessHandler->addArgument(config.getFilePath());
    _childProcessHandler->addEnvVariable(std::string("QUERY_STRING=") + _httpMessage.header.getHeader("@query"));
    _childProcessHandler->addEnvVariable(std::string("REQUEST_METHOD=") + _httpMessage.header.getHeader("@method"));
    _childProcessHandler->addEnvVariable(std::string("CONTENT_TYPE=") + _httpMessage.header.getHeader("content-length"));
    _childProcessHandler->addEnvVariable(std::string("CONTENT_LENGTH=") + toStr(_httpMessage.getBody().size()));
    _childProcessHandler->addEnvVariable(std::string("SCRIPT_FILENAME=") + config.getFilePath());
    _childProcessHandler->addEnvVariable(std::string("SCRIPT_NAME=") + _httpMessage.header.getHeader("@pathdecoded"));
    _childProcessHandler->addEnvVariable(std::string("PATH_INFO="));
    _childProcessHandler->addEnvVariable(std::string("PATH_TRANSLATED=") + config.getRootPath());
    _childProcessHandler->addEnvVariable(std::string("REQUEST_URI=") + _httpMessage.header.getHeader("@fulluri"));
    _childProcessHandler->addEnvVariable(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    _childProcessHandler->addEnvVariable(std::string("GATEWAY_INTERFACE=CGI/1.1"));
    _childProcessHandler->addEnvVariable(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
    _childProcessHandler->addEnvVariable(std::string("REMOTE_ADDR=") + ipPortData.getIpStr());
    _childProcessHandler->addEnvVariable(std::string("REMOTE_PORT=") + ipPortData.getPortStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_ADDR=") + _server.serverIpPort.getIpStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_PORT=") + _server.serverIpPort.getPortStr());
    _childProcessHandler->addEnvVariable(std::string("SERVER_NAME=") + config.getServerName());
    _childProcessHandler->addEnvVariable(std::string("REDIRECT_STATUS=200"));


    HttpHeader::headers_t const & head = _httpMessage.header.getHeaderVector();
    for (std::size_t i = 0; i != head.size(); ++i) {
        if (head[i].first[0] == '@')
            continue;
        std::string cgiHeader = "HTTP_";
        std::transform(head[i].first.begin(), head[i].first.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
        _childProcessHandler->addEnvVariable(cgiHeader + "=" + head[i].second);
    }

    int cgiSocketFd = _childProcessHandler->createChildProcess(config.getCgiExecutable());
    if (cgiSocketFd == -1) {
        // delete _childProcessHandler;
        // _childProcessHandler = NULL;
        std::cout << "CHECKING IF CHILD PROCESS IS DONE\n";
        quitCgiProcess();
        return (processError(config, ws_http::STATUS_500_INTERNAL_SERVER_ERROR));
    } else {
        _httpMessage.prepareCgi();
        _polling.startMonitoringFd(cgiSocketFd, POLLIN | POLLOUT, _childProcessHandler, true);
    }
}


void  HttpHandler::processResponse( ws_http::statuscodes_t currentStatus )
{

    if (_httpMessage.header.getHeader("@pathdecoded") == "/empty")
        return (_httpMessage.setResponse(ws_http::STATUS_200_OK, NULL, "", ""));

    HttpConfig config = _server.getHttpConfig(_httpMessage.header.getHeader("@pathdecoded"), _httpMessage.header.getHeader("host"));

    // check version
    if (ws_http::versions_rev.at(_httpMessage.header.getHeader("@version")) != ws_http::VERSION_1_1)
        return (processError(config, ws_http::STATUS_505_HTTP_VERSION_NOT_SUPPORTED));

    // check redirection
    Redirect redirect = config.getRedirection();
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED)
        return (_httpMessage.setResponse(redirect.redirectStatus, NULL, "", redirect.location));

    // check method allowed
    if (!config.checkAllowedMethod(_httpMessage.header.getHeader("@method")))
        return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));

    // check cgi
        std::string const & cgiExecutablePath = config.getCgiExecutable();
        if (!cgiExecutablePath.empty())
            return (handleCgi(config));

    std::string const & method = _httpMessage.header.getHeader("@method");
    if (method == "POST")
        return (processPost(config));
    else if (method == "DELETE")
        return (processDelete(config));
    else if (method == "GET")
        return (processGetHead(config, currentStatus));
    return (processError(config, ws_http::STATUS_501_NOT_IMPLEMENTED));
}

void  HttpHandler::processError( HttpConfig const & config, ws_http::statuscodes_t errorCode )
{
    std::string const & errorPageUri = config.getErrorPage(errorCode);
    if (errorPageUri.empty())
        return (_httpMessage.setResponse(errorCode, NULL, "", ""));
    if (!_httpMessage.header.reparseRequestLine(ws_http::METHOD_GET, errorPageUri))
        return(_httpMessage.setResponse(errorCode, NULL, "", ""));
    processResponse(errorCode);
}


void  HttpHandler::processGetHead( HttpConfig const & config, ws_http::statuscodes_t statusCode )
{
    std::cout << "\n------ processGetHead -------\n";
    // check if accessible and exists
    FileInfos fileInfo(config.getFilePath(), R_OK, true);
    std::cout << "filePath: " << config.getFilePath() << std::endl;
    // if (fileInfo.getStatus() != ws_http::STATUS_200_OK && statusCode != ws_http::STATUS_200_OK)
    //     return (_httpMessage.setResponse(statusCode, NULL, "", ""));
    // if (fileInfo.getStatus() != ws_http::STATUS_200_OK)
    //     return (processError(config, fileInfo.getStatus()));
    if (fileInfo.getStatus() != ws_http::STATUS_200_OK) {
        if (statusCode != ws_http::STATUS_200_OK)
            return (_httpMessage.setResponse(fileInfo.getStatus(), NULL, "", ""));
        return (processError(config, fileInfo.getStatus()));
    }

    if (statusCode == ws_http::STATUS_1001_INTERNAL_REDIRECT)
        statusCode = ws_http::STATUS_200_OK;

    // check if file
    if (!fileInfo.isDirectory()) {
        std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary);
        if (!ifs.is_open())
            return (processError(config, ws_http::STATUS_404_NOT_FOUND));
        _httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
        return (ifs.close());
    }
     // if dir
    std::string const & indexFileName = fileInfo.checkContainedFile(config.getIndexFile());
    if (!indexFileName.empty()) {
        if (!_httpMessage.header.reparseRequestLine(ws_http::METHOD_GET, _httpMessage.header.getHeader("@pathdecoded") + "/" + indexFileName))
            return(_httpMessage.setResponse(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, NULL, "", ""));
        return (processResponse(ws_http::STATUS_1001_INTERNAL_REDIRECT));
    }
    if (!config.hasDirectoryListing())
        return (processError(config, ws_http::STATUS_403_FORBIDDEN));
    std::stringstream ss;
    fileInfo.setDirListing(ss, _httpMessage.header.getHeader("@pathdecoded"));
    return (_httpMessage.setResponse(statusCode, &ss, "text/html", ""));
}

bool    createFile(std::string const & filePath, const char *start, const char *end)
{
    std::cout << "UPLOAD HERE: " << filePath << " | size: " << end - start << std::endl;
    // std::cout << "CONTENT: $" << std::string(start, end - start) << "$" << std::endl;
    std::ofstream ofs(filePath.c_str(), std::ofstream::binary);
    if (!ofs.is_open()) {
        // std::cout << "ERROR CREATING FILE" << std::endl;
        return (false);
    }
    ofs.write(start, end - start);
    ofs.close();
    return (true);
}

int handleMultipart(std::string const & rootPath, std::string & filename, const char *start, const char *end)
{
    HttpHeader fileHeader;
    int dist = fileHeader.parseHeader(start, end, false);
    std::cout << fileHeader.getHeader("content-disposition") << std::endl;
    std::cout << fileHeader.getHeader("content-type") << std::endl;
    std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
    std::size_t fnamepos = contentDisposition.find("filename="); //error?
    std::cout << "filename: " << filename << std::endl;
    filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
    std::string filePath = rootPath + "/" + filename;
    
    // std::cout << "filename: " << filename << std::endl;
    // std::cout << "FILEDATA: " << std::string(&(*itStart) + dist, &(*itBound)) << std::endl;
    return (createFile(filePath, start + dist, end));
}


void  HttpHandler::processPost( HttpConfig const & config )
{
    (void)config;
    // check if accessible

    // check if exists

    // check if directory

    // parse multipart

    // check if file exists
    
    // necessary to check: check if file already exists

    // _httpMessage.printMessage();
    // std::cout << "-------- body: \n" << _httpMessage.getBody() << std::endl;
    // exit(1);

    std::string const & body = _httpMessage.getBody();
    FileInfos uploadDir(config.getFilePath(), R_OK | W_OK, true);
    if (uploadDir.getStatus() != ws_http::STATUS_200_OK)
        return (processError(config, uploadDir.getStatus()));
    if (!uploadDir.isDirectory())
        return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));

    std::string contentType = _httpMessage.header.getHeader("content-type");
    std::size_t sepPos1 = contentType.find("multipart/form-data");
    std::size_t sepPos2 = contentType.find("boundary=");
    if (sepPos1 != 0 || sepPos2 == std::string::npos)
        return (processError(config, ws_http::STATUS_415_UNSUPPORTED_MEDIA_TYPE));
    std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
    std::string boundaryStart = "--";
    std::string filename, relFilePath;
    buff_t::const_iterator itStartFile = body.begin() + 2 + boundary.size() + ws_http::crlf.size();
    buff_t::const_iterator itStart = itStartFile;
    int fileNbr = 0;
    for (buff_t::const_iterator itBoundStart = itStartFile; itBoundStart + boundaryStart.size() < body.end(); itStart = itBoundStart + boundaryStart.size()) {
        itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());
        if (std::string(itBoundStart + boundaryStart.size(), itBoundStart + boundaryStart.size() + boundary.size()) == boundary) {
            handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
            itStart = itStartFile = itBoundStart + boundaryStart.size() + boundary.size() + ws_http::crlf.size();
            fileNbr++;
        }
    }
    FileInfos uploadedDir(config.getFilePath(), R_OK | W_OK, true);
    std::stringstream ss;
    uploadedDir.setDirListing(ss, _httpMessage.header.getHeader("@pathdecoded"));
    return (_httpMessage.setResponse(ws_http::STATUS_201_CREATED, &ss, "text/html", ""));
}

void  HttpHandler::processDelete( HttpConfig const & config )
{
    (void)config;
    // check if accessible

    // check if exists

    // check if file

    // -> DELETE FILE

    // std::remove()

    FileInfos deleteDir(config.getFilePath(), 0, false);
    if (deleteDir.getStatus() != ws_http::STATUS_200_OK)
        return (processError(config, deleteDir.getStatus()));
    if (deleteDir.isDirectory())
        return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
    std::string filePath = config.getFilePath().substr(0, config.getFilePath().find_last_of('/') + 1);
    FileInfos   directoryPath(filePath, R_OK | W_OK | X_OK, false);
    if (directoryPath.getStatus() != ws_http::STATUS_200_OK)
        return (processError(config, directoryPath.getStatus()));
    std::remove(config.getFilePath().c_str());
    _httpMessage.setResponse(ws_http::STATUS_204_NO_CONTENT, NULL, "", "");

}



// bool    createFile(std::string const & filePath, const char *start, const char *end)
// {
//     std::cout << "UPLOAD HERE: " << filePath << " | size: " << end - start << std::endl;
//     // std::cout << "CONTENT: $" << std::string(start, end - start) << "$" << std::endl;
//     std::ofstream ofs(filePath.c_str(), std::ofstream::binary);
//     if (!ofs.is_open()) {
//         // std::cout << "ERROR CREATING FILE" << std::endl;
//         return (false);
//     }
//     ofs.write(start, end - start);
//     ofs.close();
//     return (true);
// }

// int handleMultipart(std::string const & rootPath, std::string & filename, const char *start, const char *end)
// {
//     HttpHeader fileHeader;
//     int dist = fileHeader.parseHeader(start, end, false);
//     std::cout << fileHeader.getHeader("content-disposition") << std::endl;
//     std::cout << fileHeader.getHeader("content-type") << std::endl;
//     std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
//     std::size_t fnamepos = contentDisposition.find("filename="); //error?
//     std::cout << "filename: " << filename << std::endl;
//     filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
//     std::string filePath = rootPath + "/" + filename;
    
//     // std::cout << "filename: " << filename << std::endl;
//     // std::cout << "FILEDATA: " << std::string(&(*itStart) + dist, &(*itBound)) << std::endl;
//     return (createFile(filePath, start + dist, end));
// }



// ws_http::statuscodes_t  HttpHandler::processResponsePost(HttpConfig const & config)
// {
//     std::string const & body = _httpMessage.getBody();
//     FileInfos uploadDir(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
//     if (uploadDir.getError() == 0 && !S_ISDIR(uploadDir.getMode()))
//         return (processResponseFile(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), 0));
//     if (uploadDir.getError() == EACCES)
//         return (processResponseFile(config, ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), 0));
//     if (uploadDir.getError() != 0)
//         return (processResponseFile(config, ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404"), 0));

//     std::string contentType = _httpMessage.header.getHeader("content-type");
//     std::size_t sepPos1 = contentType.find("multipart/form-data");
//     std::size_t sepPos2 = contentType.find("boundary=");
//     if (sepPos1 == 0 && sepPos2 != std::string::npos) {
//         std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
//         std::string boundaryStart = "--";
//         std::string filename, relFilePath;
//         buff_t::const_iterator itStartFile = body.begin() + 2 + boundary.size() + ws_http::crlf.size();
//         buff_t::const_iterator itStart = itStartFile;
//         int fileNbr = 0;
//         for (buff_t::const_iterator itBoundStart = itStartFile; itBoundStart + 2 < body.end(); itStart = itBoundStart + 2) {
//             itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());
//             if (std::string(itBoundStart + 2, itBoundStart + 2 + boundary.size()) == boundary) {
//                 handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
//                 itStart = itStartFile = itBoundStart + 2 + boundary.size() + ws_http::crlf.size();
//                 fileNbr++;
//             }
//         }
//         if (fileNbr == 1) {
//             processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path") + "/" + filename, 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "", _httpMessage.header.getHeader("@path") + "/" + filename);
//         } else {
//             processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path"), 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "",  _httpMessage.header.getHeader("@path"));
//         }
//         return (ws_http::STATUS_201_CREATED);
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t  HttpHandler::processResponseDelete( HttpConfig const & config )
// {
//     (void)config;
//     return (ws_http::STATUS_UNDEFINED);
// }


// // ws_http::statuscodes_t  HttpHandler::processResponseGetHead( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & requestPathDecoded, int redirNo )
// // {
// //     FileInfos file(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
// //     if (file.getError() == 0 && S_ISREG(file.getMode())) {
// //         std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary);
// //         if (ifs.is_open()) {
// //             _httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
// //             ifs.close();
// //             return (statusCode);
// //         }
// //     } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
// //         std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
// //         if (!indexFileName.empty())
// //             return (processResponseGetHead(config, statusCode, requestPathDecoded + "/" + indexFileName, true));
// //         if (!config.hasDirectoryListing())
// //             return (processResponseGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
// //         std::stringstream ss;
// //         file.setDirListing(ss);
// //         _httpMessage.setResponse(statusCode, &ss, "text/html", "");
// //         return (statusCode);
// //     } else if (file.getError() == EACCES) {
// //         return (processResponseGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
// //     }
// //     return (processResponseGetHead(config, ws_http::STATUS_404_NOT_FOUND, config.getErrorPage("404"), ++redirNo));
// // }

// ws_http::statuscodes_t  HttpHandler::processError( ws_http::statuscodes_t errorCode )
// {
    
// }

// ws_http::statuscodes_t  HttpHandler::processGetHead( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & requestPathDecoded, int redirNo )
// {
//     FileInfos file(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
//     if (file.getError() == 0 && S_ISREG(file.getMode())) {
//         std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary);
//         if (ifs.is_open()) {
//             _httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
//             ifs.close();
//             return (statusCode);
//         }
//     } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
//         std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
//         if (!indexFileName.empty())
//             return (processGetHead(config, statusCode, requestPathDecoded + "/" + indexFileName, true));
//         if (!config.hasDirectoryListing())
//             return (processError(ws_http::STATUS_403_FORBIDDEN));
//             // return (processGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
//         std::stringstream ss;
//         file.setDirListing(ss);
//         _httpMessage.setResponse(statusCode, &ss, "text/html", "");
//         return (statusCode);
//     } else if (file.getError() == EACCES) {
//         return (processError(ws_http::STATUS_403_FORBIDDEN));
//         // return (processGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
//     }
//     return (processError(ws_http::STATUS_404_NOT_FOUND));
//     // return (processGetHead(config, ws_http::STATUS_404_NOT_FOUND, config.getErrorPage("404"), ++redirNo));
// }

// ws_http::statuscodes_t    HttpHandler::process( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
// {
//     if ((statusCode >= ws_http::STATUS_400_BAD_REQUEST && redirNo > 1) || redirNo >= 10) {
//         if (redirNo >= 10)
//             statusCode = ws_http::STATUS_500_INTERNAL_SERVER_ERROR;
//         _httpMessage.setResponse(statusCode, NULL, "text/html", "");
//         return (statusCode);
//     }

//     HttpConfig config = _server.getHttpConfig(requestPathDecoded, _httpMessage.header.getHeader("host"));
//     // std::cout << "filepath: " << config.getFilePath() << std::endl;
//     Redirect redirect = config.getRedirection();
//     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
//         _httpMessage.setResponse(redirect.redirectStatus, NULL, "", redirect.location);
//         return (redirect.redirectStatus);
//     }

//     if (ws_http::versions_rev.at(_httpMessage.header.getHeader("@version")) != ws_http::VERSION_1_1)
//         return (processResponse(ws_http::STATUS_501_NOT_IMPLEMENTED, "GET", config.getErrorPage("501"), ++redirNo));
//     if (!config.checkAllowedMethod(method))
//         return (processResponse(ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), ++redirNo));
//     std::string const & cgiExecutablePath = config.getCgiExecutable();
//     if (!cgiExecutablePath.empty()) {
//         handleCgi(config);
//         return (ws_http::STATUS_1001_INTERNAL_PROCESS_CGI);
//     }

//     std::string const & meth = _httpMessage.header.getHeader("@method");
//     if (_httpMessage.header.getHeader("@method") == "POST")
//         return (processResponsePost(config));
//     else if (_httpMessage.header.getHeader("@method") == "DELETE")
//         return (processResponseDelete(config));
//     else if (_httpMessage.header.getHeader("@method") == "GET")
//         return (processResponseGetHead(config, statusCode, requestPathDecoded, redirNo));
    
// }

// ws_http::statuscodes_t  HttpHandler::processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, 
//                                                             std::string const & method, std::string const & requestPathDecoded, int redirNo )
// {
//     FileInfos file(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
    // if (file.getError() == 0 && S_ISREG(file.getMode())) {
//         std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary | std::ios::ate);
//         if (ifs.is_open()) {
//             _httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
//             ifs.close();
//             return (statusCode);
//         }
//     } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
//         std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
//         if (!indexFileName.empty())
//             return (processResponse(statusCode, method, requestPathDecoded + "/" + indexFileName, true));
//         if (!config.hasDirectoryListing())
//             return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
//         std::stringstream ss;
//         file.setDirListing(ss);
//         _httpMessage.setResponse(statusCode, &ss, "text/html", "");
//         return (statusCode);
//     } else if (file.getError() == EACCES) {
//         return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
//     }
//     return (processResponse(ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404"), ++redirNo));
// }

// ws_http::statuscodes_t    HttpHandler::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
// {
//     if ((statusCode >= ws_http::STATUS_400_BAD_REQUEST && redirNo > 1) || redirNo >= 10) {
//         if (redirNo >= 10)
//             statusCode = ws_http::STATUS_500_INTERNAL_SERVER_ERROR;
//         _httpMessage.setResponse(statusCode, NULL, "text/html", "");
//         return (statusCode);
//     }

//     HttpConfig config = _server.getHttpConfig(requestPathDecoded, _httpMessage.header.getHeader("host"));
//     // std::cout << "filepath: " << config.getFilePath() << std::endl;
//     Redirect redirect = config.getRedirection();
//     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
//         _httpMessage.setResponse(redirect.redirectStatus, NULL, "", redirect.location);
//         return (redirect.redirectStatus);
//     }

//     if (ws_http::versions_rev.at(_httpMessage.header.getHeader("@version")) != ws_http::VERSION_1_1)
//         return (processResponse(ws_http::STATUS_501_NOT_IMPLEMENTED, "GET", config.getErrorPage("501"), ++redirNo));
//     if (!config.checkAllowedMethod(method))
//         return (processResponse(ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), ++redirNo));
//     std::string const & cgiExecutablePath = config.getCgiExecutable();
//     if (!cgiExecutablePath.empty()) {
//         handleCgi(config);
//         return (ws_http::STATUS_1001_INTERNAL_PROCESS_CGI);
//     }

//     if (_httpMessage.header.getHeader("@method") == "POST" || _httpMessage.header.getHeader("@method") == "PUT")
//         return (processResponsePost(config, statusCode));

//     return (processResponseFile(config, statusCode, method, requestPathDecoded, redirNo));
    
// }
