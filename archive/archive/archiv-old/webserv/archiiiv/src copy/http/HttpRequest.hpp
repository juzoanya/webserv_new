/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:26:48 by mberline          #+#    #+#             */
/*   Updated: 2023/12/18 15:34:09 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "HttpRequestLineParser.hpp"
#include "HttpHeaderParser.hpp"
#include "HttpBodyParser.hpp"
#include "HttpHeaderDef.hpp"
#include "HttpCgiDef.hpp"

#define WS_HTTP_DEFAULT_MAX_HEADER_SIZE 4096

// struct CgiHandler : public ProcessEventHandler {
//     CgiHandler( void );
//     CgiHandler(HttpResponse& response);
//     void operator()(ChildProcessStatus status);
//     buff_t          _cgiData;
//     HttpResponse*   _response;
// };

class HttpRequest {
public:
    // HttpRequest( void );
    HttpRequest(std::size_t maxHeaderSize = WS_HTTP_DEFAULT_MAX_HEADER_SIZE, std::size_t maxBodySize = WS_HTTP_DEFAULT_MAX_BODY_SIZE );
    ~HttpRequest( void );

    ws_http_parser_status_t getStatus( void ) const;
    buff_t const &          getBody( void ) const;
    HttpHeaderParser const & getHeaders( void ) const;
    HttpRequestLineParser const & getRequestLine( void ) const;
    void                    printRequest( void ) const;
    ws_http_parser_status_t parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    // char* const*            getCgiEnviroment( std::string const & docRoot, std::string const & serverName );
    // int                     spawnCgi( stdw::string const & cgiBinPath, char** args, std::string const & rootPath, std::string const & serverName, HttpResponse& response );
private:
    bool parseBuffer( buff_t::const_iterator it_start, buff_t::const_iterator it_end );

    HttpHeaderParser        _header;
    HttpRequestLineParser   _requestLine;
    ws_http_parser_status_t _status;
    buff_t                  _headerBuffer;
    std::size_t             _debHeaderSize;
    buff_t::iterator        _prevHeaderPos;
    long                    _remainCapacity;
    
    ws_cgi::Cgi             _cgi;
    HttpBody                _body;
    std::size_t             _maxBodySize;
    bool                    _headerDone;
    // CgiHandler              _cgiHandler;
};

#endif
