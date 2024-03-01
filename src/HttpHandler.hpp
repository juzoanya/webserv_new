/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 21:03:00 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"

class HttpHandler : public APollEventHandler {
	public:
		HttpHandler( WsIpPort const & ipPort, Polling & polling, HttpServer & server );
		~HttpHandler( void );
		void    handleEvent( struct pollfd pollfd );
		void    handleChildEvent( struct pollfd & pollfd );
		void    handleTimeout( void );

		const WsIpPort  ipPortData;

	private:
		void	quit( void );
		void	quitCgiProcess( void );
		void	handleCgi( HttpConfig const & config );
		void  	processResponse( ws_http::statuscodes_t currentStatus );
		void  	processError( HttpConfig const & config, ws_http::statuscodes_t errorCode );
		void  	processGetHead( HttpConfig const & config, FileInfo const & fileInfo, ws_http::statuscodes_t statusCode );
		void  	processPost( HttpConfig const & config, FileInfo const & fileInfo );
		void  	processDelete( HttpConfig const & config, FileInfo const & fileInfo );

		ChildProcessHandler*    _childProcessHandler;
		HttpServer&             _server;
		HttpMessage             _httpMessage;
		bool					_closeConnect;
};

#endif
