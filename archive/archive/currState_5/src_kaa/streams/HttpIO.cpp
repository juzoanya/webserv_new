/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpIO.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 14:10:24 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 10:00:36 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpIO.hpp"


HttpHeaders::HttpHeaderField HttpHeaders::dummyHeader;

HttpHeaders::HttpHeaderField::HttpHeaderField( void )
{ }

HttpHeaders::HttpHeaderField::HttpHeaderField( std::string const & token, std::string const & value )
 : _token(token), _value(value)
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
}

HttpHeaders::HttpHeaderField::HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue )
 : _token(itStartKey, itEndKey), _value(itStartValue, itEndValue)
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
}

HttpHeaders::HttpHeaderField::~HttpHeaderField( void )
{ }

bool HttpHeaders::HttpHeaderField::operator==( HttpHeaderField const & headerField ) const { return (headerField._token == this->_token); }
bool HttpHeaders::HttpHeaderField::operator==( std::string const & key ) const { return (key == this->_token); }
bool HttpHeaders::HttpHeaderField::operator!=( HttpHeaderField const & headerField ) const { return (headerField._token != this->_token); }
bool HttpHeaders::HttpHeaderField::operator!=( std::string const & key ) const { return (key != this->_token); }
std::string const & HttpHeaders::HttpHeaderField::getField( void ) const { return (this->_token); }
std::string const & HttpHeaders::HttpHeaderField::getValue( void ) const { return (this->_value); }

HttpHeaders::HttpHeaders( void )
{ }

HttpHeaders::~HttpHeaders( void )
{ }

bool    HttpHeaders::addHeader( std::string const & line ) { return (this->addHeader(line.begin(), line.end())); }

bool    HttpHeaders::addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    const buff_t::const_iterator itStartKey = itStart;
    const buff_t::const_iterator itEndKey = std::find(itStart, itEnd, ':');
    if (itEndKey == itEnd)
        return (false);
    for (; itStart != itEndKey; ++itStart) {
        if (!std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
            return (false);
    }
    itStart++;
    while (*itStart == ' ' || *itStart == '\t')
        itStart++;
    while (*itEnd == ' ' || *itEnd == '\t')
        itEnd--;
    for (buff_t::const_iterator it = itStart; it != itEnd; ++it) {
        unsigned char c = *it;
        if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
            continue ;
        return (false);
    }
    HttpHeaderField headerField(itStartKey, itEndKey, itStart, itEnd);
    if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
        return (false);
    this->_headers.push_back(headerField);
    return (true);
}

bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    while (itStart < itEnd) {
        const buff_t::const_iterator itStartKey = itStart;
        while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
            itStart++;
        if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
            return (false);
        const buff_t::const_iterator itEndKey = itStart;
        itStart++;
        while (*itStart == ' ' || *itStart == '\t')
            itStart++;
        const buff_t::const_iterator itStartValue = itStart;
        buff_t::const_iterator lastNonWhitespace = itStart;
        bool crFound = false;
        for (;itStart != itEnd; ++itStart) {
            unsigned char c = *itStart;
            if (c == '\n' && crFound) {
                HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
                if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
                    return (false);
                this->_headers.push_back(headerField);
                itStart++;
                break ;
            }
            if (c == '\r' && !crFound) {
                crFound = true;
            } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
                lastNonWhitespace = itStart;
                crFound = false;
            } else if ((c == ' ' || c == '\t') && !crFound) {
                crFound = false;
            } else {
                return (false);
            }
        }
    }
    return (true);
}

std::string const & HttpHeaders::getHeaderValue( std::string const & key ) const { return (this->getHeader(key).getValue()); }

HttpHeaders::HttpHeaderField const & HttpHeaders::getHeader( std::string const & key ) const
{
    std::vector<HttpHeaderField>::const_iterator it = std::find(this->_headers.begin(), this->_headers.end(), key);
    if (it != this->_headers.end())
        return (*it);
    return (HttpHeaders::dummyHeader);
}

std::vector<HttpHeaders::HttpHeaderField> const & HttpHeaders::getHeadersVec( void ) const { return (this->_headers); }

std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs )
{
    if (!rhs.getField().empty())
        os << rhs.getField() << ": " << rhs.getValue() << "$" << "\r\n";
    return (os);
}

std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs )
{
    std::vector<HttpHeaders::HttpHeaderField> const & headers = rhs.getHeadersVec();
    for (std::vector<HttpHeaders::HttpHeaderField>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        os << *it;
    return (os);
}

HttpIO::HttpIO( void ) : _usedWriteBuffer(_writeBuffer), _useExternalWriteBuffer(false)
{ }

HttpIO::~HttpIO( void )
{ }

std::vector<char>::size_type HttpIO::defCapa = 4096;

void    HttpIO::setReadPosition(char* start, char* current, char* end)
{
    this->_pStart = start;
    this->_pCurr = current;
    this->_pEnd = end;
}

int HttpIO::readFromSocket( void )
{
    if (this->_readBuffer.capacity() != defCapa);
        this->_readBuffer.reserve(defCapa);
    int readByte = recv(this->_socketfd, this->_readBuffer.data(), defCapa, 0);
    this->setReadPosition(this->_readBuffer.data(), this->_readBuffer.data(), this->_readBuffer.data() + readByte);
    return (readByte);
}

int HttpIO::writeToSocket( void )
{
    if (this->_useExternalWriteBuffer) {
        int sendByte = send(this->_socketfd, NULL, 0, 0);
    } else {
        if (this->_writeBuffer.capacity() != defCapa);
            this->_writeBuffer.reserve(defCapa);
    }
}

void    HttpIO::getNextHttpLine( std::string & str )
{
    bool crFound;
    
    while (true) {
        // for (;itStart != itEnd; ++itStart) {
        //     unsigned char c = *itStart;
        //     if (c == '\n' && crFound) {
        //         HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
        //         if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
        //             return (false);
        //         this->_headers.push_back(headerField);
        //         itStart++;
        //         break ;
        //     }
        //     if (c == '\r' && !crFound) {
        //         crFound = true;
        //     } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
        //         lastNonWhitespace = itStart;
        //         crFound = false;
        //     } else if ((c == ' ' || c == '\t') && !crFound) {
        //         crFound = false;
        //     } else {
        //         return (false);
        //     }
        // }
    }
}


int main()
{
    HttpHeaders headers;
    std::cout << "size: " << sizeof(headers) << std::endl;
    // headers.addHeader("Statfus: fjigfwrgw");
    // headers.addHeader("Stafdtus: fjigfwrgw");
    // headers.addHeader("Stfsdsatus: fjigfwrgw");
    std::string headerbuf = "eins:  fjigfwrgw    \r\nzwo:fwfwe\r\ndrei:fwfwe\r\nvier:fwfwe\r\nvierf:fwfwe";
    headers.addHeaderBuf(headerbuf.begin(), headerbuf.end());
    std::cout << "size: " << sizeof(headers) << std::endl;
    std::cout << headers;
    // std::cout << headers.getHeaderValue("status") << std::endl;
    // std::cout << headers.getHeader("status") << std::endl;
    return (0);
}

