/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLineParser.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:15:28 by mberline          #+#    #+#             */
/*   Updated: 2023/11/15 16:21:34 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestLineParser.hpp"

HttpRequestLineParser::HttpRequestLineParser( void ) : _method(ws_http::METHOD_INVALID), _httpVersion(ws_http::VERSION_INVALID)
{ }

HttpRequestLineParser::~HttpRequestLineParser( void )
{ }

ws_http::method_t   HttpRequestLineParser::getMethod( void )  const
{ return (this->_method); }

ws_http::version_t  HttpRequestLineParser::getVersion( void ) const
{ return (this->_httpVersion); }

const std::string&  HttpRequestLineParser::getPath( void )    const
{ return (this->_path); }

const std::string&  HttpRequestLineParser::getPathDecoded( void )    const
{
    return (this->_pathDecoded);
}

const std::string&  HttpRequestLineParser::getQuery( void )   const
{ return (this->_query); }

const std::string&  HttpRequestLineParser::getFullUri( void )   const
{ return (this->_fullUri); }

std::string    parseUrlEncode(std::string const & strUrlEncoded)
{
    const char *str = strUrlEncoded.c_str();
    std::string res;
    res.resize(std::strlen(str));

    int k = 0;
    for (int pos = 0; str[pos]; pos++)
    {
        if (str[pos] == '%' && str[pos + 1] && str[pos + 2]
        && (isdigit(str[pos + 1]) || isupper(str[pos + 1]))
        && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
            
            char first = str[pos + 1];
            char sec = str[pos + 2];

            int a = 16 * (first < 'A' ? first - 48 : first - 55);
            int b =      (sec   < 'A' ? sec   - 48 : sec   - 55);
            int x = a + b; 
            res[k++] = x;
            pos += 2;
        } else {
            if (str[pos])
                res[k++] = str[pos];
        }
    }
    res.shrink_to_fit();
    return (res);
}

ws_http_parser_status_t HttpRequestLineParser::parseRequestLine( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    std::map<const std::string, ws_http::method_t>::const_iterator   pos_method;
    std::map<const std::string, ws_http::version_t>::const_iterator   pos_version;
    buff_t::const_iterator it_space;
    
    it_space = std::find(it_start, it_end, ' ');
    if (it_space == it_end || it_space == it_start)
        return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX);

    pos_method = ws_http::methods.find(std::string(it_start, it_space));
    if (pos_method == ws_http::methods.end())
        return (WS_HTTP_PARSE_ERROR_REQUEST_LINE_METHOD);
    this->_method = (*pos_method).second;

    it_start = it_space + 1;
    it_space = std::find(it_start, it_end, ' ');
    if (it_space == it_end || it_space == it_start)
        return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX);
    this->_fullUri = std::string(it_start, it_space);
    std::size_t queryPos = this->_fullUri.find_first_of('?');
    this->_path = this->_fullUri.substr(0, queryPos);
    this->_pathDecoded = parseUrlEncode(this->_path);
    if (queryPos != std::string::npos)
        this->_query = this->_fullUri.substr(queryPos + 1, std::string::npos);

    it_start = it_space + 1;
    pos_version = ws_http::versions.find(std::string(it_start, it_end));
    if (pos_version == ws_http::versions.end())
        return (WS_HTTP_PARSE_ERROR_REQUEST_LINE_VERSION);
    this->_httpVersion = (*pos_version).second;

    return (WS_HTTP_PARSE_PARSING);
}

