/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestLineParser.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:15:14 by mberline          #+#    #+#             */
/*   Updated: 2023/12/15 10:23:15 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_LINE_PARSER
#define HTTP_REQUEST_LINE_PARSER

#include <string>
#include "HttpHeaderDef.hpp"
#include "HttpParserStatus.hpp"

class HttpRequestLineParser {
public:
    HttpRequestLineParser( void );
    ~HttpRequestLineParser( void );

    ws_http::method_t       getMethod() const;
    ws_http::version_t      getVersion() const;
    const std::string&      getPath() const;
    const std::string&      getPathDecoded() const;
    const std::string&      getQuery() const;
    const std::string&      getFullUri() const;

    ws_http_parser_status_t parseRequestLine(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

private:
    ws_http::method_t   _method;
    ws_http::version_t  _httpVersion;
    std::string         _path;
    std::string         _pathDecoded;
    std::string         _query;
    std::string         _fullUri;
};

#endif
