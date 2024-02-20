/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 17:09:24 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::string HttpRequest::method = "@method";
std::string HttpRequest::path = "@path";
std::string HttpRequest::pathDecoded = "@pathdecoded";
std::string HttpRequest::query = "@query";
std::string HttpRequest::fullUri = "@fulluri";
std::string HttpRequest::version = "@version";

std::string getStringUrlDecoded(std::string const & str)
{
    char *res = new char[str.size()];
    std::size_t k = 0;
    for (std::size_t pos = 0; pos < str.size(); pos++, k++) {
        if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
            || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
            int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
            int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
            res[k] = a + b;
            pos += 2;
        } else if (str[pos]) {
            res[k] = str[pos];
        }
    }
    std::string result = std::string(res, k);
    delete[] res;
    return (result);
}

HttpRequest::HttpRequest( HttpServer* server, bool parseReqLine )
 : status(ws_http::STATUS_UNDEFINED), _server(server), _contentLength(0), _maxBodySize(4096), _chunkSize(0),
    _headerDone(false), _parseRequestLine(parseReqLine), _crFound(false), _parseSize(true), _bodyDone(false)
{ }

HttpRequest::~HttpRequest( void )
{ }


void HttpRequest::printRequest( void ) const
{
    std::cout << " --- REQUEST HEADER --- " << std::endl;
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
    }
    std::cout << std::endl;
    std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
    std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
    std::cout << " ---------------------------------------------------- " << std::endl;
}

std::string const & HttpRequest::getHeader( std::string const & key ) const
{
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        if (_headerMap[i].first == key) {
            return (_headerMap[i].second);
        }
    }
    return (ws_http::dummyValue);
}

bool    HttpRequest::hasHeader( std::string const & key ) const
{
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        if (_headerMap[i].first == key)
            return (true);
    }
    return (false);
}

buff_t const &  HttpRequest::getBody( void ) const
{
    return (this->_bodyBuffer);
}

HttpRequest::headers_t const &  HttpRequest::getHeaderVector( void ) const
{
    return (this->_headerMap);
}

ws_http::statuscodes_t  HttpRequest::getStatus( void ) const
{
return (this->status);
}

bool    HttpRequest::addHeader(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
   const buff_t::const_iterator itStartKey = it_start;
    const buff_t::const_iterator itEndKey = std::find(it_start, it_end, ':');
    if (itEndKey == it_end)
        return (false);
    for (; it_start != itEndKey; ++it_start) {
        if (!std::isalnum(*it_start) && ws_http::headerTchar.find(*it_start) == std::string::npos)
            return (false);
    }
    it_start++;
    while (*it_start == ' ' || *it_start == '\t')
        it_start++;
    while (*it_end == ' ' || *it_end == '\t')
        it_end--;
    for (buff_t::const_iterator it = it_start; it != it_end; ++it) {
        unsigned char c = *it;
        if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
            continue ;
        return (false);
    }
    std::string key = std::string(itStartKey, itEndKey);
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (hasHeader(key))
        return (false);
    _headerMap.push_back(std::make_pair(key, std::string(it_start, it_end)));
    return (true);
}

ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    buff_t::const_iterator pos = it_start;
    if (this->_parseRequestLine) {
        pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        buff_t::const_iterator itSep1 = it_end, itSep2 = it_end;
        itSep1 = std::find(it_start, pos, ' ');
        if (itSep1 != it_end)
            itSep2 = std::find(itSep1 + 1, pos, ' ');
        if (itSep1 == pos || itSep2 == pos)
            return (ws_http::STATUS_400_BAD_REQUEST);
        std::string tmethod = std::string(it_start, itSep1);
        std::string tfulluri = std::string(itSep1 + 1, itSep2);
        std::string tversion = std::string(itSep2 + 1, pos);
        std::string tpath, tpathdecoded, tquery;
        if (ws_http::versions_rev.find(tversion) == ws_http::versions_rev.end() || ws_http::methods_rev.find(tmethod) == ws_http::methods_rev.end())
            return (ws_http::STATUS_400_BAD_REQUEST);
        std::size_t queryPos = tfulluri.find_first_of('?');
        tpath = tfulluri.substr(0, queryPos);
        tpathdecoded = getStringUrlDecoded(tpath);
        if (queryPos != std::string::npos)
            tquery = tfulluri.substr(queryPos + 1, std::string::npos);
        pos += ws_http::crlf.size();
        _headerMap.push_back(std::make_pair(path, tpath));
        _headerMap.push_back(std::make_pair(pathDecoded, tpathdecoded));
        _headerMap.push_back(std::make_pair(method, tmethod));
        _headerMap.push_back(std::make_pair(fullUri, tfulluri));
        _headerMap.push_back(std::make_pair(version, tversion));
        _headerMap.push_back(std::make_pair(query, tquery));
    }

    for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
        pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        if (!addHeader(start, pos)) {
            return (ws_http::STATUS_400_BAD_REQUEST);
        }
    }
    return (ws_http::STATUS_UNDEFINED);
}


ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->status != ws_http::STATUS_UNDEFINED)
        return (this->status);
    if (this->_server && this->_headerDone) {
        this->status = this->parseBody(it_start, it_end);
        return (this->status);
    }
    // std::size_t buffSize = this->_buffer.size();
    std::copy(it_start, it_end, std::back_inserter(this->_buffer));
    buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
        ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
    // this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
    if (end == this->_buffer.end())
        return (ws_http::STATUS_UNDEFINED);
    this->status = this->parseHeaders(this->_buffer.begin(), end);
    if (this->status != ws_http::STATUS_UNDEFINED)
        return (this->status);
    if (getHeader("host").empty()) {
        this->status = ws_http::STATUS_400_BAD_REQUEST;
    } else {
        this->_headerDone = true;
        if (this->_server) {
            this->_maxBodySize = _server->getHttpConfig(pathDecoded, getHeader("host")).getMaxBodySize();
            this->_contentLength = getHeader("transfer-encoding") != ""
                ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
            this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
        }
        this->_buffer.clear();
        if (!this->_server)
            return (ws_http::STATUS_200_OK);
    }
    return (this->status);
}

ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_contentLength >= 0) {
        long buffDist = std::distance(it_start, it_end);
        if (static_cast<long>(this->_bodyBuffer.size()) + buffDist > this->_contentLength)
            return (ws_http::STATUS_400_BAD_REQUEST);
        if (static_cast<long>(this->_bodyBuffer.size()) + buffDist > this->_maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
        if (static_cast<long>(this->_bodyBuffer.size()) == this->_contentLength)
            return (ws_http::STATUS_200_OK);
        return (ws_http::STATUS_UNDEFINED);
    } else {
        return (parseBodyChunked(it_start, it_end));
    }
}

ws_http::statuscodes_t  HttpRequest::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    for (; it_start != it_end; ++it_start) {
        unsigned char c = *it_start;
        if (_chunkSize == 0 && c == '\r') {
            _crFound = true;
            continue;
        } 
        if (!_parseSize && _crFound && c == '\n') {
            _parseSize = true;
            if (_bodyDone)
                return (ws_http::STATUS_200_OK);
        } else if (_parseSize && _crFound && c == '\n') {
            _chunkSize = strtol(_buffer.c_str(), NULL, 16);
            _buffer.clear();
            if (_chunkSize == 0)
                _bodyDone = true;
            _parseSize = false;
        } else if ((unsigned long)_chunkSize > 0 && _bodyBuffer.size() + 1 > (unsigned long)_maxBodySize) {
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        } else if (!_parseSize && _chunkSize > 0) {
            _bodyBuffer.push_back(c);
            _chunkSize--;
        } else if (_parseSize && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            _buffer.push_back(c);
        } else {
            return (ws_http::STATUS_400_BAD_REQUEST);
        }
        _crFound = false;
    }
    return (ws_http::STATUS_UNDEFINED);
}

