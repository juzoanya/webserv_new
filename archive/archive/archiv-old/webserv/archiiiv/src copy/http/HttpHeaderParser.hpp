/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaderParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 10:19:50 by mberline          #+#    #+#             */
/*   Updated: 2023/12/15 10:23:47 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HEADER_PARSER_HPP
#define HTTP_HEADER_PARSER_HPP

#include <map>
#include <string>
#include <utility>
#include "HttpHeaderDef.hpp"
#include "HttpParserStatus.hpp"

class HttpHeaderParser {
public:
    HttpHeaderParser( void );
    ~HttpHeaderParser( void );

    bool setHeader(ws_http::header_field_t headerField, std::string const & headerValue);
    bool setHeader(std::string const & headerField, std::string const & headerValue);
    bool removeHeader(ws_http::header_field_t headerField);
    bool removeHeader(std::string const & headerField);
    std::string const & getHeader(ws_http::header_field_t headerField) const;
    std::string const & getHeader(std::string const & headerField) const;
    bool headerIsSet(ws_http::header_field_t headerField) const;
    bool headerIsSet(std::string headerField) const;
    void writeHeadersToBuffer( buff_t& buffer ) const;

    
    std::map<ws_http::header_field_t, std::string> const &          getHeaderMap( void ) const;
    std::map<std::string, std::string, ws_http::CaInCmp> const &    getCustomHeaderMap( void ) const;

    void    printHeaders( void ) const;
    ws_http_parser_status_t parseHeader(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    ws_http_parser_status_t parseHeadersInBuffer(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

private:
    ws_http::header_field_t findStrHeader( std::string const & headerField ) const;
    std::map<ws_http::header_field_t, std::string>          _headerFields;
    std::map<std::string, std::string, ws_http::CaInCmp>    _customHeaderFields;
    std::string                                             _dummyEmptyHeader;
};

// class HttpHeaderParserMultiline : public HttpHeaderParser {

// };

#endif
