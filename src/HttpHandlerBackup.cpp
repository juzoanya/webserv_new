
#include "headers.hpp"



HttpHandler::HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server )
 : APollEventHandler(polling, true), ipPortData(ipPort), _childProcessHandler(NULL), _server(server), _httpMessage(&server)
{ }

HttpHandler::~HttpHandler( void )
{ }

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
        if (_httpMessage.getStatus() == ws_http::STATUS_200_OK)
            processResponse(ws_http::STATUS_200_OK, _httpMessage.header.getHeader("@method"), _httpMessage.header.getHeader("@pathdecoded"), false);
        else if (_httpMessage.getStatus() != ws_http::STATUS_UNDEFINED)
            _httpMessage.setResponse(_httpMessage.getStatus(), NULL, "text/html", "");
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

    _httpMessage.prepareCgi();
    int cgiSocketFd = _childProcessHandler->createChildProcess(config.getCgiExecutable());
    if (cgiSocketFd == -1) {
        // delete _childProcessHandler;
        // _childProcessHandler = NULL;
        quitCgiProcess();
    } else {
        _polling.startMonitoringFd(cgiSocketFd, POLLIN | POLLOUT, _childProcessHandler, true);
    }
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


ws_http::statuscodes_t  HttpHandler::processResponsePost(HttpConfig const & config)
{
    std::string const & body = _httpMessage.getBody();
    FileInfos uploadDir(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
    if (uploadDir.getError() == 0 && !S_ISDIR(uploadDir.getMode()))
        return (processResponseFile(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), 0));
    if (uploadDir.getError() == EACCES)
        return (processResponseFile(config, ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), 0));
    if (uploadDir.getError() != 0)
        return (processResponseFile(config, ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404"), 0));

    std::string contentType = _httpMessage.header.getHeader("content-type");
    std::size_t sepPos1 = contentType.find("multipart/form-data");
    std::size_t sepPos2 = contentType.find("boundary=");
    if (sepPos1 == 0 && sepPos2 != std::string::npos) {
        std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
        std::string boundaryStart = "--";
        std::string filename, relFilePath;
        buff_t::const_iterator itStartFile = body.begin() + 2 + boundary.size() + ws_http::crlf.size();
        buff_t::const_iterator itStart = itStartFile;
        int fileNbr = 0;
        for (buff_t::const_iterator itBoundStart = itStartFile; itBoundStart + 2 < body.end(); itStart = itBoundStart + 2) {
            itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());
            if (std::string(itBoundStart + 2, itBoundStart + 2 + boundary.size()) == boundary) {
                handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
                itStart = itStartFile = itBoundStart + 2 + boundary.size() + ws_http::crlf.size();
                fileNbr++;
            }
        }
        if (fileNbr == 1) {
            processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path") + "/" + filename, 0);
            // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "", _httpMessage.header.getHeader("@path") + "/" + filename);
        } else {
            processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path"), 0);
            // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "",  _httpMessage.header.getHeader("@path"));
        }
        return (ws_http::STATUS_201_CREATED);
    }
    return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t  HttpHandler::processResponseDelete( HttpConfig const & config )
{
    (void)config;
    return (ws_http::STATUS_UNDEFINED);
}


ws_http::statuscodes_t  HttpHandler::processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
{
    FileInfos file(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
    if (file.getError() == 0 && S_ISREG(file.getMode())) {
        std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary | std::ios::ate);
        if (ifs.is_open()) {
            _httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
            ifs.close();
            return (statusCode);
        }
    } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
        std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
        if (!indexFileName.empty())
            return (processResponseGetHead(config, statusCode, requestPathDecoded + "/" + indexFileName, true));
        if (!config.hasDirectoryListing())
            return (processResponseGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
        std::stringstream ss;
        file.setDirListing(ss);
        _httpMessage.setResponse(statusCode, &ss, "text/html", "");
        return (statusCode);
    } else if (file.getError() == EACCES) {
        return (processResponseGetHead(config, ws_http::STATUS_403_FORBIDDEN, config.getErrorPage("403"), ++redirNo));
    }
    return (processResponseGetHead(config, ws_http::STATUS_404_NOT_FOUND, config.getErrorPage("404"), ++redirNo));
}

ws_http::statuscodes_t    HttpHandler::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
{
    if ((statusCode >= ws_http::STATUS_400_BAD_REQUEST && redirNo > 1) || redirNo >= 10) {
        if (redirNo >= 10)
            statusCode = ws_http::STATUS_500_INTERNAL_SERVER_ERROR;
        _httpMessage.setResponse(statusCode, NULL, "text/html", "");
        return (statusCode);
    }

    HttpConfig config = _server.getHttpConfig(requestPathDecoded, _httpMessage.header.getHeader("host"));
    // std::cout << "filepath: " << config.getFilePath() << std::endl;
    Redirect redirect = config.getRedirection();
    if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
        _httpMessage.setResponse(redirect.redirectStatus, NULL, "", redirect.location);
        return (redirect.redirectStatus);
    }

    if (ws_http::versions_rev.at(_httpMessage.header.getHeader("@version")) != ws_http::VERSION_1_1)
        return (processResponse(ws_http::STATUS_505_HTTP_VERSION_NOT_SUPPORTED, "GET", config.getErrorPage("505"), ++redirNo));
    if (!config.checkAllowedMethod(method))
        return (processResponse(ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), ++redirNo));
    std::string const & cgiExecutablePath = config.getCgiExecutable();
    if (!cgiExecutablePath.empty()) {
        handleCgi(config);
        return (ws_http::STATUS_1001_INTERNAL_PROCESS_CGI);
    }

    std::string const & meth = _httpMessage.header.getHeader("@method");
    if (_httpMessage.header.getHeader("@method") == "POST")
        return (processResponsePost(config));
    else if (_httpMessage.header.getHeader("@method") == "DELETE")
        return (processResponseDelete(config));
    else if (_httpMessage.header.getHeader("@method") == "GET")
        return (processResponseGetHead(config, statusCode, requestPathDecoded, redirNo));
    
}


// ws_http::statuscodes_t  HttpHandler::processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, 
//                                                             std::string const & method, std::string const & requestPathDecoded, int redirNo )
// {
// //     std::cout << "FILEPLATH: " << config.getFilePath() << std::endl;
    
//     FileInfos file(config.getFilePath(), _httpMessage.getHeader(HttpMessage::pathDecoded));
//     if (file.getError() == 0 && S_ISREG(file.getMode())) {
// //         std::cout << "----> is a regular file" << std::endl;
//         std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary | std::ios::ate);
//         if (ifs.is_open()) {
//             _httpMessage
//                 .clearHeader()
//                 .setStatusLine(ws_http::VERSION_1_1, statusCode)
//                 .setHeader("connection", "keep-alive").setHeader("keep-alive", "timeout=1, max=1000")
//                 .clearBody()
//                 .setBody(ifs, config.getMimeType())
//                 .flushMessage();
//             ifs.close();
//             return (statusCode);
//         }
//     } else if (file.getError() == 0 && S_ISDIR(file.getMode())) {
// //         std::cout << "----> is a directory" << std::endl;
//         std::string const & indexFileName = file.checkContainedFile(config.getIndexFile());
// //         std::cout << "----> indexFileName:" << indexFileName << std::endl;
//         if (!indexFileName.empty())
//             return (processResponse(statusCode, method, requestPathDecoded + "/" + indexFileName, true));
//         if (!config.hasDirectoryListing())
//             return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
//         std::stringstream ss;
//         file.setDirListing(ss);
//         _httpMessage
//             .clearHeader()
//             .setStatusLine(ws_http::VERSION_1_1, statusCode)
//             .setHeader("connection", "keep-alive").setHeader("keep-alive", "timeout=1, max=1000")
//             .clearBody()
//             .setBody(ss, "text/html")
//             .flushMessage();
//         return (statusCode);
//     } else if (file.getError() == EACCES) {
// //         std::cout << "----> permission denied" << std::endl;
//         return (processResponse(ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), ++redirNo));
//     }
// //     std::cout << "----> not found" << std::endl;
//     return (processResponse(ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404"), ++redirNo));
// }

// ws_http::statuscodes_t    HttpHandler::processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo )
// {
// //     std::cout << "\n---------------- HttpHandler::processResponse ----------------\n"
//         << "status: " << ws_http::statuscodes.at(statusCode)
//         << " | method: " << method << " | requestPathDecoded: "
//         << requestPathDecoded << " | redirNo: "
//         << redirNo
//         << std::endl;

//     if ((statusCode >= ws_http::STATUS_400_BAD_REQUEST && redirNo > 1) || redirNo >= 10) {
//         if (redirNo >= 10)
//             statusCode = ws_http::STATUS_500_INTERNAL_SERVER_ERROR;
//         _httpMessage
//             .clearHeader()
//             .setStatusLine(ws_http::VERSION_1_1, statusCode)
//             .setHeader("connection", "keep-alive").setHeader("keep-alive", "timeout=1, max=1000")
//             .clearBody()
//             .setBodyError(statusCode)
//             .flushMessage();
//         return (statusCode);
//     }

//     HttpConfig config = _server.getHttpConfig(requestPathDecoded, _httpMessage.getHeader("host"));
// //     std::cout << "rootPath: " << config.getRootPath() << std::endl;
// //     std::cout << "filePath: " << config.getFilePath() << std::endl;
//     Redirect redirect = config.getRedirection();
//     if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
//         _httpMessage
//             .clearHeader()
//             .setStatusLine(ws_http::VERSION_1_1, redirect.redirectStatus)
//             .setHeader("connection", "keep-alive").setHeader("keep-alive", "timeout=1, max=1000")
//             .setHeader("location", redirect.location)
//             .clearBody()
//             .flushMessage();
//         return (redirect.redirectStatus);
//     }
//     if (!config.checkAllowedMethod(method))
//         return ( processResponse(ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), ++redirNo));
//     std::string const & cgiExecutablePath = config.getCgiExecutable();
// //     std::cout << "cgi executable: " << cgiExecutablePath << std::endl;
//     if (!cgiExecutablePath.empty()) {
//         handleCgi(config);
//         return (ws_http::STATUS_1001_INTERNAL_PROCESS_CGI);
//     }

//     return (processResponseFile(config, statusCode, method, requestPathDecoded, redirNo));
    
// }



// ws_http::statuscodes_t  HttpHandler::processResponsePost( HttpConfig const & config, ws_http::statuscodes_t statusCode )
// {
//     (void)config;
//     (void)statusCode;
//     _httpMessage.printMessage();
//     std::cout << "--- BODY: ---\n" << _httpMessage.getBody() << "\n-------------" << std::endl;
//     // exit(1);

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

//         std::cout << "handle multipart" << std::endl;

//         std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
//         std::cout << "boundary: " << boundary << std::endl;
//         std::string boundaryStart = "--";

//         std::string filename, relFilePath;

        
//         buff_t::const_iterator itStartFile = body.begin() + 2 + boundary.size() + ws_http::crlf.size();
//         buff_t::const_iterator itStart = itStartFile;
//         int fileNbr = 0;
//         // for (buff_t::const_iterator itBoundStart = body.begin(); itBoundStart + 2 < body.end(); itStart = itBoundStart + 2 + boundary.size()) {
//         for (buff_t::const_iterator itBoundStart = itStartFile; itBoundStart + 2 < body.end(); itStart = itBoundStart + 2) {
//             itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());

//             // std::cout << "\n\n\n ------------------------------- FOR ------------------" << std::endl;
//             // std::cout << "\n----- itStart - itBoundStart -----\n$" << std::string(itStart, itBoundStart) << "$" << std::endl;
//             // std::cout << "\n----- itStartFile - itBoundStart -----\n$" << std::string(itStartFile, itBoundStart) << "$" << std::endl;
//             // std::cout << "\n----- itBoundStart - END -----\n$" << std::string(itBoundStart, body.end()) << "$" << std::endl;
//             std::cout << "\n----- itStart - itBoundStart dist -----\n$" << std::distance(itStart, itBoundStart) << "$" << std::endl;
//             if (std::string(itBoundStart + 2, itBoundStart + 2 + boundary.size()) == boundary) {
//                 std::cout << "END FOUND" << std::endl;
//                 handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
//                 itStart = itStartFile = itBoundStart + 2 + boundary.size() + ws_http::crlf.size();
//                 fileNbr++;
//             }
            
//         }
//         std::cout << "fileno: " << fileNbr << std::endl;
//         if (fileNbr == 1) {
//             processResponseFile(config, ws_http::STATUS_201_CREATED, "GET", _httpMessage.header.getHeader("@path") + "/" + filename, 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "", _httpMessage.header.getHeader("@path") + "/" + filename);
//         } else {
//             processResponseFile(config, ws_http::STATUS_201_CREATED, "GET", _httpMessage.header.getHeader("@path"), 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "",  _httpMessage.header.getHeader("@path"));
//         }
//         return (ws_http::STATUS_201_CREATED);
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t  HttpHandler::processResponsePost( HttpConfig const & config, ws_http::statuscodes_t statusCode )
// {
//     (void)config;
//     (void)statusCode;
//     _httpMessage.printMessage();
//     std::cout << "--- BODY: ---\n" << _httpMessage.getBody() << "\n-------------" << std::endl;
//     // exit(1);

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

//         std::cout << "handle multipart" << std::endl;

//         std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
//         std::cout << "boundary: " << boundary << std::endl;
//         std::string boundaryStart = "--";

//         std::string filename, relFilePath;

//         buff_t::const_iterator itStart = body.begin();
//         buff_t::const_iterator itStartFile = body.begin();
//         int fileNbr = 0;
//         // for (buff_t::const_iterator itBoundStart = body.begin(); itBoundStart + 2 < body.end(); itStart = itBoundStart + 2 + boundary.size()) {
//         for (buff_t::const_iterator itBoundStart = body.begin(); itBoundStart + 2 < body.end(); itStart = itBoundStart + 2) {
//             itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());
//             std::cout << "\n\n\n ------------------------------- FOR ------------------" << std::endl;
//             // std::cout << "\n----- itStart - itBoundStart -----\n$" << std::string(itStart, itBoundStart) << "$" << std::endl;
//             std::cout << "\n----- itStartFile - itBoundStart -----\n$" << std::string(itStartFile, itBoundStart) << "$" << std::endl;
//             std::cout << "\n----- itBoundStart - END -----\n$" << std::string(itBoundStart, body.end()) << "$" << std::endl;
//             std::cout << "\n----- itStart - itBoundStart dist -----\n$" << std::distance(itStart, itBoundStart) << "$" << std::endl;
//             if (itBoundStart + 2 >= body.end())
//                 break ;
//             if (itBoundStart == body.begin())
//                 itStartFile = itBoundStart + 2 + boundary.size() + ws_http::crlf.size();
//             else if (std::string(itBoundStart + 2, itBoundStart + 2 + boundary.size()) == boundary) {
//                 std::cout << "END FOUND" << std::endl;
//                 handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
//                 itStartFile = itBoundStart + 2 + boundary.size() + ws_http::crlf.size();
//                 fileNbr++;
//             }
            
//         }
//         std::cout << "fileno: " << fileNbr << std::endl;
//         if (fileNbr == 1) {
//             processResponseFile(config, ws_http::STATUS_201_CREATED, "GET", _httpMessage.header.getHeader("@path") + "/" + filename, 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "", _httpMessage.header.getHeader("@path") + "/" + filename);
//         } else {
//             processResponseFile(config, ws_http::STATUS_201_CREATED, "GET", _httpMessage.header.getHeader("@path"), 0);
//             // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "",  _httpMessage.header.getHeader("@path"));
//         }
//         return (ws_http::STATUS_201_CREATED);
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


// ws_http::statuscodes_t  HttpHandler::processResponsePost( HttpConfig const & config, ws_http::statuscodes_t statusCode )
// {
//     (void)config;
//     (void)statusCode;
//     _httpMessage.printMessage();
//     // std::cout << "--- BODY: ---\n" << _httpMessage.getBody() << "\n-------------" << std::endl;
//     // exit(1);

//     std::string const & body = _httpMessage.getBody();
//     FileInfos uploadDir(config.getFilePath(), _httpMessage.header.getHeader("@pathdecoded"));
//     if (uploadDir.getError() == 0 && !S_ISDIR(uploadDir.getMode()))
//         return (processResponseFile(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED, "GET", config.getErrorPage("405"), 0));
//     if (uploadDir.getError() == EACCES)
//         return (processResponseFile(config, ws_http::STATUS_403_FORBIDDEN, "GET", config.getErrorPage("403"), 0));
//     if (uploadDir.getError() != 0)
//         return (processResponseFile(config, ws_http::STATUS_404_NOT_FOUND, "GET", config.getErrorPage("404s"), 0));

//     std::string contentType = _httpMessage.header.getHeader("content-type");
//     std::size_t sepPos1 = contentType.find("multipart/form-data");
//     std::size_t sepPos2 = contentType.find("boundary=") + 9;
//     if (sepPos1 == 0 && sepPos2 != std::string::npos) {
//         std::cout << "handle multipart" << std::endl;
//         std::string boundStart = "--";


//         for (; contentType[sepPos2] == '-'; ++sepPos2);
//         std::string const & boundary = contentType.substr(sepPos2, contentType.size() - sepPos2);
//         std::cout << "boundary: " << boundary << std::endl;

//         buff_t::const_iterator itBound = body.begin();
//         buff_t::const_iterator itStart = body.begin();
//         while (itStart < body.end()) {
//             itBound = std::search(itStart, body.end(), boundStart.begin(), boundStart.end());
//             if (itBound == itStart)
//                 continue;
//             for (; *itBound == '-'; ++itBound);
//             std::cout << "bound found? : " << std::string(itBound, itBound + boundary.size()) << std::endl;
//             if (itBound == itStart || std::string(itBound, itBound + boundary.size()) != boundary) {
//                 itStart = itBound;
//                 continue;
//             }
//             // itBound += boundary.size();
//             std::cout << "data found: " << std::string(itStart, itBound) << std::endl;
            
            
//             HttpHeader fileHeader;
//             int dist = fileHeader.parseHeader(&(*itStart), &(*itBound), false);
//             if (dist == 2)
//                 exit(1);
//             std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
//             std::size_t fnamepos = contentDisposition.find("filename="); //error?
//             std::string filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
//             std::cout << "filename: " << filename << std::endl;
//             std::cout << "FILEDATA: " << std::string(&(*itStart) + dist, &(*itBound)) << std::endl;

//             itStart = itBound + boundary.size() + ws_http::crlf.size();

//         }

//     }
//     return (ws_http::STATUS_UNDEFINED);
// }
