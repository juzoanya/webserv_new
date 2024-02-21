/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 20:30:42 by juzoanya         ###   ########.fr       */
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

void    HttpHandler::handleEvent( struct pollfd & pollfd )
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
		if (_httpMessage.getStatus() != ws_http::STATUS_UNDEFINED)
			processResponse(_httpMessage.getStatus());
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
		quitCgiProcess();
		return ;
	}
	if (pollfd.revents & POLLIN && !_httpMessage.responseSet()) {
	   int readBytes = _httpMessage.readFromSocketAndParseHttp(pollfd.fd, 0);
		if (readBytes == -1) {
			logging("\n -------- handleEvent - quit - readBytes: ", readBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return (quit());
		}
	}
	if (pollfd.revents & POLLOUT && _httpMessage.responseSet()) {
		int sendBytes = _httpMessage.sendDataToSocket(pollfd.fd, 0);
		if (sendBytes == -1) {
			logging("\n -------- handleEvent - quit - sendBytes: ", sendBytes, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
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
		logging("CHECKING IF CHILD PROCESS IS DONE", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
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
	logging("\n------ processGetHead -------\n", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	// check if accessible and exists
	FileInfos fileInfo(config.getFilePath(), R_OK, true);
	logging("filePath: ", config.getFilePath(), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
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
	logging("UPLOAD HERE: ", filePath, " | size: ", end - start, EMPTY_STRING);
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
	logging(fileHeader.getHeader("content-disposition"), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	logging(fileHeader.getHeader("content-type"), EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
	std::size_t fnamepos = contentDisposition.find("filename="); //error?
	logging("filename: ", filename, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
	std::string filePath = rootPath + "/" + filename;
	return (createFile(filePath, start + dist, end));
}


void  HttpHandler::processPost( HttpConfig const & config )
{
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
