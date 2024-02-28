/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 23:29:43 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

// std::string getPollEventStr(short events)
// {
// 	std::string str = "";
// 	if (events & POLLIN)
// 		str += "POLLIN ";
// 	if (events & POLLPRI)
// 		str += "POLLPRI ";
// 	if (events & POLLOUT)
// 		str += "POLLOUT ";
// 	if (events & POLLRDNORM)
// 		str += "POLLRDNORM ";
// 	if (events & POLLWRNORM)
// 		str += "POLLWRNORM ";
// 	if (events & POLLRDBAND)
// 		str += "POLLRDBAND ";
// 	if (events & POLLWRBAND)
// 		str += "POLLWRBAND ";
// 	if (events & POLLEXTEND)
// 		str += "POLLEXTEND ";
// 	if (events & POLLATTRIB)
// 		str += "POLLATTRIB ";
// 	if (events & POLLNLINK)
// 		str += "POLLNLINK ";
// 	if (events & POLLWRITE)
// 		str += "POLLWRITE ";
// 	if (events & POLLERR)
// 		str += "POLLERR ";
// 	if (events & POLLHUP)
// 		str += "POLLHUP ";
// 	if (events & POLLNVAL)
// 		str += "POLLNVAL ";

// 	return (str);
// }

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
		logging("\n -------- quitCgiProcess -------- ", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		_polling.stopMonitoringFd(_childProcessHandler);
		_childProcessHandler = NULL;
	}
}

void    HttpHandler::quit( void )
{
	quitCgiProcess();
	logging("\n -------- quitClientConnection -------- ", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	return (_polling.stopMonitoringFd(this));
}

void    HttpHandler::handleTimeout( void )
{
    if(_httpMessage.isCgi()){
		quitCgiProcess();
        _httpMessage = HttpMessage(NULL);
        _httpMessage.setResponse(ws_http::STATUS_504_GATEWAY_TIMEOUT, NULL, "", "");
    } else if (_httpMessage.getStatus() == ws_http::STATUS_UNDEFINED) {
        _httpMessage.setResponse(ws_http::STATUS_408_REQUEST_TIMEOUT, NULL, "", "");
    } else {
        quit();
    }
}

void    HttpHandler::handleEvent( struct pollfd pollfd )
{
	if (pollfd.revents & POLLERR)
		logging("POLLERR CLIENT", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);

	if (pollfd.revents & POLLHUP) {
		logging("\n -------- handleEvent - quit - POLLHUP ", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (quit());
	}
	if (pollfd.revents & POLLIN && !_httpMessage.responseSet() && !_httpMessage.isCgi()) {
		int readBytes = _httpMessage.readFromSocketAndParseHttp(pollfd.fd, 0);
		if (readBytes == -1 || readBytes == 0) {
			logging("\n -------- handleEvent - quit - readBytes: ", readBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (quit());
		}
		if (_httpMessage.getStatus() == ws_http::STATUS_200_OK) {
			_httpMessage.printMessage(0);
			processResponse(_httpMessage.getStatus());
		} else if (_httpMessage.getStatus() != ws_http::STATUS_UNDEFINED) {
			_httpMessage.setResponse(_httpMessage.getStatus(), NULL, "", "");
		}
	}
	if (pollfd.revents & POLLOUT && _httpMessage.responseSet() && !_httpMessage.isCgi()) {
		int sendBytes = _httpMessage.sendDataToSocket(pollfd.fd, 0);
		if (sendBytes == -1) {
			logging("\n -------- handleEvent - quit - sendBytes: ", sendBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (quit());
		} else if (sendBytes == 0) {
			logging("\n ------- RESPONSE SENT, RESET HTTP MESSAGE ------- ", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			_httpMessage = HttpMessage(&_server);
		}
	}
}

void    HttpHandler::handleChildEvent( struct pollfd & pollfd )
{
	if (pollfd.revents & POLLERR)
		logging("POLLERR CHILD", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);

	if (pollfd.revents & POLLHUP) {
		_httpMessage.setCgiResponseDone();
		return (quitCgiProcess());
	}
	if (pollfd.revents & POLLIN && !_httpMessage.responseSet() && _httpMessage.isCgi()) {
		int readBytes = _httpMessage.readFromSocketAndParseHttp(pollfd.fd, 0);
		if (_httpMessage.getStatus() >= ws_http::STATUS_400_BAD_REQUEST){
			quitCgiProcess();
			_httpMessage =HttpMessage(&_server);
			return (_httpMessage.setResponse(ws_http::STATUS_502_BAD_GATEWAY, NULL, "", ""));
		}
		if (readBytes == -1) {
			logging("HandleEvent - quit - readBytes: ", readBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (quit());
		} else if (readBytes == 0) {

		} else {

		}
	}
	if (pollfd.revents & POLLOUT && _httpMessage.responseSet() && _httpMessage.isCgi()) {
		int sendBytes = _httpMessage.sendDataToSocket(pollfd.fd, 0);
		if (sendBytes == -1) {
			logging("HandleEvent - quit - sendBytes: ", sendBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (quit());
		} else if (sendBytes == 0) {

		} else {

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
	logging("\n------ handleCgi -------\n", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	_childProcessHandler = new ChildProcessHandler(_polling, *this);

	_childProcessHandler->addArgument(config.getFilePath());
	_childProcessHandler->addEnvVariable(std::string("QUERY_STRING=") + _httpMessage.header.getHeader("@query"));
	_childProcessHandler->addEnvVariable(std::string("REQUEST_METHOD=") + _httpMessage.header.getHeader("@method"));
	_childProcessHandler->addEnvVariable(std::string("CONTENT_TYPE=") + _httpMessage.header.getHeader("content-type"));
	_childProcessHandler->addEnvVariable(std::string("CONTENT_LENGTH=") + toStr(_httpMessage.getBody().size()));
	_childProcessHandler->addEnvVariable(std::string("SCRIPT_FILENAME=") + config.getFilePath());
	_childProcessHandler->addEnvVariable(std::string("DOCUMENT_ROOT=") + config.getRootPath());
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

	try
	{
		// if (!_childProcessHandler->createChildProcess(config.getCgiExecutable(), config.getFilePath().substr(0, config.getFilePath().find_last_of('/')))) {
		_childProcessHandler->createChildProcess(config.getCgiExecutable(), config.getRootPath());
		_httpMessage.prepareCgi();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		quitCgiProcess();
		return (processError(config, ws_http::STATUS_500_INTERNAL_SERVER_ERROR));
	}
	

}

void  HttpHandler::processResponse( ws_http::statuscodes_t currentStatus )
{
	std::string const & method = _httpMessage.header.getHeader("@method");
	std::string const & version = _httpMessage.header.getHeader("@version");
	std::string const & pathDecoded = _httpMessage.header.getHeader("@pathdecoded");
	HttpConfig			config = _server.getHttpConfig(pathDecoded, _httpMessage.header.getHeader("host"));
	FileInfo			fileInfo(config.getFilePath(), true);

	logging("PROCESS RESPONSE\t", method, pathDecoded, version, config.getFilePath());

	if (currentStatus >= ws_http::STATUS_400_BAD_REQUEST) {
		if (!fileInfo.checkInfo(FileInfo::EXISTS))
			return (_httpMessage.setResponse(ws_http::STATUS_404_NOT_FOUND, NULL, "", ""));
		if (!fileInfo.checkInfo(FileInfo::READABLE))
			return (_httpMessage.setResponse(ws_http::STATUS_403_FORBIDDEN, NULL, "", ""));
	}
	
	// check version
	if (version != "HTTP/1.1") {
		logging("--> Version not supprtet, return 505: version: ", version, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (processError(config, ws_http::STATUS_505_HTTP_VERSION_NOT_SUPPORTED));
	}
	// check redirection
	Redirect redirect = config.getRedirection();
	if (redirect.redirectStatus != ws_http::STATUS_UNDEFINED) {
		logging("-> Return Redirect of: ", ws_http::statuscodes.at(redirect.redirectStatus), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (_httpMessage.setResponse(redirect.redirectStatus, NULL, "", redirect.location));
	}
	// if (!fileInfo.readStat(config.getFilePath(), true))
	if (!fileInfo.checkInfo(FileInfo::EXISTS)) {
		logging("-> Ressource does not exist: ", pathDecoded, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (processError(config, ws_http::STATUS_404_NOT_FOUND));
	}
	// check method allowed
	if (!config.checkAllowedMethod(method)) {
		logging("-> Method not supportet, return 405: version: ", method, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
	}
	// check if dir and indexFile
	if (fileInfo.checkInfo(FileInfo::IS_DIRECTORY)) {
		std::vector<std::string> const & indexFiles = config.getIndexFile();
		for (std::vector<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); ++it) {
			if (!fileInfo.checkContainedFile(*it))
				continue;
			if (!_httpMessage.header.reparseRequestLine(method, pathDecoded[pathDecoded.size() - 1] == '/' ? pathDecoded + *it : pathDecoded + "/" + *it))
				return(_httpMessage.setResponse(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, NULL, "", ""));
			logging("is Directory: index found: ", *it, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (processResponse(currentStatus));
		}
	}
	std::string const & cgiExecutablePath = config.getCgiExecutable();
	if (!cgiExecutablePath.empty())
		return (handleCgi(config));
	if (method == "POST")
		return (processPost(config, fileInfo));
	if (method == "DELETE")
		return (processDelete(config, fileInfo));
	if (method == "GET")
		return (processGetHead(config, fileInfo, currentStatus));
	return (processError(config, ws_http::STATUS_501_NOT_IMPLEMENTED));
}

void  HttpHandler::processError( HttpConfig const & config, ws_http::statuscodes_t errorCode )
{
	logging("PROCESS ERROR\t", ws_http::statuscodes.at(errorCode), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	std::string const & errorPageUri = config.getErrorPage(errorCode);
	if (errorPageUri.empty())
		return (_httpMessage.setResponse(errorCode, NULL, "", ""));
	if (!_httpMessage.header.reparseRequestLine("GET", errorPageUri))
		return(_httpMessage.setResponse(ws_http::STATUS_500_INTERNAL_SERVER_ERROR, NULL, "", ""));
	processResponse(errorCode);
}


void  HttpHandler::processGetHead( HttpConfig const & config, FileInfo const & fileInfo, ws_http::statuscodes_t statusCode )
{
	logging("\n------ processGetHead -------\n", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	
	if (!fileInfo.checkInfo(FileInfo::READABLE))
		return (processError(config, ws_http::STATUS_403_FORBIDDEN));

	// check if file
	if (fileInfo.checkInfo(FileInfo::IS_FILE)) {
		logging("--> is File. Return File Content", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		std::ifstream   ifs(config.getFilePath().c_str(), std::ios::binary);
		if (!ifs.is_open())
			return (processError(config, ws_http::STATUS_403_FORBIDDEN));
		_httpMessage.setResponse(statusCode, &ifs, config.getMimeType(), "");
		return (ifs.close());
	}
	// if dir
	if (!config.hasDirectoryListing()) {
		logging("---> No Dirlisting: Return 403", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		return (processError(config, ws_http::STATUS_403_FORBIDDEN));
	}
	logging("---> Set Dirlisting", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	std::stringstream ss;
	fileInfo.setDirListing(ss, _httpMessage.header.getHeader("@pathdecoded"));
	return (_httpMessage.setResponse(statusCode, &ss, "text/html", ""));
}

bool    createFile(std::string const & filePath, const char *start, const char *end)
{
	// logging("UPLOAD HERE: ", filePath, " | size: ", end - start, EMPTY_STRING);
	std::ofstream ofs(filePath.c_str(), std::ofstream::binary);
	if (!ofs.is_open()) {
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
	// logging(fileHeader.getHeader("content-disposition"), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	// logging(fileHeader.getHeader("content-type"), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
	std::size_t fnamepos = contentDisposition.find("filename="); //error?
	// logging("filename: ", filename, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
	std::string filePath = rootPath + "/" + filename;
	return (createFile(filePath, start + dist, end));
}


void  HttpHandler::processPost( HttpConfig const & config, FileInfo const & fileInfo )
{
	logging("\n------ processPost -------\n", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	std::string const & body = _httpMessage.getBody();

	if (!fileInfo.checkInfo(FileInfo::IS_DIRECTORY))
		return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));
	if (!fileInfo.checkInfo(FileInfo::READABLE) || !fileInfo.checkInfo(FileInfo::WRITEABLE))
	    return (processError(config, ws_http::STATUS_403_FORBIDDEN));
	// if (!fileInfo.checkInfo(FileInfo::READABLE))
	// 	return (processError(config, ws_http::STATUS_403_FORBIDDEN));
	// if (!fileInfo.checkInfo(FileInfo::WRITEABLE & FileInfo::READABLE))
	// 	return (processError(config, ws_http::STATUS_403_FORBIDDEN));

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
	// FileInfos uploadedDir(config.getFilePath(), R_OK | W_OK, true);
	FileInfo uploadedDir(config.getFilePath(), true);
	std::stringstream ss;
	uploadedDir.setDirListing(ss, _httpMessage.header.getHeader("@pathdecoded"));
	_httpMessage.printMessage(0);
	std::cout << "path: " << _httpMessage.header.getHeader("@path") << std::endl;
	return (_httpMessage.setResponse(ws_http::STATUS_201_CREATED, &ss, "text/html", _httpMessage.header.getHeader("@path")));
}

void  HttpHandler::processDelete( HttpConfig const & config, FileInfo const & fileInfo )
{
	logging("\n------ processDelete -------\n", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);

	if (!fileInfo.checkInfo(FileInfo::IS_FILE))
		return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));

	std::string filePath = config.getFilePath().substr(0, config.getFilePath().find_last_of('/') + 1);
	FileInfo   directoryPath(filePath, false);
	if (!directoryPath.checkInfo(FileInfo::READABLE) || !directoryPath.checkInfo(FileInfo::WRITEABLE) || !directoryPath.checkInfo(FileInfo::EXECUTABLE))
		return (processError(config, ws_http::STATUS_403_FORBIDDEN));

	std::remove(config.getFilePath().c_str());
	_httpMessage.setResponse(ws_http::STATUS_204_NO_CONTENT, NULL, "", "");

}
