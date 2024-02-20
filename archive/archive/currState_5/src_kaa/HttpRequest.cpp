/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/02/06 11:30:34 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

HttpHeaders::HttpHeaderField HttpHeaders::dummyHeader;

HttpHeaders::HttpHeaderField::HttpHeaderField( void )
{ }

HttpHeaders::HttpHeaderField::HttpHeaderField( std::string const & token, std::string const & value )
 : _token(token), _value(value)
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
}

HttpHeaders::HttpHeaderField::HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue )
 : _token(itStartKey, itEndKey), _value(itStartValue, itEndValue)
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
}

HttpHeaders::HttpHeaderField::~HttpHeaderField( void )
{ }

bool HttpHeaders::HttpHeaderField::operator==( HttpHeaderField const & headerField ) const { return (headerField._token == this->_token); }
bool HttpHeaders::HttpHeaderField::operator==( std::string const & key ) const { return (key == this->_token); }
bool HttpHeaders::HttpHeaderField::operator!=( HttpHeaderField const & headerField ) const { return (headerField._token != this->_token); }
bool HttpHeaders::HttpHeaderField::operator!=( std::string const & key ) const { return (key != this->_token); }
std::string const & HttpHeaders::HttpHeaderField::getField( void ) const { return (this->_token); }
std::string const & HttpHeaders::HttpHeaderField::getValue( void ) const { return (this->_value); }

HttpHeaders::HttpHeaders( void )
{ }

HttpHeaders::~HttpHeaders( void )
{ }

bool    HttpHeaders::addHeader( std::string const & line ) { return (this->addHeader(line.begin(), line.end())); }

bool    HttpHeaders::addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    const buff_t::const_iterator itStartKey = itStart;
    const buff_t::const_iterator itEndKey = std::find(itStart, itEnd, ':');
    if (itEndKey == itEnd)
        return (false);
    for (; itStart != itEndKey; ++itStart) {
        if (!std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
            return (false);
    }
    itStart++;
    while (*itStart == ' ' || *itStart == '\t')
        itStart++;
    while (*itEnd == ' ' || *itEnd == '\t')
        itEnd--;
    for (buff_t::const_iterator it = itStart; it != itEnd; ++it) {
        unsigned char c = *it;
        if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
            continue ;
        return (false);
    }
    HttpHeaderField headerField(itStartKey, itEndKey, itStart, itEnd);
    if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
        return (false);
    this->_headers.push_back(headerField);
    return (true);
}

bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    while (itStart < itEnd) {
        const buff_t::const_iterator itStartKey = itStart;
        while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
            itStart++;
        if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
            return (false);
        const buff_t::const_iterator itEndKey = itStart;
        itStart++;
        while (*itStart == ' ' || *itStart == '\t')
            itStart++;
        const buff_t::const_iterator itStartValue = itStart;
        buff_t::const_iterator lastNonWhitespace = itStart;
        bool crFound = false;
        for (;itStart != itEnd; ++itStart) {
            unsigned char c = *itStart;
            if (c == '\n' && crFound) {
                HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
                if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
                    return (false);
                this->_headers.push_back(headerField);
                itStart++;
                break ;
            }
            if (c == '\r' && !crFound) {
                crFound = true;
            } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
                lastNonWhitespace = itStart;
                crFound = false;
            } else if ((c == ' ' || c == '\t') && !crFound) {
                crFound = false;
            } else {
                return (false);
            }
        }
    }
    return (true);
}

std::string const & HttpHeaders::getHeaderValue( std::string const & key ) const { return (this->getHeader(key).getValue()); }

HttpHeaders::HttpHeaderField const & HttpHeaders::getHeader( std::string const & key ) const
{
    std::vector<HttpHeaderField>::const_iterator it = std::find(this->_headers.begin(), this->_headers.end(), key);
    if (it != this->_headers.end())
        return (*it);
    return (HttpHeaders::dummyHeader);
}

std::vector<HttpHeaders::HttpHeaderField> const & HttpHeaders::getHeadersVec( void ) const { return (this->_headers); }

std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs )
{
    if (!rhs.getField().empty())
        os << rhs.getField() << ": " << rhs.getValue() << "$" << "\r\n";
    return (os);
}

std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs )
{
    std::vector<HttpHeaders::HttpHeaderField> const & headers = rhs.getHeadersVec();
    for (std::vector<HttpHeaders::HttpHeaderField>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        os << *it;
    return (os);
}

int main()
{
    HttpHeaders headers;
    std::cout << "size: " << sizeof(headers) << std::endl;
    // headers.addHeader("Statfus: fjigfwrgw");
    // headers.addHeader("Stafdtus: fjigfwrgw");
    // headers.addHeader("Stfsdsatus: fjigfwrgw");
    std::string headerbuf = "eins:  fjigfwrgw    \r\nzwo:fwfwe\r\ndrei:fwfwe\r\nvier:fwfwe\r\nvierf:fwfwe";
    headers.addHeaderBuf(headerbuf.begin(), headerbuf.end());
    std::cout << "size: " << sizeof(headers) << std::endl;
    std::cout << headers;
    // std::cout << headers.getHeaderValue("status") << std::endl;
    // std::cout << headers.getHeader("status") << std::endl;
    return (0);
}









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

HttpRequest::HttpRequest( OnHttpRequestHeaderDoneFunctor* headerDoneFunc, bool parseReqLine )
 : status(ws_http::STATUS_UNDEFINED), _headerDoneFunc(headerDoneFunc), _headerDone(false), _parseRequestLine(parseReqLine)
{ }

HttpRequest::~HttpRequest( void )
{ }


void HttpRequest::printRequest( void ) const
{
    // std::cout << " --- REQUEST HEADER --- " << std::endl;
    // std::cout << " --- Request-Line --- " << std::endl;
    // std::cout << "Request Method: " << this->method << std::endl;
    // std::cout << "Request Target PathEncoded: " << this->path << std::endl;
    // std::cout << "Request Target PathDecoded: " << this->pathDecoded << std::endl;
    // std::cout << "Request Target Query: " << this->query << std::endl;
    // std::cout << "Request Target FullUri: " << this->fullUri << std::endl;
    // std::cout << " --- Header-Fields --- " << std::endl;
    for (http_header_map_t::const_iterator it = this->headerMap.begin(); it != this->headerMap.end(); ++it) {
        // std::cout << it->first << ": " << it->second << std::endl;
    }
    // std::cout << std::endl;
    // std::cout << " --- REQUEST BODY Size: " << this->requestBody.bodyBuffer.size() << "--- " << std::endl;
    // // std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + std::min(1000ul, this->requestBody.bodyBuffer.size())) << std::endl;
    // std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + this->requestBody.bodyBuffer.size()) << std::endl;
}

bool    addHeader(std::string const & line, http_header_map_t& headers)
{
    std::size_t keyEnd = line.find(':');
    if (keyEnd == std::string::npos || keyEnd == 0)
        return (false);
    for (std::size_t pos = 0; pos != keyEnd; ++pos) {
        if (!std::isalnum(line[pos]) && ws_http::headerTchar.find(line[pos]) == std::string::npos)
            return (false);
    }
    std::string token = line.substr(0, keyEnd);
    std::transform(token.begin(), token.end(), token.begin(), tolower);
    if (headers.find(token) != headers.end())
        return (false);
    std::size_t valueStart = line.find_first_not_of(" \t", keyEnd + 1);
    std::size_t valueEnd = line.find_last_not_of(" \t") + 1;
    for (std::size_t pos = valueStart; pos != valueEnd; ++pos) {
        uint8_t c = line[valueStart];
        if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
            continue ;
        return (false);
    }
    headers.insert(std::make_pair(token, line.substr(valueStart, valueEnd - valueStart)));
    return (true);
}

ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    // if (this->_parseRequestLine) {
    //     buff_t::const_iterator endl = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    //     std::string         version;
    //     std::stringstream   ss(std::string(it_start, pos));
    //     std::getline(ss, this->method, ' ');
    //     std::getline(ss, this->fullUri, ' ');
    //     std::getline(ss, version);
    //     if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end())
    //         return (ws_http::STATUS_400_BAD_REQUEST);
    //     std::size_t queryPos = this->fullUri.find_first_of('?');
    //     this->path = this->fullUri.substr(0, queryPos);
    //     this->pathDecoded = getStringUrlDecoded(this->path);
    //     if (queryPos != std::string::npos)
    //         this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
    //     pos += ws_http::crlf.size();
    // }
    // for (buff_t::const_iterator pos = it_start; it_start < it_end; it_start = pos + ws_http::crlf.size()) {
    //     pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    //     if (!addHeader(std::string(it_start, pos), this->headerMap))
    //         return (ws_http::STATUS_400_BAD_REQUEST);
    // }

    buff_t::const_iterator pos = it_start;
    if (this->_parseRequestLine) {
        pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        std::string         version;
        std::stringstream   ss(std::string(it_start, pos));
        std::getline(ss, this->method, ' ');
        std::getline(ss, this->fullUri, ' ');
        std::getline(ss, version);
        if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end())
            return (ws_http::STATUS_400_BAD_REQUEST);
        std::size_t queryPos = this->fullUri.find_first_of('?');
        this->path = this->fullUri.substr(0, queryPos);
        this->pathDecoded = getStringUrlDecoded(this->path);
        if (queryPos != std::string::npos)
            this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
        pos += ws_http::crlf.size();
    }
    for (buff_t::const_iterator start = pos + ws_http::crlf.size(); start < it_end; start = pos + ws_http::crlf.size()) {
        pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        if (!addHeader(std::string(start, pos), this->headerMap)) {
            return (ws_http::STATUS_400_BAD_REQUEST);
        }
    }
    return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->status != ws_http::STATUS_UNDEFINED)
        return (this->status);
    if (this->_headerDoneFunc && this->_headerDone) {
        this->status = this->requestBody.parseBody(it_start, it_end);
    } else {
        std::size_t buffSize = this->_buffer.size();
        std::copy(it_start, it_end, std::back_inserter(this->_buffer));
        buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
            ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
        this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
        if (end == this->_buffer.end())
            return (ws_http::STATUS_UNDEFINED);
        this->status = this->parseHeaders(this->_buffer.begin(), end);
        if (this->status == ws_http::STATUS_UNDEFINED) {
            std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itHostHeader = this->headerMap.find("host");
            if (itHostHeader == this->headerMap.end()) {
                this->status = ws_http::STATUS_400_BAD_REQUEST;
            } else {
                this->_headerDone = true;
                this->hostHeader = itHostHeader->second;
                if (this->_headerDoneFunc) {
                    this->requestBody = HttpRequestBody(*this, this->_headerDoneFunc->operator()(this->pathDecoded, this->hostHeader));
                    this->status = this->requestBody.parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
                }
                this->_buffer.clear();
                if (!this->_headerDoneFunc)
                    return (ws_http::STATUS_200_OK);
            }
        }
    }
    return (this->status);
}


// ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (this->_headerDoneFunc && this->_headerDone) {
//         this->status = this->requestBody.parseBody(it_start, it_end);
//     } else {
//         std::size_t buffSize = this->_buffer.size();
//         this->endPos = std::search(it_start, it_end, ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//         std::copy(it_start, this->endPos, std::back_inserter(this->_buffer));
//         //  = it_start + ((end - this->_buffer.begin()) - buffSize);
//         if (this->endPos == it_end)
//             return (ws_http::STATUS_UNDEFINED);
//         this->status = this->parseHeaders(this->_buffer.begin(), this->endPos);
//         if (this->status == ws_http::STATUS_UNDEFINED) {
//             std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itHostHeader = this->headerMap.find("host");
//             if (itHostHeader == this->headerMap.end()) {
//                 this->status = ws_http::STATUS_400_BAD_REQUEST;
//             } else {
//                 this->_headerDone = true;
//                 this->hostHeader = itHostHeader->second;
//                 if (this->_headerDoneFunc) {
//                     this->requestBody = HttpRequestBody(*this, this->_headerDoneFunc->operator()(this->pathDecoded, this->hostHeader));
//                     this->status = this->requestBody.parseBody(this->endPos + ws_http::httpHeaderEnd.size(), it_end);
//                 }
//                 this->_buffer.clear();
//                 if (!this->_headerDoneFunc)
//                     return (ws_http::STATUS_200_OK);
//             }
//         }
//     }
//     return (this->status);
// }


HttpRequestBody::HttpRequestBody( void )
 : maxBodySize(0), _contentLength(0), _chunkSize(0), _splitCase(0)
{ }

HttpRequestBody::HttpRequestBody( HttpRequest const & request, long maxRequestBodySize )
 : maxBodySize(maxRequestBodySize), _contentLength(0), _chunkSize(0), _splitCase(0)
{
    std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itContLen = request.headerMap.find("content-length");
    std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itTransEnc = request.headerMap.find("transfer-encoding");
    if (itContLen != request.headerMap.end())
        this->_contentLength = strtol(itContLen->second.c_str(), NULL, 10);
    else if (itTransEnc != request.headerMap.end())
        this->_contentLength = -1;
}

HttpRequestBody::~HttpRequestBody( void )
{ }

buff_t::const_iterator    HttpRequestBody::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
        this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
        this->_buffer.clear();
        return (it_start + 1);
    }
    if (this->_splitCase != 0) {
        it_start += this->_splitCase;
        this->_splitCase = 0;
    }
    buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    if (itSize == it_end) {
        std::copy(it_start, it_end, std::back_inserter(this->_buffer));
        return (it_start);
    }
    if (!this->_buffer.empty()) {
        std::copy(it_start, itSize, std::back_inserter(this->_buffer));
        this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
        this->_buffer.clear();
    } else {
        this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
    }
    return (itSize + ws_http::crlf.size());
}

ws_http::statuscodes_t  HttpRequestBody::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_contentLength >= 0) {
        long buffDist = std::distance(it_start, it_end);
        if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->_contentLength)
            return (ws_http::STATUS_400_BAD_REQUEST);
        if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
        if (static_cast<long>(this->bodyBuffer.size()) == this->_contentLength)
            return (ws_http::STATUS_200_OK);
        return (ws_http::STATUS_UNDEFINED);
    }
    buff_t::const_iterator content_start = it_start;
    while (std::distance(it_start, it_end) > 0) {
        if (this->_chunkSize == 0) {
            content_start = this->setChunkSize(it_start, it_end);
            if (content_start == it_start)
                return (ws_http::STATUS_UNDEFINED);
            if (this->_chunkSize == 0)
                return (ws_http::STATUS_200_OK);
        }
        long dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
        long rest_dist = std::distance(content_start + dist, it_end);
        if (static_cast<long>(this->bodyBuffer.size()) + dist > this->maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(content_start, content_start + dist, std::back_inserter(this->bodyBuffer));
        this->_chunkSize -= dist;
        if (this->_chunkSize == 0 && rest_dist == 0) {
            this->_splitCase = 2;
        } else if (this->_chunkSize == 0 && rest_dist == 1) {
            this->_splitCase = 1;
        }
        it_start = content_start + dist + ws_http::crlf.size();
    }
    return (ws_http::STATUS_UNDEFINED);
}



