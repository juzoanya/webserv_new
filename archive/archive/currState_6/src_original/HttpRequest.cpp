/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 19:52:43 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

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
    buff_t::const_iterator pos = it_start;
    if (this->_parseRequestLine) {
        pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        std::string         version;
        std::stringstream   ss(std::string(it_start, pos));
        ss >> this->method;
        ss >> this->fullUri;
        ss >> version;
        if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
            return (ws_http::STATUS_400_BAD_REQUEST);
        }
        std::size_t queryPos = this->fullUri.find_first_of('?');
        this->path = this->fullUri.substr(0, queryPos);
        this->pathDecoded = getStringUrlDecoded(this->path);
        if (queryPos != std::string::npos)
            this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
        pos += ws_http::crlf.size();
    }

    for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
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



