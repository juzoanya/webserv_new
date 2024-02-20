/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpIO.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 14:10:26 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 10:00:41 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_IO_HPP
#define HTTP_IO_HPP

#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include "../HttpConstants.hpp"

typedef std::vector<char>   buff_t;

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


class HttpIO {
    public:
        HttpIO( void );
        ~HttpIO( void );
        int readFromSocket( void );
        int writeToSocket( void );
        void    getNextHttpLine( std::string & str );
    private:
        void    setReadPosition(char* start, char* current, char* end);
        static std::vector<char>::size_type defCapa;
        char*   _pStart;
        char*   _pCurr;
        char*   _pEnd;
        int                 _socketfd;
        std::vector<char>   _readBuffer;
        std::vector<char>   _writeBuffer;
        std::vector<char>&  _usedWriteBuffer;
        bool                _useExternalWriteBuffer;
};

#endif