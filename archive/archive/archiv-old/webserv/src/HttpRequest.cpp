/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 21:55:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

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
    return (result);
}

const std::string HttpRequest::crlf = "\r\n";
const std::string HttpRequest::headEnd = "\r\n\r\n";

HttpRequest::HttpRequest( void )
 : status(ws_http::STATUS_UNDEFINED), headerDone(false), maxBodySize(4096), _contentLength(0)
{ }

HttpRequest::~HttpRequest( void )
{ }

ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    buff_t::const_iterator pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    std::string         version;
    std::stringstream   ss(std::string(it_start, pos));
    std::getline(ss, this->method, ' ');
    std::getline(ss, this->fullUri, ' ');
    std::getline(ss, version);

    if (ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end())
        return (ws_http::STATUS_400_BAD_REQUEST);
    std::size_t queryPos = this->fullUri.find_first_of('?');
    this->path = this->fullUri.substr(0, queryPos);
    this->pathDecoded = getStringUrlDecoded(this->path);
    if (queryPos != std::string::npos)
        this->query = this->fullUri.substr(queryPos + 1, std::string::npos);

    for (buff_t::const_iterator start = pos + ws_http::crlf.size(); start < it_end; start = pos + ws_http::crlf.size()) {
        pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        buff_t::const_iterator it_sep = std::find(start, pos, ':');
        if (it_sep == pos)
            return (ws_http::STATUS_400_BAD_REQUEST);
        std::string token = std::string(start, it_sep);
        std::string value = std::string(it_sep + 1, pos);
        if (value.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos
        || token.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos
        || this->headerMap.find(token) != this->headerMap.end())
            return (ws_http::STATUS_400_BAD_REQUEST);
        value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());
        this->headerMap[token] = value;
        // std::string value = std::string(it_sep + 1, pos);
        // std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
        // std::string value = s.erase(std::remove_if(s.begin(), s.end(), std::isspace), s.end());
        // this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
    }
    return (ws_http::STATUS_UNDEFINED);
}


buff_t::const_iterator    HttpRequest::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
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

ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_contentLength >= 0) {
        std::size_t buffDist = std::distance(it_start, it_end);
        if (this->bodyBuffer.size() + buffDist > this->_contentLength)
            return (ws_http::STATUS_400_BAD_REQUEST);
        if (this->bodyBuffer.size() + buffDist > this->maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
        if (this->bodyBuffer.size() == this->_contentLength)
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
        std::size_t dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
        std::size_t rest_dist = std::distance(content_start + dist, it_end);
        if (this->bodyBuffer.size() + dist > this->maxBodySize)
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

ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->status != ws_http::STATUS_UNDEFINED)
        return (this->status);
    if (this->headerDone)
        return (this->parseBody(it_start, it_end));
    std::copy(it_start, it_end, std::back_inserter(this->_buffer));
    buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(), HttpRequest::headEnd.begin(), HttpRequest::headEnd.end());
    if (end != this->_buffer.end())
        return (ws_http::STATUS_UNDEFINED);
    this->status = this->parseHeaders(this->_buffer.begin(), end);
    if (this->status != ws_http::STATUS_UNDEFINED)
        return (this->status);
    std::map<std::string, std::string, ws_http::CaInCmp>::iterator itContLen = this->headerMap.find("content-length");
    std::map<std::string, std::string, ws_http::CaInCmp>::iterator itTransEnc = this->headerMap.find("transfer-encoding");
    std::map<std::string, std::string, ws_http::CaInCmp>::iterator itHostHeader = this->headerMap.find("host");
    if (itHostHeader == this->headerMap.end())
        return (this->status = ws_http::STATUS_400_BAD_REQUEST);
    this->hostHeader = itHostHeader->second;
    if (itContLen != this->headerMap.end())
        this->_contentLength = strtol(itContLen->second.c_str(), NULL, 10);
    else if (itTransEnc != this->headerMap.end())
        this->_contentLength = -1;
    buff_t tmpBuffer = std::vector<char>(end + HttpRequest::headEnd.size(), this->_buffer.end());
    this->_buffer.clear();
    this->status = this->parseBody(tmpBuffer.begin(), tmpBuffer.end());
    return (this->status);
}





















// void    HttpRequest::parseHeaders(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::string         version;
//     std::stringstream   ss(std::string(it_start, it_end));
//     std::getline(ss, this->method, ' ');
//     std::getline(ss, this->fullUri, ' ');
//     std::getline(ss, version);
//     if (ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
//         this->status = ws_http::STATUS_400_BAD_REQUEST;
//     } else {
//         std::size_t queryPos = this->fullUri.find_first_of('?');
//         this->path = this->fullUri.substr(0, queryPos);
//         this->pathDecoded = getStringUrlDecoded(this->path);
//         if (queryPos != std::string::npos)
//             this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
//     }
//     for (start; start < end; start = pos + HttpRequest::crlf.size()) {
//         pos = std::search(start, end, HttpRequest::crlf.begin(), HttpRequest::crlf.end());
//         buff_t::iterator it_sep = std::find(start, pos, ':');
//         std::string token = std::string(start, it_sep);
//         if (it_sep != pos && token.find_first_of(" \"(),/:;<=>?@[\\]{}") != std::string::npos
//         || this->headerMap.find(token) != this->headerMap.end()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//             return ;
//         }
//         std::string value = std::string(it_sep + 1, pos);
//         std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
//         this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
//     }
// }


// void    HttpRequest::parseRequestLine(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::string         version;
//     std::stringstream   ss(std::string(it_start, it_end));
//     std::getline(ss, this->method, ' ');
//     std::getline(ss, this->fullUri, ' ');
//     std::getline(ss, version);
//     if (ws_http::versions.find(version) == ws_http::versions.end()
//         || ws_http::methods.find(this->method) == ws_http::methods.end()) {
//         this->status = ws_http::STATUS_400_BAD_REQUEST;
//     } else {
//         std::size_t queryPos = this->fullUri.find_first_of('?');
//         this->path = this->fullUri.substr(0, queryPos);
//         this->pathDecoded = getStringUrlDecoded(this->path);
//         if (queryPos != std::string::npos)
//             this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
//     }
// }

// void HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return ;
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(), HttpRequest::headEnd.begin(), HttpRequest::headEnd.end());
//     if (end != this->_buffer.end())
//         return ;
//     buff_t::iterator pos = std::search(this->_buffer.begin(), end, HttpRequest::crlf.begin(), HttpRequest::crlf.end());
//     this->parseRequestLine(this->_buffer.begin(), pos);
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return ;
//     buff_t::iterator start = pos + HttpRequest::crlf.size();
//     for (start; start < end; start = pos + HttpRequest::crlf.size()) {
//         pos = std::search(start, end, HttpRequest::crlf.begin(), HttpRequest::crlf.end());
//         buff_t::iterator it_sep = std::find(start, pos, ':');
//         std::string token = std::string(start, it_sep);
//         if (it_sep != pos && token.find_first_of(" \"(),/:;<=>?@[\\]{}") != std::string::npos
//         || this->headerMap.find(token) != this->headerMap.end()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//             return ;
//         }
//         std::string value = std::string(it_sep + 1, pos);
//         std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
//         this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
//     }
//     std::map<std::string, std::string>::iterator itContLen = this->headerMap.find("content-length");
//     std::map<std::string, std::string>::iterator itTransEnc = this->headerMap.find("transfer-encoding");
//     this->_contentLength = itContLen != this->headerMap.end() ? strtol(itContLen->second.c_str(), NULL, 10) 
//                             : itTransEnc != this->headerMap.end() ? -1 : 0;
//     this->_buffer.erase(this->_buffer.begin(), end + HttpRequest::headEnd.size());
// }



// buff_t::iterator    insertBodyData(buff_t::iterator it_start, buff_t::iterator it_end, buff_t& bodyBuffer)
// {
//     buff_t::iterator  itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize != it_end) {
//         long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//         buff_t::iterator contStart = itSize + ws_http::crlf.size();
//         if (std::distance(contStart, it_end) >= chunkSize + ws_http::crlf.size() + 2) {
//             std::copy(contStart, contStart + chunkSize, std::back_inserter(bodyBuffer));
//             return (contStart + chunkSize + ws_http::crlf.size());
//         }
//     }
//     return (it_end);
// }

// void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         it_start = this->_buffer.begin();
//         it_end = this->_buffer.end();
//     }
//     while (true) {
//         buff_t::iterator  itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itSize != it_end) {
//             long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//             buff_t::iterator contStart = itSize + ws_http::crlf.size();
//             if (std::distance(contStart, it_end) >= chunkSize + ws_http::crlf.size() + 2) {
//                 std::copy(contStart, contStart + chunkSize, std::back_inserter(bodyBuffer));
//                 it_start = contStart + chunkSize + ws_http::crlf.size();
//                 continue ;
//             }
//         }
//         break ;
//     }
//     if (!this->_buffer.empty()) {
//         this->_buffer.erase(this->_buffer.begin(), it_start);
//     } else {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     }
// }

// void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     buff_t::iterator nextChunk = it_start;
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         it_start = this->_buffer.begin();
//         it_end = this->_buffer.end();
//     }
//     while ((nextChunk = insertBodyData(it_start, it_end, this->bodyBuffer)) != it_start) {
//         nextChunk = insertBodyData(it_start, it_end, this->bodyBuffer);
//         if (nextChunk == it_start)
//             break ;
//         it_start = nextChunk;
//     }
// }

// void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     // buff_t::iterator itSize = this->_buffer.end();
//     // buff_t::iterator itContent = this->_buffer.end();
//     // buff_t::iterator nextChunk = it_start;
//     // while (true) {
//     //     if (!this->_buffer.empty()) {
//     //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     //         nextChunk = insertBodyData(this->_buffer.begin(), this->_buffer.end(), this->bodyBuffer);
//     //         if (nextChunk != )
//     //         this->_buffer.erase(this->_buffer.begin(), nextChunk);
//     //     } else {
//     //         nextChunk = insertBodyData(it_start, it_end, this->bodyBuffer);
//     //         if (nextChunk == )
//     //     }
//     // }

//     buff_t::iterator nextChunk = it_start;
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         it_start = this->_buffer.begin();
//         while (nextChunk != it_start) {
//             nextChunk = insertBodyData(it_start, this->_buffer.end(), this->bodyBuffer);
//             if (nextChunk != it_start) {
//                 this->_buffer.erase(it_start, nextChunk);
//             } else {
                
//             }
                
//         }
//     } else {
//         while (true) {
            
//         }
//     }
 
//     buff_t::iterator nextChunk = it_start;
//     while (true) {
//         if (!this->_buffer.empty()) {
//             std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//             nextChunk = insertBodyData(this->_buffer.begin(), this->_buffer.end(), this->bodyBuffer);
//             if (nextChunk != )
//             this->_buffer.erase(this->_buffer.begin(), nextChunk);
//         } else {
//             nextChunk = insertBodyData(it_start, it_end, this->bodyBuffer);
//             if (nextChunk == )
//         }
//     }
// }

// void    insertBodyData(buff_t::iterator it_start, buff_t::iterator it_end, buff_t& bodyBuffer, buff_t& tmpBuffer)
// {
//     buff_t::iterator  itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(tmpBuffer));
//     } else {
//         long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//         if (std::distance(itSize + ws_http::crlf.size(), itSize + (2 * ws_http::crlf.size()) + chunkSize)) {
//             std::copy(it_start, it_end, std::back_inserter(tmpBuffer));
//         } else {
//             std::copy(itSize + ws_http::crlf.size(), itSize + ws_http::crlf.size() + chunkSize, std::back_inserter(bodyBuffer));
            
            
//         }
//     }
// }

// void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     buff_t::iterator itSize = this->_buffer.end();
//     buff_t::iterator itContent = this->_buffer.end();
//     while (true) {
//         itSize = std::search(this->_buffer.begin(), this->_buffer.end(), ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itSize != this->_buffer.end())
//             itContent = std::search(itSize + ws_http::crlf.size(), this->_buffer.end(), ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itContent == this->_buffer.end()) {
//             std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//             return ;
//         }
        
//     }
// }