/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:26:49 by mberline          #+#    #+#             */
/*   Updated: 2024/02/07 20:35:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP


#include "headers.hpp"

typedef std::map<std::string, std::string, ws_http::CaInCmp>    http_header_map_t;

class HttpHeaders {
    public:
    class HttpHeaderField {
        public:
            HttpHeaderField( void );
            HttpHeaderField( std::string const & token, std::string const & value );
            HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue );
            ~HttpHeaderField( void );
            bool operator==( HttpHeaderField const & headerField ) const;
            bool operator==( std::string const & key ) const;
            bool operator!=( HttpHeaderField const & headerField ) const;
            bool operator!=( std::string const & key ) const;
            std::string const & getField( void ) const;
            std::string const & getValue( void ) const;
        private:
            std::string _token;
            std::string _value;
    };
        HttpHeaders( void );
        ~HttpHeaders( void );
        bool    addHeader( std::string const & line );
        bool    addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );
        bool    addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );
        std::string const & getHeaderValue( std::string const & key ) const;
        HttpHeaderField const & getHeader( std::string const & key ) const;
        std::vector<HttpHeaderField> const & getHeadersVec( void ) const;
    private:
        static HttpHeaderField         dummyHeader;
        std::vector<HttpHeaderField>   _headers;
};

std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs );
std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs );


class HttpRequest;

class HttpRequestBody {
    public:
        HttpRequestBody( void );
        HttpRequestBody( HttpRequest const & request, long maxRequestBodySize );
        ~HttpRequestBody( void );

        ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        buff_t  bodyBuffer;
        long    maxBodySize;

    private:
        buff_t::const_iterator  setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        buff_t  _buffer;
        long    _contentLength;
        long    _chunkSize;
        int     _splitCase;
};

struct OnHttpRequestHeaderDoneFunctor {
    OnHttpRequestHeaderDoneFunctor( void ) {}
    virtual ~OnHttpRequestHeaderDoneFunctor( void ) {};
    virtual long operator()( std::string const & pathDecoded, std::string const & hostHeader ) = 0;
};

class HttpParser {
    public:
        HttpParser( void );
        ~HttpParser( void );
        HttpHeaders headers;
    private:
};

class HttpRequest {
    public:
        // HttpRequest( void );
        HttpRequest( OnHttpRequestHeaderDoneFunctor* headerDoneFunc, bool parseReqLine );
        ~HttpRequest( void );

        ws_http::statuscodes_t    parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        void    printRequest( void ) const;

        http_header_map_t       headerMap;
        std::string             method;
        std::string             path;
        std::string             pathDecoded;
        std::string             query;
        std::string             fullUri;
        std::string             hostHeader;
        ws_http::statuscodes_t  status;
        HttpRequestBody         requestBody;
        buff_t::const_iterator  endPos;

    private:
        ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        buff_t          _buffer;
        OnHttpRequestHeaderDoneFunctor*  _headerDoneFunc;
        bool            _headerDone;
        bool            _parseRequestLine;
};

bool    addHeader(std::string const & line, http_header_map_t& headers);












// class HttpRequest {
//     public:
//         HttpRequest( void );
//         ~HttpRequest( void );

//         buff_t::const_iterator    parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

//         std::map<std::string, std::string, ws_http::CaInCmp>  headerMap;
//         std::string             method;
//         std::string             path;
//         std::string             pathDecoded;
//         std::string             query;
//         std::string             fullUri;
//         std::string             hostHeader;
//         ws_http::statuscodes_t  status;

//     private:
//         ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

//         buff_t  _buffer;
// };


#endif