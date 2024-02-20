/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/02/10 23:16:49 by mberline         ###   ########.fr       */
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

HttpRequest::HttpRequest( HttpServer* server, bool parseReqLine, buff_t *customBodyBuffer )
 :  status(ws_http::STATUS_UNDEFINED), _bodyBuffer(),
    _usedBodyBuffer(customBodyBuffer ? customBodyBuffer : &_bodyBuffer), _server(server),
    _contentLength(0), _maxBodySize(4096), _chunkSize(0),
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
    return (*this->_usedBodyBuffer);
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
        if (ws_http::versions.find(tversion) == ws_http::versions.end() || ws_http::methods.find(tmethod) == ws_http::methods.end())
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



// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator itStartKey = it_start;
//     buff_t::const_iterator itEndKey = it_start;
//     buff_t::const_iterator itStartValue = it_start;
//     buff_t::const_iterator lastNonWhitespace = it_start;
//     bool parseKey = true;
//     bool valueStartFound = false;
//     bool crFound = false;

//     for (; it_start != it_end; ++it_start)
//     {
//         unsigned char c = *it_start;
//         if (parseKey) {
//             if (c == ':') {
//                 parseKey = false;
//                 itEndKey = it_start;
//             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 return (false);
//             }
//         } else if (crFound && c == '\n') {
//             std::string key = std::string(itStartKey, itEndKey);
//             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//             if (hasHeader(key))
//                 return (false);
//             this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
//             parseKey = true;
//             valueStartFound = false;
//             crFound = false;
//             itStartKey = it_start + 1;
//         } else if (c >= 128 || std::isprint(c)) {
//             crFound = false;
//             if (!valueStartFound) {
//                 itStartValue = it_start;
//                 valueStartFound = true;
//             } else {
//                 lastNonWhitespace = it_start;
//             }
//         } else if(c == '\r') {
//             crFound = true;
//         } else if (c == ' ' || c == '\t') {
//             crFound = false;
//         } else {
//             return (false);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }




// ws_http::statuscodes_t  HttpRequest::parseRequestLine(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     std::string tmethod, tfulluri, tversion, tpath, tpathdecoded, tquery;


//     for (; it_start < it_end; ++it_start) {
//         unsigned char c = *it_start;
//     }

//     char *res = new char[str.size()];
//     std::size_t k = 0;
//     for (std::size_t pos = 0; pos < str.size(); pos++, k++) {
//         if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
//             || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
//             int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
//             int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
//             res[k] = a + b;
//             pos += 2;
//         } else if (str[pos]) {
//             res[k] = str[pos];
//         }
//     }
//     std::string result = std::string(res, k);
//     delete[] res;
//     return (result);
// }

// ws_http::statuscodes_t    HttpRequest::parseHeaders( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// {
//     while (itStart < itEnd) {
//         const buff_t::const_iterator itStartKey = itStart;
//         while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
//             itStart++;
//         if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         const buff_t::const_iterator itEndKey = itStart;
//         itStart++;
//         while (*itStart == ' ' || *itStart == '\t')
//             itStart++;
//         const buff_t::const_iterator itStartValue = itStart;
//         buff_t::const_iterator lastNonWhitespace = itStart;
//         bool crFound = false;
//         for (;itStart != itEnd; ++itStart) {
//             unsigned char c = *itStart;
//             if (c == '\n' && crFound) {
//                 std::string key = std::string(itStartKey, itEndKey);
//                 std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//                 if (hasHeader(key))
//                     return (ws_http::STATUS_400_BAD_REQUEST);
//                 this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
//                 itStart++;
//                 break ;
//             }
//             if (c == '\r' && !crFound) {
//                 crFound = true;
//             } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
//                 lastNonWhitespace = itStart;
//                 crFound = false;
//             } else if ((c == ' ' || c == '\t') && !crFound) {
//                 crFound = false;
//             } else {
//                 return (ws_http::STATUS_400_BAD_REQUEST);
//             }
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


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
        if (static_cast<long>(this->_usedBodyBuffer->size()) + buffDist > this->_contentLength)
            return (ws_http::STATUS_400_BAD_REQUEST);
        if (static_cast<long>(this->_usedBodyBuffer->size()) + buffDist > this->_maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(it_start, it_end, std::back_inserter(*this->_usedBodyBuffer));
        if (static_cast<long>(this->_usedBodyBuffer->size()) == this->_contentLength)
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
        } else if ((unsigned long)_chunkSize > 0 && _usedBodyBuffer->size() + 1 > (unsigned long)_maxBodySize) {
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        } else if (!_parseSize && _chunkSize > 0) {
            _usedBodyBuffer->push_back(c);
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








// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator pos = it_start;
//     if (this->_parseRequestLine) {
//         pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         std::string         version;
//         std::stringstream   ss(std::string(it_start, pos));

//         std::getline(ss, this->method, ' ');
//         std::getline(ss, this->fullUri, ' ');
//         std::getline(ss, version);
//         if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         std::size_t queryPos = this->fullUri.find_first_of('?');
//         this->path = this->fullUri.substr(0, queryPos);
//         this->pathDecoded = getStringUrlDecoded(this->path);
//         if (queryPos != std::string::npos)
//             this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
//         pos += ws_http::crlf.size();
//     }

//     for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
//         pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (!addHeader(start, pos)) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator pos = it_start;
//     if (this->_parseRequestLine) {
//         pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         std::string         version;
//         std::stringstream   ss(std::string(it_start, pos));
//         std::getline(ss, this->method, ' ');
//         std::getline(ss, this->fullUri, ' ');
//         std::getline(ss, version);
//         if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         std::size_t queryPos = this->fullUri.find_first_of('?');
//         this->path = this->fullUri.substr(0, queryPos);
//         this->pathDecoded = getStringUrlDecoded(this->path);
//         if (queryPos != std::string::npos)
//             this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
//         pos += ws_http::crlf.size();
//     }

//     for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
//         pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (!addHeader(std::string(start, pos), this->_headerMap)) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator itStartKey = it_start;
//     buff_t::const_iterator itEndKey = it_start;
//     buff_t::const_iterator itStartValue = it_start;
//     buff_t::const_iterator lastNonWhitespace = it_start;
//     bool parseKey = true;
//     bool valueStartFound = false;
//     bool crFound = false;

//     for (; it_start != it_end; ++it_start)
//     {
//         unsigned char c = *it_start;
//         if (parseKey) {
//             if (c == ':') {
//                 parseKey = false;
//                 itEndKey = it_start;
//             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 return (false);
//             }
//         } else if (crFound && c == '\n') {
//             std::string key = std::string(itStartKey, itEndKey);
//             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//             if (hasHeader(key))
//                 return (false);
//             this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
//             parseKey = true;
//             valueStartFound = false;
//             crFound = false;
//             itStartKey = it_start + 1;
//         } else if (c >= 128 || std::isprint(c)) {
//             crFound = false;
//             if (!valueStartFound) {
//                 itStartValue = it_start;
//                 valueStartFound = true;
//             } else {
//                 lastNonWhitespace = it_start;
//             }
//         } else if(c == '\r') {
//             crFound = true;
//         } else if (c == ' ' || c == '\t') {
//             crFound = false;
//         } else {
//             return (false);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


// buff_t::const_iterator    HttpRequest::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//         return (it_start + 1);
//     }
//     if (this->_splitCase != 0) {
//         it_start += this->_splitCase;
//         this->_splitCase = 0;
//     }
//     buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         return (it_start);
//     }
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, itSize, std::back_inserter(this->_buffer));
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//     } else {
//         this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//     }
//     return (itSize + ws_http::crlf.size());
// }

// // ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->_contentLength >= 0) {
// //         long buffDist = std::distance(it_start, it_end);
// //         if (static_cast<long>(this->_bodyBuffer.size()) + buffDist > this->_contentLength)
// //             return (ws_http::STATUS_400_BAD_REQUEST);
// //         if (static_cast<long>(this->_bodyBuffer.size()) + buffDist > this->_maxBodySize)
// //             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
// //         std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
// //         if (static_cast<long>(this->_bodyBuffer.size()) == this->_contentLength)
// //             return (ws_http::STATUS_200_OK);
// //         return (ws_http::STATUS_UNDEFINED);
// //     }
// //     buff_t::const_iterator content_start = it_start;
// //     while (std::distance(it_start, it_end) > 0) {
// //         if (this->_chunkSize == 0) {
// //             content_start = this->setChunkSize(it_start, it_end);
// //             if (content_start == it_start)
// //                 return (ws_http::STATUS_UNDEFINED);
// //             if (this->_chunkSize == 0)
// //                 return (ws_http::STATUS_200_OK);
// //         }
// //         long dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
// //         long rest_dist = std::distance(content_start + dist, it_end);
// //         if (static_cast<long>(this->_bodyBuffer.size()) + dist > this->_maxBodySize)
// //             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
// //         std::copy(content_start, content_start + dist, std::back_inserter(this->_bodyBuffer));
// //         this->_chunkSize -= dist;
// //         if (this->_chunkSize == 0 && rest_dist == 0) {
// //             this->_splitCase = 2;
// //         } else if (this->_chunkSize == 0 && rest_dist == 1) {
// //             this->_splitCase = 1;
// //         }
// //         it_start = content_start + dist + ws_http::crlf.size();
// //     }
// //     return (ws_http::STATUS_UNDEFINED);
// // }




// ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     bool    crFound = false;
//     bool    parseSize = true;
//     std::string sizeStr;
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;

//         if (crFound && c != '\n')
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         crFound = false;
//         if (c == '\r' && parseSize) {
//             _chunkSize = strtol(sizeStr.c_str(), NULL, 16);
//             sizeStr.clear();
//             crFound = true;
//             parseSize = false;
//         } else if (c == '\r') {
//             crFound = true;
//             parseSize = true;
//         } else if (c == '\n') {

//         } else if (_chunkSize > 0) {
//             _usedBodyBuffer.push_back(c);
//             _chunkSize--;
//         } else {
//             sizeStr.push_back(c);
//         }
//     }


//     return (ws_http::STATUS_UNDEFINED);
// }
































// std::string getStringUrlDecoded(std::string const & str)
// {
//     char *res = new char[str.size()];
//     std::size_t k = 0;
//     for (std::size_t pos = 0; pos < str.size(); pos++, k++) {
//         if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
//             || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
//             int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
//             int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
//             res[k] = a + b;
//             pos += 2;
//         } else if (str[pos]) {
//             res[k] = str[pos];
//         }
//     }
//     std::string result = std::string(res, k);
//     delete[] res;
//     return (result);
// }

// HttpRequest::HttpRequest( HttpServer* server, bool parseReqLine )
//  : status(ws_http::STATUS_UNDEFINED), _server(server), _headerDone(false), _parseRequestLine(parseReqLine)
// { }

// HttpRequest::~HttpRequest( void )
// { }


// void HttpRequest::printRequest( void ) const
// {
// //     // std::cout << " --- REQUEST HEADER --- " << std::endl;
// //     // std::cout << " --- Request-Line --- " << std::endl;
// //     // std::cout << "Request Method: " << this->method << std::endl;
// //     // std::cout << "Request Target PathEncoded: " << this->path << std::endl;
// //     // std::cout << "Request Target PathDecoded: " << this->pathDecoded << std::endl;
// //     // std::cout << "Request Target Query: " << this->query << std::endl;
// //     // std::cout << "Request Target FullUri: " << this->fullUri << std::endl;
// //     // std::cout << " --- Header-Fields --- " << std::endl;
//     // for (http_header_map_t::const_iterator it = this->headerMap.begin(); it != this->headerMap.end(); ++it) {
// //     //     // std::cout << it->first << ": " << it->second << std::endl;
//     // }
// //     // std::cout << std::endl;
// //     // std::cout << " --- REQUEST BODY Size: " << this->requestBody.bodyBuffer.size() << "--- " << std::endl;
// //     // // std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + std::min(1000ul, this->requestBody.bodyBuffer.size())) << std::endl;
// //     // std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + this->requestBody.bodyBuffer.size()) << std::endl;
// }

// bool    addHeader(std::string const & line, http_header_map_t& headers)
// {
//     std::size_t keyEnd = line.find(':');
//     if (keyEnd == std::string::npos || keyEnd == 0)
//         return (false);
//     for (std::size_t pos = 0; pos != keyEnd; ++pos) {
//         if (!std::isalnum(line[pos]) && ws_http::headerTchar.find(line[pos]) == std::string::npos)
//             return (false);
//     }
//     std::string token = line.substr(0, keyEnd);
//     if (headers.find(token) != headers.end())
//         return (false);
//     std::size_t valueStart = line.find_first_not_of(" \t", keyEnd + 1);
//     std::size_t valueEnd = line.find_last_not_of(" \t") + 1;
//     for (std::size_t pos = valueStart; pos != valueEnd; ++pos) {
//         uint8_t c = line[valueStart];
//         if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
//             continue ;
//         return (false);
//     }
//     headers.insert(std::make_pair(token, line.substr(valueStart, valueEnd - valueStart)));
//     return (true);
// }

// std::string const & HttpRequest::getHeader( std::string const & key )
// {
//     http_header_map_t::const_iterator it = this->headerMap.find(key);
//     if (it != this->headerMap.end())
//         return (it->second);
//     return (ws_http::dummyValue);
// }

// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator pos = it_start;
//     if (this->_parseRequestLine) {
//         pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         std::string         version;
//         std::stringstream   ss(std::string(it_start, pos));
//         ss >> this->method;
//         ss >> this->fullUri;
//         ss >> version;
//         if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         std::size_t queryPos = this->fullUri.find_first_of('?');
//         this->path = this->fullUri.substr(0, queryPos);
//         this->pathDecoded = getStringUrlDecoded(this->path);
//         if (queryPos != std::string::npos)
//             this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
//         pos += ws_http::crlf.size();
//     }

//     for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
//         pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (!addHeader(std::string(start, pos), this->headerMap)) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (this->_server && this->_headerDone) {
//         this->status = this->requestBody.parseBody(it_start, it_end);
//     } else {
//         std::size_t buffSize = this->_buffer.size();
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
//             ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//         this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
//         if (end == this->_buffer.end())
//             return (ws_http::STATUS_UNDEFINED);
//         this->status = this->parseHeaders(this->_buffer.begin(), end);
//         if (this->status == ws_http::STATUS_UNDEFINED) {
//             if (getHeader("host").empty()) {
//                 this->status = ws_http::STATUS_400_BAD_REQUEST;
//             } else {
//                 this->_headerDone = true;
//                 if (this->_server) {
//                     long maxBodySize = this->_server->getHttpConfig(this->pathDecoded, getHeader("host")).getMaxBodySize();
//                     if (getHeader("transfer-encoding") != "") 
//                         this->requestBody = HttpRequestBody(-1, maxBodySize);
//                     else
//                         this->requestBody = HttpRequestBody(strtol(getHeader("content-length").c_str(), NULL, 10), maxBodySize);
//                     this->status = this->requestBody.parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
//                 }
//                 this->_buffer.clear();
//                 if (!this->_server)
//                     return (ws_http::STATUS_200_OK);
//             }
//         }
//     }
//     return (this->status);
// }


// HttpRequestBody::HttpRequestBody( void )
//  : _contentLength(0), _maxBodySize(0), _chunkSize(0), _splitCase(0)
// { }

// HttpRequestBody::HttpRequestBody( long contLen, long maxRequestBodySize )
//  : _contentLength(contLen), _maxBodySize(maxRequestBodySize), _chunkSize(0), _splitCase(0)
// { }

// HttpRequestBody::~HttpRequestBody( void )
// { }

// buff_t::const_iterator    HttpRequestBody::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//         return (it_start + 1);
//     }
//     if (this->_splitCase != 0) {
//         it_start += this->_splitCase;
//         this->_splitCase = 0;
//     }
//     buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         return (it_start);
//     }
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, itSize, std::back_inserter(this->_buffer));
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//     } else {
//         this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//     }
//     return (itSize + ws_http::crlf.size());
// }

// ws_http::statuscodes_t  HttpRequestBody::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->_contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->_maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
//         if (static_cast<long>(this->bodyBuffer.size()) == this->_contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     }
//     buff_t::const_iterator content_start = it_start;
//     while (std::distance(it_start, it_end) > 0) {
//         if (this->_chunkSize == 0) {
//             content_start = this->setChunkSize(it_start, it_end);
//             if (content_start == it_start)
//                 return (ws_http::STATUS_UNDEFINED);
//             if (this->_chunkSize == 0)
//                 return (ws_http::STATUS_200_OK);
//         }
//         long dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
//         long rest_dist = std::distance(content_start + dist, it_end);
//         if (static_cast<long>(this->bodyBuffer.size()) + dist > this->_maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(content_start, content_start + dist, std::back_inserter(this->bodyBuffer));
//         this->_chunkSize -= dist;
//         if (this->_chunkSize == 0 && rest_dist == 0) {
//             this->_splitCase = 2;
//         } else if (this->_chunkSize == 0 && rest_dist == 1) {
//             this->_splitCase = 1;
//         }
//         it_start = content_start + dist + ws_http::crlf.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }



