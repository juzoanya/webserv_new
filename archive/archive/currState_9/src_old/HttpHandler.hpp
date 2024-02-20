/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 22:17:15 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"


class HttpContent {
    public:
        HttpContent( void );
        ~HttpContent( void );
        int                     sendDataToSocket( int socketfd, int flags );
        ws_http::statuscodes_t  setResponseDefaultError( ws_http::statuscodes_t errorStatusCode );
        ws_http::statuscodes_t  setResponseRedirection( ws_http::statuscodes_t status, std::string const & location );
        ws_http::statuscodes_t  setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
        ws_http::statuscodes_t  appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
        ws_http::statuscodes_t  appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
        void                    setContentExternalBuffer( buff_t const & buffer );
        void                    setContent( std::istream& httpBodyStream, std::string const & mimeType );
        void                    setStatus( ws_http::statuscodes_t status );
        void                    setHeader( std::string const & key, std::string const & value );
        void                    flushResponse( void );
        bool                    empty( void ) const;
    private:
        const char* headerPtr;
        int         headerSize;
        const char* bodyPtr;
        int         bodySize;
        buff_t      _headerBuffer;
        buff_t      _bodyBuffer;
};


class HttpHandler : public APollEventHandler, public IParentProcess {
    public:
        HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server );
        ~HttpHandler( void );
        void    handleEvent( struct pollfd & pollfd );
        void    closeChildProcess( int bytesSend );
        void    writeToChildProcess( int socketfd );
        void    readFromChildProcess( int socketfd );
        const WsIpPort  ipPortData;
    private:
        void                    handleCgi( HttpConfig const & config );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ws_http::statuscodes_t  processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ChildProcessHandler*    _childProcessHandler;
        HttpContent             _content;
        HttpServer&             _server;
        HttpRequest             _request;
};

#endif