/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaderParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 10:21:01 by mberline          #+#    #+#             */
/*   Updated: 2023/11/09 08:07:00 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpHeaderParser.hpp"
#include <iostream>
#include <algorithm>

HttpHeaderParser::HttpHeaderParser( void )
{ }

HttpHeaderParser::~HttpHeaderParser()
{ }

std::map<ws_http::header_field_t, std::string> const &  HttpHeaderParser::getHeaderMap( void ) const
{
    return (this->_headerFields);
}
std::map<std::string, std::string, ws_http::CaInCmp> const &    HttpHeaderParser::getCustomHeaderMap( void ) const
{
    return (this->_customHeaderFields);
}

std::string const &  HttpHeaderParser::getHeader( ws_http::header_field_t headerField ) const
{
    std::map<ws_http::header_field_t, std::string>::const_iterator    pos_header;
    pos_header = this->_headerFields.find(headerField);
    if (pos_header != this->_headerFields.end())
        return ((*pos_header).second);
    return (this->_dummyEmptyHeader);
}


// std::string const & HttpHeaderParser::getHeader( std::string const & headerField ) const
// {
//     std::map<const std::string, ws_http::header_field_t, ws_http::CaInCmp>::const_iterator   pos_header_g;
//     pos_header_g = ws_http::headers.find(headerField);
//     if (pos_header_g != ws_http::headers.end()) {
//         const std::string& str = this->getHeader((*pos_header_g).second);
//         if (!str.empty())
//             return (str);
//     }
//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator   pos_header_cust;
    
//     pos_header_cust = this->_customHeaderFields.find(headerField);
//     if (pos_header_cust != this->_customHeaderFields.end())
//         return ((*pos_header_cust).second);
//     return (this->_dummyEmptyHeader);
// }

ws_http::header_field_t HttpHeaderParser::findStrHeader( std::string const & headerField ) const
{
    std::map<const std::string, ws_http::header_field_t, ws_http::CaInCmp>::const_iterator   pos_header_g;
    pos_header_g = ws_http::headers.find(headerField);
    if (pos_header_g == ws_http::headers.end())
        return (ws_http::HEADER_INVALID);
    return (pos_header_g->second);
}

std::string const & HttpHeaderParser::getHeader( std::string const & headerField ) const
{
    std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator   pos_header_cust;
    pos_header_cust = this->_customHeaderFields.find(headerField);
    if (pos_header_cust != this->_customHeaderFields.end())
        return ((*pos_header_cust).second);
    
    ws_http::header_field_t header = this->findStrHeader(headerField);
    if (header == ws_http::HEADER_INVALID)
        return (this->_dummyEmptyHeader);
    return (this->getHeader(header));
    // std::map<const std::string, ws_http::header_field_t, ws_http::CaInCmp>::const_iterator   pos_header_g;
    // pos_header_g = ws_http::headers.find(headerField);
    // if (pos_header_g != ws_http::headers.end())
    //     return (this->getHeader(pos_header_g->second));
    // return (this->_dummyEmptyHeader);
}


bool HttpHeaderParser::setHeader( ws_http::header_field_t headerField, std::string const & headerValue )
{
    if (headerField == ws_http::HEADER_INVALID)
        return (false);
    this->_headerFields[headerField] = headerValue;
    return (true);
}

bool HttpHeaderParser::setHeader( std::string const & headerField, std::string const & headerValue )
{
    ws_http::header_field_t header = this->findStrHeader(headerField);
    
    if (this->setHeader(header, headerValue))
        return (true);
    if (headerField.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos)
        return (false);
    this->_customHeaderFields[headerField] = headerValue;
    return (true);
}

bool    HttpHeaderParser::removeHeader( ws_http::header_field_t headerField )
{
    if (headerField == ws_http::HEADER_INVALID)
        return (false);
    return (this->_headerFields.erase(headerField));
}

bool    HttpHeaderParser::removeHeader( std::string const & headerField )
{
    ws_http::header_field_t header = this->findStrHeader(headerField);
    if (header != ws_http::HEADER_INVALID)
        return (this->removeHeader(header));
    
    return (this->_customHeaderFields.erase(headerField));
}

bool    HttpHeaderParser::headerIsSet( ws_http::header_field_t headerField ) const
{
    return (!this->getHeader(headerField).empty());
}

bool    HttpHeaderParser::headerIsSet( std::string headerField  ) const
{
    return (!this->getHeader(headerField).empty());
}


void    HttpHeaderParser::printHeaders( void ) const
{
    std::map<ws_http::header_field_t, std::string>::const_iterator it;
    for (it = this->_headerFields.begin(); it != this->_headerFields.end(); ++it) {
        std::cout << ws_http::headers_rev.at((*it).first) << ":\t" << (*it).second << std::endl;
    }
    std::map<std::string, std::string>::const_iterator itc;
    for (itc = this->_customHeaderFields.begin(); itc != this->_customHeaderFields.end(); ++itc) {
        std::cout << (*itc).first << ":\t" << (*itc).second << std::endl;
    }
}

void HttpHeaderParser::writeHeadersToBuffer( buff_t& buffer ) const
{
    std::map<ws_http::header_field_t, std::string>::const_iterator it_header;
    
    for (it_header = this->_headerFields.begin(); it_header != this->_headerFields.end(); ++it_header) {
        ws_http::writeHeader(ws_http::headers_rev.at(it_header->first), it_header->second, buffer);
    }
    std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator it_cheader;
    for (it_cheader = this->_customHeaderFields.begin(); it_cheader != this->_customHeaderFields.end(); ++it_cheader) {
        ws_http::writeHeader(it_cheader->first, it_cheader->second, buffer); 
    }
}

std::string wsStripWhitespace( const std::string & str )
{
    std::size_t pos1 = str.find_first_not_of(" \t");
    pos1 = pos1 == std::string::npos ? 0 : pos1;
    std::size_t pos2 = str.find_last_not_of((" \t"));
    return (str.substr(pos1, pos2 + 1 - pos1));
}



ws_http_parser_status_t HttpHeaderParser::parseHeader( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    std::map<const std::string, ws_http::header_field_t, ws_http::CaInCmp>::const_iterator   pos_header;
    buff_t::const_iterator it_sep;
    it_sep = std::find(it_start, it_end, ':');
    if (it_sep == it_start || it_sep == it_end)
        return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX);
    std::string token = std::string(it_start, it_sep);
    std::string value = wsStripWhitespace(std::string(it_sep + 1, it_end));

    if (this->headerIsSet(token))
        return (WS_HTTP_PARSE_ERROR_HEADER_DOUBLE_FIELD);
    
    if (!this->setHeader(token, value))
        return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX_FIELD);
    
    return (WS_HTTP_PARSE_PARSING);
}

ws_http_parser_status_t HttpHeaderParser::parseHeadersInBuffer(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    buff_t::const_iterator  it_pos_start = it_start;
    ws_http_parser_status_t status = WS_HTTP_PARSE_PARSING;
    while (true) {
        buff_t::const_iterator it_pos = std::search(it_pos_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        status = this->parseHeader(it_pos_start, it_pos);
        if (status != WS_HTTP_PARSE_PARSING || it_pos == it_end)
            break ;
        it_pos_start = it_pos + ws_http::crlf.size();
    }
    return (status);
}













// ws_http_parser_status_t HttpHeaderParser::parseHeader( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
// {
//     std::map<const std::string, ws_http::header_field_t, ws_http::CaInCmp>::const_iterator   pos_header;
//     buff_t::const_iterator it_sep;
//     it_sep = std::find(it_start, it_end, ':');
//     if (it_sep == it_start || it_sep == it_end)
//         return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX);
//     std::string token = std::string(it_start, it_sep);
//     std::string value = wsStripWhitespace(std::string(it_sep + 1, it_end));
//     pos_header = ws_http::headers.find(token);
//     if (pos_header != ws_http::headers.end()) {
//         if (this->_headerFields.find((*pos_header).second) != this->_headerFields.end())
//             return (WS_HTTP_PARSE_ERROR_HEADER_DOUBLE_FIELD);
//         this->_headerFields[(*pos_header).second] = value;
//     } else {
//         if (token.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos)
//             return (WS_HTTP_PARSE_ERROR_HEADER_SYNTAX_FIELD);
//         this->_customHeaderFields[token] = value;
//     }
//     return (WS_HTTP_PARSE_PARSING);
// }
