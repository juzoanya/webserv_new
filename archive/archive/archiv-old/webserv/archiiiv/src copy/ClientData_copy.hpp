/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData_copy.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 12:02:23 by mberline          #+#    #+#             */
/*   Updated: 2023/12/28 15:45:37 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_DATA_COPY_HPP
#define CLIENT_DATA_COPY_HPP

#include <vector>
#include <string>
#include <arpa/inet.h>
#include "ConfigParser.hpp"

struct Content {
    Content( void );
    Content( const char* dataPtr, int dataSize );
    ~Content( void );
    const char* data;
    int         size;
};

struct  IpPortData {
    IpPortData( std::string const & ip, std::string const & port );
    ~IpPortData( void );
    std::string ipString;
    std::string portString;
};

struct HttpServerData {
    HttpServerData( struct sockaddr_in const & addr, ConfigParser::ServerContext& serverConf );
    ~HttpServerData( void );
    ConfigParser::ServerContext& serverConfig;
    std::string ipString;
    std::string portString;
};

struct HttpClientData {
    HttpClientData( struct sockaddr_in const & addr, HttpServerData& servData );
    ~HttpClientData( void );
    HttpServerData& serverData;
    std::string     ipString;
    std::string     portString;
    bool            toSend;
    std::string     headerData;
    Content         headerContent;
    Content         bodyContent;
};

#endif