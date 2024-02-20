/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 12:02:23 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 11:52:50 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_DATA_HPP
#define CLIENT_DATA_HPP

#include <vector>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <map>
#include "ConfigParser.hpp"

// struct  IpPortData {
//     IpPortData( std::string const & ip, std::string const & port );
//     ~IpPortData( void );
//     std::string ipString;
//     std::string portString;
// };

struct Content {
    Content( void );
    Content( const char* dataPtr, int dataSize );
    ~Content( void );
    const char* data;
    int         size;
};


struct HttpServerData {
    HttpServerData( struct sockaddr_in const & addr, ConfigParser::ServerContext& serverConf );
    ~HttpServerData( void );
    ConfigParser::ServerContext& serverConfig;
    std::string ipString;
    std::string portString;
};

class HttpClientData {
public:
    HttpClientData( struct sockaddr_in const & addr, HttpServerData& servData );
    ~HttpClientData( void );
    
    Content*    getContent( void );
    HttpServerData& serverData;
    std::string     ipString;
    std::string     portString;
    bool            toSend;
    std::string     headerData;
    Content         headerContent;
    Content         bodyContent;
};

// class HttpData {
// public:
//     HttpData( int kq );
//     ~HttpData( void );
//     int addServerData( ConfigParser::ServerContext& serverConfig, int backlog);
//     int addClientData( int serversockfd );
// private:
//     int _kq;
//     std::map<int, HttpServerData>   _serverData;
//     std::map<int, HttpClientData>   _clientData;
// };


// struct HttpServerData {
//     HttpServerData( struct sockaddr_in const & addr, ConfigParser::ServerContext& serverConf );
//     ~HttpServerData( void );
//     ConfigParser::ServerContext& serverConfig;
//     std::string ipString;
//     std::string portString;
// };

// struct HttpClientData {
//     HttpClientData( struct sockaddr_in const & addr, HttpServerData& servData );
//     ~HttpClientData( void );
//     HttpServerData& serverData;
//     std::string     ipString;
//     std::string     portString;
//     bool            toSend;
//     std::string     headerData;
//     Content         headerContent;
//     Content         bodyContent;
// };

// class HttpData {
// public:
//     HttpData( int kq );
//     ~HttpData( void );
//     int addServerData( ConfigParser::ServerContext& serverConfig, int backlog);
//     int addClientData( int serversockfd );
// private:
//     int _kq;
//     std::map<int, HttpServerData>   _serverData;
//     std::map<int, HttpClientData>   _clientData;
// };

#endif