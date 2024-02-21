/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 11:31:42 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 20:18:38 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::string getStateStr(short state)
{
	std::string str;
	if (state & HttpMessage::PARSE_SIZE)
		str += "PARSE_SIZE ";
	if (state & HttpMessage::CR_FOUND)
		str += "CR_FOUND ";
	if (state & HttpMessage::HEADER_DONE)
		str += "HEADER_DONE ";
	if (state & HttpMessage::BODY_DONE)
		str += "BODY_DONE ";
	if (state & HttpMessage::BODY_CHUNKED)
		str += "BODY_CHUNKED ";
	if (state & HttpMessage::MAKE_CGI)
		str += "MAKE_CGI ";
	if (state & HttpMessage::IS_RESPONSE_FROM_CGI)
		str += "IS_RESPONSE_FROM_CGI ";
	if (state & HttpMessage::RESPONSE_SET)
		str += "RESPONSE_SET ";
	return (str);
}

std::size_t  HttpMessage::readBufferSize = 4096;
char         HttpMessage::readBuffer[4096];

HttpMessage::HttpMessage( void ) 
 :  _buffer2(), _dataPtr(NULL), _maxBodySizeGetter(NULL), _contentLength(0), _maxBodySize(readBufferSize), _status(ws_http::STATUS_UNDEFINED), _state(PARSE_SIZE)
{ }

HttpMessage::HttpMessage( IOnHttpMessageBodySize* maxBodySizeGetter )
 :  _buffer2(), _dataPtr(NULL), _maxBodySizeGetter(maxBodySizeGetter), _contentLength(0), _maxBodySize(readBufferSize), _status(ws_http::STATUS_UNDEFINED), _state(PARSE_SIZE) 
{ }

HttpMessage::~HttpMessage( void )
{ }

ws_http::statuscodes_t  HttpMessage::getStatus( void ) const
{
	return (_status);
}

bool    HttpMessage::isCgi( void ) const
{
	return (_state & MAKE_CGI);
}

bool    HttpMessage::responseSet( void ) const
{
	return (_state & RESPONSE_SET);
}

buff_t const &  HttpMessage::getBody( void ) const
{
	if (_status != ws_http::STATUS_UNDEFINED)
		return (_buffer2);
	return (ws_http::dummyValue);
}


// void HttpMessage::printMessage( void ) const
// {
// 	HttpHeader::headers_t const & head = header.getHeaderVector();
// 	logging(" --- REQUEST HEADER --- ", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
// 	for (std::size_t i = 0; i != head.size(); ++i) {
// 		std::cout << head[i].first << ": " << head[i].second << " | size header-value: " << head[i].second.size() << std::endl;
// 	}
// 	std::cout << std::endl;
// 	std::cout << " --- REQUEST BODY Size: " << getBody().size() << " --- " << std::endl;
// 	std::cout << " ---------------------------------------------------- " << std::endl;
// }


ws_http::statuscodes_t  HttpMessage::parseBody(const char* start, const char* end)
{
	if (!(_state & BODY_CHUNKED)) {
		std::size_t buffDist = end - start;
		if (_buffer2.size() + buffDist > _contentLength)
			return (ws_http::STATUS_400_BAD_REQUEST);
		if (_buffer2.size() + buffDist > _maxBodySize)
			return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
		std::copy(start, end, std::back_inserter(_buffer2));
		if (_buffer2.size() == _contentLength) {
			return (ws_http::STATUS_200_OK);
		}
	} else {
		for (; start != end; ++start) {
			ws_http::statuscodes_t status;
			if (_state & PARSE_SIZE)
				status = parseBodyChunkSize(*start);
			else
				status = parseBodyChunk(*start);
			if (status != ws_http::STATUS_UNDEFINED)
				return (status);
		}
	}
	return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t  HttpMessage::parseBodyChunkSize(unsigned char c)
{
	if (c == '\r') {
		_state |= CR_FOUND;
	} else if ((_state & CR_FOUND) && c == '\n') {
		_state &= ~PARSE_SIZE;
		if (_contentLength == 0)
			_state |= BODY_DONE;
		_state &= ~CR_FOUND;
	} else if (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
		_contentLength = 16 * _contentLength + getCharFromHex(c);
		_state &= ~CR_FOUND;
	} else {
		return (ws_http::STATUS_400_BAD_REQUEST);
	}
	return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t  HttpMessage::parseBodyChunk(unsigned char c)
{
	if (_contentLength > 0) {
		if (_buffer2.size() + 1 > _maxBodySize)
			return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
		_buffer2.push_back(c);
		_contentLength--;
		_state &= ~CR_FOUND;
	} else if (_contentLength == 0 && c == '\r'){
		_state |= CR_FOUND;
	} else if ((_state & CR_FOUND) && c == '\n') {
		_state |= PARSE_SIZE;
		_state &= ~CR_FOUND;
		if ((_state & BODY_DONE))
			return (ws_http::STATUS_200_OK);
	} else {
		return (ws_http::STATUS_400_BAD_REQUEST);
	}
	return (ws_http::STATUS_UNDEFINED);
}


void    HttpMessage::prepareCgi( void )
{
	header = HttpHeader();
	_state |= PARSE_SIZE;
	_state &= ~CR_FOUND;
	_state &= ~HEADER_DONE;
	_state &= ~BODY_CHUNKED;
	_state &= ~BODY_DONE;
	_status = ws_http::STATUS_UNDEFINED;
	_state |= MAKE_CGI;
	if (_buffer2.size() > 0) {
		_state |= RESPONSE_SET;
		_dataPtr = _buffer2.data();
		_contentLength = _buffer2.size();
	}
}


void    HttpMessage::setCgiResponseDone( void )
{
	if (!(_state & MAKE_CGI))
		return ;
	_contentLength = _buffer2.size();
	_maxBodySize = _buffer2.size();
	HttpHeader::headers_t const & headVec = header.getHeaderVector();
	std::string const & status = header.getHeader("status");
	_buffer2 += ws_http::versions.at(ws_http::VERSION_1_1) + " " + (status.empty() ? ws_http::statuscodes.at(ws_http::STATUS_200_OK) : status) + ws_http::crlf;
	for (HttpHeader::headers_t::const_iterator it = headVec.begin(); it != headVec.end(); ++it) {
		if (it->first[0] != '@' && it->first != "status")
			_buffer2 += it->first + ": " + it->second + ws_http::crlf;
	}
	if (!header.hasHeader("date"))
		_buffer2 += "date: " + getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") + ws_http::crlf;
	if (!header.hasHeader("connection")) {
		_buffer2 += "connection: keep-alive" + ws_http::crlf;
		_buffer2 += "keep-alive: timeout=1, max=100" + ws_http::crlf;
	}
	_buffer2 += "content-length: " + toStr(_maxBodySize) + ws_http::crlf;
	_buffer2 += ws_http::crlf;

	_state &= ~MAKE_CGI;
	_state |= RESPONSE_SET;
	if (_maxBodySize > 0)
		_state |= IS_RESPONSE_FROM_CGI;
	_dataPtr = _buffer2.data() + _maxBodySize;
	_contentLength = _buffer2.size() - _maxBodySize;
}

void    HttpMessage::setResponse( ws_http::statuscodes_t status, std::istream *data, std::string const & mimeType, std::string const & location )
{
	
	header = HttpHeader();
	_buffer2.clear();
	_buffer2 += ws_http::versions.at(ws_http::VERSION_1_1) + " " + ws_http::statuscodes.at(status) + ws_http::crlf;
	if (!location.empty())
		_buffer2 += "location: " + location + ws_http::crlf;
	_buffer2 += "connection: keep-alive" + ws_http::crlf;
	_buffer2 += "keep-alive: timeout=1, max=100" + ws_http::crlf;
	_buffer2 += "date: " + getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") + ws_http::crlf;
	if (data) {
		std::size_t bodySize = getStreamSize(*data);
		std::string bodySizeStr = toStr(bodySize);
		_buffer2 += "content-type: " + mimeType + ws_http::crlf;
		_buffer2 += "content-length: " + bodySizeStr + ws_http::crlf;
		_buffer2 += ws_http::crlf;
		addStreamToBuff(_buffer2, *data, bodySize);
	} else if (status >= ws_http::STATUS_400_BAD_REQUEST) {
		std::string errStr;
		errStr +=  "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">";
		errStr += "<title>" + ws_http::statuscodes.at(status) + "</title>" "</head>";
		errStr += "<body>" "<div style=\"text-align: center;\">";
		errStr += "<h1>" + ws_http::statuscodes.at(status) + "</h1>";
		errStr += "<hr>" "<p>" + ws_http::webservVersion + "</p>" "</div>";
		errStr += "</body>" "</html>";
		_buffer2 += "content-type: text/html" + ws_http::crlf;
		_buffer2 += "content-length: " + toStr(errStr.size()) + ws_http::crlf;
		_buffer2 += ws_http::crlf;
		_buffer2 += errStr;
	} else {
		_buffer2 += "content-type: " + mimeType + ws_http::crlf;
		_buffer2 += "content-length: 0" + ws_http::crlf;
		_buffer2 += ws_http::crlf;
	}
	_state |= RESPONSE_SET;
	_dataPtr = _buffer2.data();
	_contentLength = _buffer2.size();
}

int     HttpMessage::readFromSocketAndParseHttp( int socketfd, int flags )
{
	int readBytes = recv(socketfd, readBuffer, readBufferSize, flags);
	const std::string loggingString = std::string(readBuffer, readBufferSize);
	logging("\n-------- data read ---------\n", loggingString , "\n------------------------------", EMPTY_STRING, EMPTY_STRING);
	if (readBytes == -1 || readBytes == 0)
		return (readBytes);
	if (_status != ws_http::STATUS_UNDEFINED)
		return (readBytes);

	if (_state & HEADER_DONE) {
		_status = parseBody(readBuffer, readBuffer + readBytes);
	} else {
		int parseHeaderRes = header.parseHeader(readBuffer, readBuffer + readBytes, !(_state & MAKE_CGI));
		if (parseHeaderRes == -1 || (!(_state & MAKE_CGI) && parseHeaderRes > 0 && !header.hasHeader("host"))) {
			_status = ws_http::STATUS_400_BAD_REQUEST;
		} else if (parseHeaderRes > 0) {
			_state |= HEADER_DONE;
			if (_maxBodySizeGetter)
				_maxBodySize = (*_maxBodySizeGetter)(header.getHeader("host"), header.getHeader("@pathdecoded"));
			if (_state & MAKE_CGI) {
				_contentLength = std::numeric_limits<std::size_t>::max();
				_maxBodySize = std::numeric_limits<std::size_t>::max();
			}else if (header.getHeader("transfer-encoding") == "chunked") {
				_state |= BODY_CHUNKED;
			} else {
				_contentLength = strtol(header.getHeader("content-length").c_str(), NULL, 10);
			}
			if (readBuffer + parseHeaderRes <= readBuffer + readBytes)
				_status = parseBody(readBuffer + parseHeaderRes, readBuffer + readBytes);
		}
	}
	return (readBytes);
}

int     HttpMessage::sendDataToSocket( int socketfd, int flags )
{
	if (_contentLength == 0)
		return (0);
	int sendBytes = send(socketfd, _dataPtr, _contentLength, flags);
	if (sendBytes == -1 || sendBytes == 0)
		return (sendBytes);
	_dataPtr += sendBytes;
	_contentLength -= sendBytes;
	if (_contentLength == 0 && _state & IS_RESPONSE_FROM_CGI) {
		_dataPtr = _buffer2.data();
		_contentLength = _maxBodySize;
		_state &= ~IS_RESPONSE_FROM_CGI;
	}
	return (sendBytes);
}
