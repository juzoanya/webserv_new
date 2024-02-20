/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 12:05:49 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 14:15:49 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

// #include <iostream>
// #include <string>
// #include <vector>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include "../HttpConstants.hpp"
// typedef std::vector<char> buff_t;

#include "headers.hpp"


class httpResponseLine {

};

class HttpRequestLine {

};


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


class HttpMessage {
    public:
        HttpMessage( ConfigHandler* configHandler );
        ~HttpMessage( void );
        ws_http::statuscodes_t handleDataFromSocket( buff_t::const_iterator it_start, buff_t::const_iterator itEnd );
        int setResponseBy( buff_t::const_iterator it_start, buff_t::const_iterator itEnd );
        ws_http::statuscodes_t  setResponseContent( std::istream& httpBodyStream, std::string const & mimeType );
        int writeToSocket( int socketfd );
        std::string             method;
        std::string             path;
        std::string             pathDecoded;
        std::string             query;
        std::string             fullUri;
        std::string             hostHeader;
        ws_http::statuscodes_t  status;
        HttpHeaders             requestHeaders;
        HttpHeaders             responseHeaders;
    private:
};

class HttpRequest {

};

class HttpResponse {

};

#endif