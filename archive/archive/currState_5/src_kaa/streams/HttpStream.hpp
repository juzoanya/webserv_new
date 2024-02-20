/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStream.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 11:38:47 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 11:32:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STREAM_HPP
#define HTTP_STREAM_HPP

#include <streambuf>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>

#include <iostream>
#include <streambuf>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <algorithm>

class SocketStreambuf : public std::streambuf {
public:
    SocketStreambuf( int socketFd );
    ~SocketStreambuf( void );
    void    setReadable( void );
    void    setWriteable( void );

protected:
    // put to buffer - virtual inherited from streambuf
    virtual std::streamsize xsputn( const std::streambuf::char_type* s, std::streamsize n );
    virtual std::streambuf::int_type overflow( std::streambuf::int_type ch );
    // put to buffer - owm impl
    int readSock( void );


    // get area
    virtual std::streamsize showmanyc( void );
    virtual std::streambuf::int_type underflow( void );
    // virtual std::streambuf::int_type uflow( void );
    virtual std::streamsize xsgetn( std::streambuf::char_type* s, std::streamsize n );

private:
    int     _socketFd;
    bool    _isReadable;
    bool    _isWriteable;
    int     _lastReadByte;
    int     _usedCapa;
    static std::vector<char>::size_type defCapa;
    std::vector<char>   obuffer;
    std::vector<char>   ibuffer;
};

class SocketStream : public std::iostream {
public:
    SocketStream( int socketFd );
    ~SocketStream( void );

    void    readFromSocket( void );
    void    sendToSocket( void );
    void    setReadable( void );
    void    setWriteable( void );
    void    readHttpLine( std::string & str );
    std::ostream & flush( void );
private:
    SocketStreambuf sb;
};


#endif