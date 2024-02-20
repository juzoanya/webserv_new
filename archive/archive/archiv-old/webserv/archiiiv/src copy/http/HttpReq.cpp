/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2023/12/24 09:21:28 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

bool    CaInCmp::operator()( const std::string& a, const std::string& b ) const
{
    std::string::const_iterator it_a = a.begin();
    std::string::const_iterator it_b = b.begin();
    for (; (it_a != a.end()) && (it_b != b.end()); ++it_a, (void) ++it_b)
    {
        unsigned char ca = std::tolower(*it_a);
        unsigned char cb = std::tolower(*it_b);
        if (ca < cb)
            return (true);
        if (cb < ca)
            return (false);
    }
    return (it_a == a.end()) && (it_b != b.end());
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
    return (result);
}

HttpMsg::HttpMsg( void )
{ }

HttpMsg::~HttpMsg( void )
{ }

int HttpMsg::parseRequestLine(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    std::string         version;
    std::stringstream   ss(std::string(it_start, it_end));
    std::getline(ss, this->_method, ' ');
    std::getline(ss, this->_fullUri, ' ');
    std::getline(ss, version);
    if ((this->_method == "GET" || this->_method == "POST" || this->_method == "DELETE") && version == "HTTP/1.1") {
        std::size_t queryPos = this->_fullUri.find_first_of('?');
        this->_path = this->_fullUri.substr(0, queryPos);
        this->_pathDecoded.resize(this->_fullUri.size());
        for (std::size_t pos = 0, k = 0; pos < this->_fullUri.size(); pos++, k++) {
            if (this->_fullUri[pos] == '%' && this->_fullUri[pos + 1] && this->_fullUri[pos + 2] && (isdigit(this->_fullUri[pos + 1])
                || isupper(this->_fullUri[pos + 1])) && (isdigit(this->_fullUri[pos + 2]) || isupper(this->_fullUri[pos + 2]))) {
                int a = 16 * (this->_fullUri[pos + 1] < 'A' ? this->_fullUri[pos + 1] - 48 : this->_fullUri[pos + 1] - 55);
                int b =      (this->_fullUri[pos + 2] < 'A' ? this->_fullUri[pos + 2] - 48 : this->_fullUri[pos + 2] - 55);
                this->_pathDecoded[k] = a + b;
                pos += 2;
            } else if (this->_fullUri[pos]) {
                this->_pathDecoded[k] = this->_fullUri[pos];
            }
        }
        if (queryPos != std::string::npos)
            this->_query = this->_fullUri.substr(queryPos + 1, std::string::npos);
        return (1);
    }
    return (-1);
}

int HttpMsg::parseReq(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    std::copy(it_start, it_end, std::back_inserter(this->_headerBuffer));
    buff_t::iterator itEndPos = std::search(this->_headerBuffer.begin(), this->_headerBuffer.end(),
                                    ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
    if (itEndPos == this->_headerBuffer.end())
        return (1);
    buff_t::iterator it_pos = std::search(this->_headerBuffer.begin(), itEndPos, ws_http::crlf.begin(), ws_http::crlf.end());
    this->parseRequestLine(this->_headerBuffer.begin(), it_pos);
    buff_t::iterator  it_pos_start = it_pos + ws_http::crlf.size();;
    while (it_pos_start < itEndPos) {
        it_pos = std::search(it_pos_start, itEndPos, ws_http::crlf.begin(), ws_http::crlf.end());
        buff_t::iterator it_sep = std::find(it_pos_start, it_pos, ':');
        std::string token = std::string(it_pos_start, it_sep);
        if (it_sep == it_pos_start || it_sep == it_pos || token.find_first_of(" \"(),/:;<=>?@[\\]{}") != std::string::npos || this->headerMap.find(token) != this->headerMap.end())
            return (-1); 
        std::string value = std::string(it_sep + 1, it_pos);
        std::size_t pos1 = value.find_first_not_of(" \t") == std::string::npos ? 0 : value.find_first_not_of(" \t");
        this->headerMap[token] = value.substr(pos1, value.find_last_not_of((" \t")) + 1 - pos1);
        it_pos_start = it_pos + ws_http::crlf.size();
    }
}

int main()
{
    std::string reli = "DELETE /fwen/few/rwfw.html?fwfw HTTP/1.1";
    std::vector<char> loo(reli.begin(), reli.end());
    std::string reqline(loo.begin(), loo.end()), method, path, version, rest;
    std::stringstream   ss(reqline);
    
    std::getline(ss, method, ' ');
    std::getline(ss, path, ' ');
    std::getline(ss, version, ' ');
    std::getline(ss, rest, ' ');

    if (!(method == "GET" || method == "POST" || method == "DELETE")) {
        std::cout << "jooo is not" << std::endl;
    }

    std::cout << "method: " << method << std::endl;
    std::cout << "path: " << path << std::endl;
    std::cout << "version: " << version << std::endl;
    std::cout << "rest: " << rest << std::endl;
    return (0);
}