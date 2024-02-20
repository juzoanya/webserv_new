/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/19 21:41:16 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"



class HttpHandler : public APollEventHandler {
    public:
        HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server );
        ~HttpHandler( void );
        void    handleEvent( struct pollfd & pollfd );
        void    handleChildEvent( struct pollfd & pollfd );
        const WsIpPort  ipPortData;
    private:

        void                    quit( void );
        void                    quitCgiProcess( void );
        void                    handleCgi( HttpConfig const & config );
        
        
        // ws_http::statuscodes_t  processFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );


        void  processResponse( ws_http::statuscodes_t currentStatus );
        void  processError( HttpConfig const & config, ws_http::statuscodes_t errorCode );
        void  processGetHead( HttpConfig const & config, ws_http::statuscodes_t statusCode );
        void  processPost( HttpConfig const & config );
        void  processDelete( HttpConfig const & config );
       
       
        // backup
        ws_http::statuscodes_t  processResponsePost( HttpConfig const & config, ws_http::statuscodes_t statusCode );
        ws_http::statuscodes_t  processResponseDelete( HttpConfig const & config, ws_http::statuscodes_t statusCode );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ws_http::statuscodes_t  processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );


        ChildProcessHandler*    _childProcessHandler;
        HttpServer&             _server;
        HttpMessage             _httpMessage;
};

#endif
