/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestNew.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 19:48:25 by mberline          #+#    #+#             */
/*   Updated: 2024/02/10 11:25:05 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_NEW_HPP
#define HTTP_REQUEST_NEW_HPP

#include "headers.hpp"

class HttpRequest;

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
                int         _pos;
                std::string _header;
        };
        HttpHeaders( void );
        ~HttpHeaders( void );

        bool    addHeader( std::string const & line );
        bool    addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );
        bool    addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );

        static HttpHeaderField  dummyHeader;
        static std::string      reqline_method;
        static std::string      reqline_path;
        static std::string      reqline_pathDecoded;
        static std::string      reqline_query;
        static std::string      reqline_fullUri;

        std::string const & getHeaderValue( std::string const & key ) const;
        HttpHeaderField const & getHeader( std::string const & key ) const;
        std::vector<HttpHeaderField> const & getHeadersVec( void ) const;
    private:
        std::vector<HttpHeaderField>   _headers;
        std::vector<std::string>   _headersNew;
        bool    _parseReqLine;
        bool    _reqLineDone;
        bool    _done;
};

std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs );
std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs );


class HttpRequestNew {
   public:
        HttpRequestNew( HttpServer* server, bool parseReqLine );
        ~HttpRequestNew( void );


        ws_http::statuscodes_t  parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        void    printRequest( void ) const;

        std::string             method;
        std::string             path;
        std::string             pathDecoded;
        std::string             query;
        std::string             fullUri;
        ws_http::statuscodes_t  status;
        buff_t::const_iterator  endPos;

        HttpHeaders     headers;
    private:
        // int readFromSocketAndParseHttp(int socketfd );
        ws_http::statuscodes_t  parseBodyByContentLength(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseBodyByTransferEncodingChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        buff_t::const_iterator  setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        buff_t          _buffer;
        HttpServer*     _server;
        long            _maxBodySize;
        int             _contentLength;
        int             _chunkSize;
        short           _splitCase;
        bool            _headerDone;
        bool            _parseRequestLine;
};

// class HttpRequest;

// class HttpRequestLine {
//     public:
//         HttpRequestLine( void );
//         ~HttpRequestLine( void );
//     private:
// };

// class HttpHeaders {
//     public:
//     class HttpHeaderField {
//         public:
//             HttpHeaderField( void );
//             HttpHeaderField( std::string const & token, std::string const & value );
//             HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue );
//             ~HttpHeaderField( void );
//             bool operator==( HttpHeaderField const & headerField ) const;
//             bool operator==( std::string const & key ) const;
//             bool operator!=( HttpHeaderField const & headerField ) const;
//             bool operator!=( std::string const & key ) const;
//             std::string const & getField( void ) const;
//             std::string const & getValue( void ) const;
//         private:
//             std::string _token;
//             std::string _value;
//     };
//         HttpHeaders( void );
//         ~HttpHeaders( void );

//         bool    addHeader( std::string const & line );
//         bool    addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );
//         bool    addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd );

//         static HttpHeaderField  dummyHeader;
//         static std::string      reqline_method;
//         static std::string      reqline_path;
//         static std::string      reqline_pathDecoded;
//         static std::string      reqline_query;
//         static std::string      reqline_fullUri;

//         std::string const & getHeaderValue( std::string const & key ) const;
//         HttpHeaderField const & getHeader( std::string const & key ) const;
//         std::vector<HttpHeaderField> const & getHeadersVec( void ) const;
//     private:
//         std::vector<HttpHeaderField>   _headers;
//         std::vector<std::string>   _headersNew;
//         bool    _parseReqLine;
//         bool    _reqLineDone;
//         bool    _done;
// };

// std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs );
// std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs );


// class HttpRequestNew {
//    public:
//         HttpRequestNew( HttpServer* server, bool parseReqLine );
//         ~HttpRequestNew( void );


//         ws_http::statuscodes_t  parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         void    printRequest( void ) const;

//         std::string             method;
//         std::string             path;
//         std::string             pathDecoded;
//         std::string             query;
//         std::string             fullUri;
//         ws_http::statuscodes_t  status;
//         buff_t::const_iterator  endPos;

//         HttpHeaders     headers;
//     private:
//         // int readFromSocketAndParseHttp(int socketfd );
//         ws_http::statuscodes_t  parseBodyByContentLength(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         ws_http::statuscodes_t  parseBodyByTransferEncodingChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         buff_t::const_iterator  setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         buff_t          _buffer;
//         HttpServer*     _server;
//         long            _maxBodySize;
//         int             _contentLength;
//         int             _chunkSize;
//         short           _splitCase;
//         bool            _headerDone;
//         bool            _parseRequestLine;
// };

#endif