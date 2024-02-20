/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:26:49 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 17:08:46 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "headers.hpp"

typedef std::map<std::string, std::string, ws_http::CaInCmp>    http_header_map_t;

class HttpRequest {
    public:
        typedef std::vector< std::pair<std::string, std::string> >    headers_t;

        HttpRequest( HttpServer* server, bool parseReqLine );
        ~HttpRequest( void );

        static std::string  method;
        static std::string  path;
        static std::string  pathDecoded;
        static std::string  query;
        static std::string  fullUri;
        static std::string  version;

        ws_http::statuscodes_t  parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        std::string const &     getHeader( std::string const & key ) const;
        bool                    hasHeader( std::string const & key ) const;
        buff_t const &          getBody( void ) const;
        headers_t const &       getHeaderVector( void ) const;
        ws_http::statuscodes_t  getStatus( void ) const;
        void                    printRequest( void ) const;

        ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    private:
        buff_t::const_iterator  setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        bool                    addHeader(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        ws_http::statuscodes_t  status;
        headers_t   _headerMap;
        buff_t      _buffer;
        buff_t      _bodyBuffer;
        HttpServer* _server;
        long        _contentLength;
        long        _maxBodySize;
        long        _chunkSize;
        bool        _headerDone;
        bool        _parseRequestLine;
        bool        _reqLineDone;
        bool        _crFound;
        bool        _parseSize;
        bool        _bodyDone;
};


#endif