/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 07:56:42 by mberline          #+#    #+#             */
/*   Updated: 2024/02/11 19:30:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

// HttpResponse::HttpResponse( void )
//  : headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
// { }

// HttpResponse::~HttpResponse( void )
// { }

// int HttpResponse::sendDataToSocket( int socketfd, int flags )
// {
//     std::cout << "send data to socket" << std::endl;
//     int writeByte = 0;
//     if (this->headerSize > 0) {
//         writeByte = send(socketfd, this->headerPtr, this->headerSize, flags);
//         if (writeByte > 0) {
//             this->headerPtr += writeByte;
//             this->headerSize -= writeByte;
//         }
//     } else if (this->bodySize > 0) {
//         writeByte = send(socketfd, this->bodyPtr, this->bodySize, flags);
//         if (writeByte > 0) {
//             this->bodyPtr += writeByte;
//             this->bodySize -= writeByte;
//         }
//     } 
//     return (writeByte);
// }

// bool    HttpResponse::empty( void ) const
// {
//     return (this->bodySize == 0 && this->headerSize == 0);
// }

// void    HttpResponse::setContentExternalBuffer( buff_t const & buffer )
// {
//     this->bodyPtr = buffer.data();
//     this->bodySize = buffer.size();
// }


// ws_http::statuscodes_t HttpResponse::setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType )
// {
//     std::cout << "-- setHeader\n";
//     this->_bodyBuffer.reserve(4096);
//     std::stringstream ss;
//     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
//     ss << "connection: close" << ws_http::crlf;
//     std::size_t httpBodyStreamSize = getStreamSize(httpBodyStream);
//     ss << "content-type: " << mimeType << ws_http::crlf;
//     ss << "content-length: " << httpBodyStreamSize << ws_http::crlf;
//     ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
//     ss << ws_http::crlf;

//     this->_headerBuffer.clear();
//     this->_bodyBuffer.clear();
//     addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
//     addStreamToBuff(this->_bodyBuffer, httpBodyStream, httpBodyStreamSize);
//     this->headerPtr = this->_headerBuffer.data();
//     this->headerSize = this->_headerBuffer.size();
//     this->bodyPtr = this->_bodyBuffer.data();
//     this->bodySize = this->_bodyBuffer.size();
//     return (status);
// }

// ws_http::statuscodes_t  HttpResponse::setResponseDefaultError( ws_http::statuscodes_t errorStatusCode )
// {
//     std::stringstream ss;
//     ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
//         << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" "</head>" 
//         << "<body>" "<div style=\"text-align: center;\">" 
//         << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>"
//         << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
//         << "</body>" "</html>";
//     return(this->setResponseStatic(errorStatusCode, ss, "text/html"));
// }

// ws_http::statuscodes_t  HttpResponse::setResponseRedirection( ws_http::statuscodes_t status, std::string const & location )
// {
//    std::cout << "-- setRedirection\n";
//     std::stringstream ss;
//     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
//     ss << "connection: close" << ws_http::crlf;
//     ss << "location: " << location << ws_http::crlf;
//     ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
//     ss << ws_http::crlf;

//     this->_headerBuffer.clear();
//     addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
//     this->headerPtr = this->_headerBuffer.data();
//     this->headerSize = this->_headerBuffer.size();
//     return (status);
// }

// ws_http::statuscodes_t     HttpResponse::appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
// {
//     std::copy(itStart, itEnd, std::back_inserter(this->_headerBuffer));
//     if (flush) {
//         this->headerPtr = this->_headerBuffer.data();
//         this->headerSize = this->_headerBuffer.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t     HttpResponse::appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
// {
//     std::copy(itStart, itEnd, std::back_inserter(this->_bodyBuffer));
//     if (flush) {
//         this->bodyPtr = this->_bodyBuffer.data();
//         this->bodySize = this->_bodyBuffer.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }








// HttpContent::HttpContent( void )
//  : headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
// { }

// HttpContent::~HttpContent( void )
// { }

// int HttpContent::sendDataToSocket( int socketfd, int flags )
// {
//     std::cout << "send data to socket" << std::endl;
//     int writeByte = 0;
//     if (this->headerSize > 0) {
//         writeByte = send(socketfd, this->headerPtr, this->headerSize, flags);
//         if (writeByte > 0) {
//             this->headerPtr += writeByte;
//             this->headerSize -= writeByte;
//         }
//     } else if (this->bodySize > 0) {
//         writeByte = send(socketfd, this->bodyPtr, this->bodySize, flags);
//         if (writeByte > 0) {
//             this->bodyPtr += writeByte;
//             this->bodySize -= writeByte;
//         }
//     } 
//     return (writeByte);
// }

// bool    HttpContent::empty( void ) const
// {
//     return (this->bodySize == 0 && this->headerSize == 0);
// }

// void    HttpContent::setContentExternalBuffer( buff_t const & buffer )
// {
//     this->bodyPtr = buffer.data();
//     this->bodySize = buffer.size();
// }


// ws_http::statuscodes_t HttpContent::setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType )
// {
//     std::cout << "-- setHeader\n";
//     this->_bodyBuffer.reserve(4096);
//     std::stringstream ss;
//     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
//     ss << "connection: close" << ws_http::crlf;
//     std::size_t httpBodyStreamSize = getStreamSize(httpBodyStream);
//     ss << "content-type: " << mimeType << ws_http::crlf;
//     ss << "content-length: " << httpBodyStreamSize << ws_http::crlf;
//     ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
//     ss << ws_http::crlf;

//     this->_headerBuffer.clear();
//     this->_bodyBuffer.clear();
//     addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
//     addStreamToBuff(this->_bodyBuffer, httpBodyStream, httpBodyStreamSize);
//     this->headerPtr = this->_headerBuffer.data();
//     this->headerSize = this->_headerBuffer.size();
//     this->bodyPtr = this->_bodyBuffer.data();
//     this->bodySize = this->_bodyBuffer.size();
//     return (status);
// }

// ws_http::statuscodes_t  HttpContent::setResponseDefaultError( ws_http::statuscodes_t errorStatusCode )
// {
//     std::stringstream ss;
//     ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
//         << "<title>" << ws_http::statuscodes.at(errorStatusCode) << "</title>" "</head>" 
//         << "<body>" "<div style=\"text-align: center;\">" 
//         << "<h1>" << ws_http::statuscodes.at(errorStatusCode) << "</h1>"
//         << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
//         << "</body>" "</html>";
//     return(this->setResponseStatic(errorStatusCode, ss, "text/html"));
// }

// ws_http::statuscodes_t  HttpContent::setResponseRedirection( ws_http::statuscodes_t status, std::string const & location )
// {
//    std::cout << "-- setRedirection\n";
//     std::stringstream ss;
//     ss << "HTTP/1.1 " << ws_http::statuscodes.at(status) << ws_http::crlf;
//     ss << "connection: close" << ws_http::crlf;
//     ss << "location: " << location << ws_http::crlf;
//     ss << "date: " << getDateString(0, "%a, %d %b %Y %H:%M:%S GMT") << ws_http::crlf;
//     ss << ws_http::crlf;

//     this->_headerBuffer.clear();
//     addStreamToBuff(this->_headerBuffer, ss, getStreamSize(ss));
//     this->headerPtr = this->_headerBuffer.data();
//     this->headerSize = this->_headerBuffer.size();
//     return (status);
// }

// ws_http::statuscodes_t     HttpContent::appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
// {
//     std::copy(itStart, itEnd, std::back_inserter(this->_headerBuffer));
//     if (flush) {
//         this->headerPtr = this->_headerBuffer.data();
//         this->headerSize = this->_headerBuffer.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t     HttpContent::appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush )
// {
//     std::copy(itStart, itEnd, std::back_inserter(this->_bodyBuffer));
//     if (flush) {
//         this->bodyPtr = this->_bodyBuffer.data();
//         this->bodySize = this->_bodyBuffer.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


