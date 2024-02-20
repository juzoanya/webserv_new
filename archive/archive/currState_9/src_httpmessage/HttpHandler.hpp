/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 17:00:54 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"

class HttpHandler : public APollEventHandler, public IParentProcess {
    public:
        HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server );
        ~HttpHandler( void );
        void    handleEvent( struct pollfd & pollfd );
        void    handleChildEvent( struct pollfd & pollfd );
        const WsIpPort  ipPortData;
    private:

        enum CgiStatus { NO_CGI, CGI_WRITE_TO_CHILD, CGI_READ_FROM_CHILD, CGI_DONE };
        char    _cgiStatus;

        void                    handleCgi( HttpConfig const & config );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ws_http::statuscodes_t  processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ChildProcessHandler*    _childProcessHandler;
        // HttpContent             _content;
        HttpServer&             _server;
        HttpMessage             _httpMessage;
};

// class HttpHandler : public APollEventHandler, public IParentProcess {
//     public:
//         HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server );
//         ~HttpHandler( void );
//         void    handleEvent( struct pollfd & pollfd );
//         void    closeChildProcess( int bytesSend );
//         void    writeToChildProcess( int socketfd );
//         void    readFromChildProcess( int socketfd );
//         const WsIpPort  ipPortData;
//     private:
//         void                    handleCgi( HttpConfig const & config );
//         ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
//         ws_http::statuscodes_t  processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
//         ChildProcessHandler*    _childProcessHandler;
//         // HttpContent             _content;
//         HttpServer&             _server;
//         HttpRequest             _request;
// };

#endif