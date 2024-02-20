/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/01/03 11:25:46 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

// bool    CaInCmp::operator()( const std::string& a, const std::string& b ) const
// {
//     std::string::const_iterator it_a = a.begin();
//     std::string::const_iterator it_b = b.begin();
//     for (; (it_a != a.end()) && (it_b != b.end()); ++it_a, (void) ++it_b)
//     {
//         unsigned char ca = std::tolower(*it_a);
//         unsigned char cb = std::tolower(*it_b);
//         if (ca < cb)
//             return (true);
//         if (cb < ca)
//             return (false);
//     }
//     return (it_a == a.end()) && (it_b != b.end());
// }


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

const std::string HttpMsg::crlf = "\r\n";
const std::string HttpMsg::headEnd = "\r\n\r\n";

HttpMsg::HttpMsg( void )
 : _status(ws_http::STATUS_INVALID)
{ }

HttpMsg::~HttpMsg( void )
{ }

void HttpMsg::parseRequestLine(buff_t::iterator it_start, buff_t::iterator it_end)
{
    std::string         version;
    std::stringstream   ss(std::string(it_start, it_end));
    std::getline(ss, this->_method, ' ');
    std::getline(ss, this->_fullUri, ' ');
    std::getline(ss, version);
    if (version != "HTTP/1.1") {
        this->_status = ws_http::STATUS_505_HTTP_VERSION_NOT_SUPPORTED;
    } else if (ws_http::methods.find(this->_method) == ws_http::methods.end()) {
        this->_status = ws_http::STATUS_400_BAD_REQUEST;
    } else {
        std::size_t queryPos = this->_fullUri.find_first_of('?');
        this->_path = this->_fullUri.substr(0, queryPos);
        this->_pathDecoded = getStringUrlDecoded(this->_path);
        if (queryPos != std::string::npos)
            this->_query = this->_fullUri.substr(queryPos + 1, std::string::npos);
    }
}

void HttpMsg::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_status != ws_http::STATUS_INVALID)
        return ;
    std::copy(it_start, it_end, std::back_inserter(this->_buffer));
    buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(), HttpMsg::headEnd.begin(), HttpMsg::headEnd.end());
    if (end != this->_buffer.end())
        return ;
    buff_t::iterator pos = std::search(this->_buffer.begin(), end, HttpMsg::crlf.begin(), HttpMsg::crlf.end());
    this->parseRequestLine(this->_buffer.begin(), pos);
    if (this->_status != ws_http::STATUS_INVALID)
        return ;
    buff_t::iterator start = pos + HttpMsg::crlf.size();
    for (start; start < end; start = pos + HttpMsg::crlf.size()) {
        pos = std::search(start, end, HttpMsg::crlf.begin(), HttpMsg::crlf.end());
        buff_t::iterator it_sep = std::find(start, pos, ':');
        std::string token = std::string(start, it_sep);
        if (it_sep != pos && token.find_first_of(" \"(),/:;<=>?@[\\]{}") != std::string::npos
        || this->headerMap.find(token) != this->headerMap.end()) {
            this->_status = ws_http::STATUS_400_BAD_REQUEST;
            return ;
        }
        std::string value = std::string(it_sep + 1, pos);
        std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
        this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
    }
    this->_buffer.erase(this->_buffer.begin(), end + HttpMsg::headEnd.size());
    this->_status = ws_http::STATUS_200_OK;
}


// void HttpMsg::parseHeader(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     buff_t::iterator it_sep = std::find(it_start, it_end, ':');
//     std::string token = std::string(it_start, it_sep);
//     if (it_sep != it_end && token.find_first_of(" \"(),/:;<=>?@[\\]{}") == std::string::npos
//     && this->headerMap.find(token) == this->headerMap.end()) {
//         std::string value = std::string(it_sep + 1, it_end);
//         std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
//         this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
//     } else {
//         this->_status = ws_http::STATUS_400_BAD_REQUEST;
//     }
// }

// void HttpMsg::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_status != ws_http::STATUS_INVALID)
//         return ;
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(), HttpMsg::headEnd.begin(), HttpMsg::headEnd.end());
//     if (end != this->_buffer.end())
//         return ;
//     buff_t::iterator pos = std::search(this->_buffer.begin(), end, HttpMsg::crlf.begin(), HttpMsg::crlf.end());
//     this->parseRequestLine(this->_buffer.begin(), pos);
//     if (this->_status != ws_http::STATUS_INVALID)
//         return ;
//     buff_t::iterator start = pos + HttpMsg::crlf.size();
//     for (start; start < end; start = pos + HttpMsg::crlf.size()) {
//         pos = std::search(start, end, HttpMsg::crlf.begin(), HttpMsg::crlf.end());
//         this->parseHeader(start, pos);
//         if (this->_status != ws_http::STATUS_INVALID)
//             return ;
//     }
//     this->_buffer.erase(this->_buffer.begin(), end + HttpMsg::headEnd.size());
//     this->_status = ws_http::STATUS_200_OK;
// }



// int HttpMsg::parseReq(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::iterator itEndPos = std::search(this->_buffer.begin(), this->_buffer.end(),
//                                     ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//     if (itEndPos == this->_buffer.end())
//         return (1);
//     buff_t::iterator it_pos = std::search(this->_buffer.begin(), itEndPos, ws_http::crlf.begin(), ws_http::crlf.end());
//     this->parseRequestLine(this->_buffer.begin(), it_pos);
//     buff_t::iterator  it_pos_start = it_pos + ws_http::crlf.size();;
//     while (it_pos_start < itEndPos) {
//         it_pos = std::search(it_pos_start, itEndPos, ws_http::crlf.begin(), ws_http::crlf.end());
//         buff_t::iterator it_sep = std::find(it_pos_start, it_pos, ':');
//         std::string token = std::string(it_pos_start, it_sep);
//         if (it_sep == it_pos_start || it_sep == it_pos || token.find_first_of(" \"(),/:;<=>?@[\\]{}") != std::string::npos || this->headerMap.find(token) != this->headerMap.end())
//             return (-1); 
//         std::string value = std::string(it_sep + 1, it_pos);
//         std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
//         this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
//         it_pos_start = it_pos + ws_http::crlf.size();
//     }
// }
