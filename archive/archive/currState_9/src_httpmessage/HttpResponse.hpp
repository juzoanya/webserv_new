/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 07:54:32 by mberline          #+#    #+#             */
/*   Updated: 2024/02/11 19:30:29 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "headers.hpp"

// class HttpResponse {
//     public:
//         HttpResponse( void );
//         ~HttpResponse( void );
//         int                     sendDataToSocket( int socketfd, int flags );
//         ws_http::statuscodes_t  setResponseDefaultError( ws_http::statuscodes_t errorStatusCode );
//         ws_http::statuscodes_t  setResponseRedirection( ws_http::statuscodes_t status, std::string const & location );
//         ws_http::statuscodes_t  setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
//         ws_http::statuscodes_t  appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
//         ws_http::statuscodes_t  appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
//         void                    setContentExternalBuffer( buff_t const & buffer );
//         void                    setContent( std::istream& httpBodyStream, std::string const & mimeType );
//         void                    setStatus( ws_http::statuscodes_t status );
//         void                    setHeader( std::string const & key, std::string const & value );
//         void                    flushResponse( void );
//         bool                    empty( void ) const;
//     private:
//         const char* headerPtr;
//         int         headerSize;
//         const char* bodyPtr;
//         int         bodySize;
//         buff_t      _headerBuffer;
//         buff_t      _bodyBuffer;
// };

// class HttpContent {
//     public:
//         HttpContent( void );
//         ~HttpContent( void );
//         int                     sendDataToSocket( int socketfd, int flags );
//         ws_http::statuscodes_t  setResponseDefaultError( ws_http::statuscodes_t errorStatusCode );
//         ws_http::statuscodes_t  setResponseRedirection( ws_http::statuscodes_t status, std::string const & location );
//         ws_http::statuscodes_t  setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
//         ws_http::statuscodes_t  appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
//         ws_http::statuscodes_t  appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
//         void                    setContentExternalBuffer( buff_t const & buffer );
//         void                    setContent( std::istream& httpBodyStream, std::string const & mimeType );
//         void                    setStatus( ws_http::statuscodes_t status );
//         void                    setHeader( std::string const & key, std::string const & value );
//         void                    flushResponse( void );
//         bool                    empty( void ) const;
//     private:
//         const char* headerPtr;
//         int         headerSize;
//         const char* bodyPtr;
//         int         bodySize;
//         buff_t      _headerBuffer;
//         buff_t      _bodyBuffer;
// };


#endif
