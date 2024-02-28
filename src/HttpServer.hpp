/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 22:20:59 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "headers.hpp"

class HttpServer : public APollEventHandler, public IOnHttpMessageBodySize {
    public:
        HttpServer( WsIpPort const & ipPort, Polling & polling );
        ~HttpServer( void );

        const WsIpPort  serverIpPort;
        void        addServerConfig( ConfigParser::ServerContext const & serverContext );
        HttpConfig  getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader );
        long        operator()( std::string const & host, std::string const & path );
        void        handleEvent( struct pollfd pollfd );
        std::vector<ConfigParser::ServerContext const *> const & getServerConfigs( void ) const;

    private:
        std::vector<ConfigParser::ServerContext const *>   _serverConfigs;
};

#endif
