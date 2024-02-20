/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/14 07:31:23 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "headers.hpp"

class HttpServer : public APollEventHandler, public IOnHttpMessageBodySize {
    public:
        HttpServer( WsIpPort const & ipPort, Polling & polling );
        // HttpServer( WsIpPort const & ipPort, Polling & polling, ConfigParser::ServerContext const & serverContext );
        ~HttpServer( void );

        const WsIpPort  serverIpPort;
        void        addServerConfig( ConfigParser::ServerContext const & serverContext );
        HttpConfig  getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader );
        long        operator()( std::string const & host, std::string const & path );
        void        handleEvent( struct pollfd & pollfd );
        std::vector<ConfigParser::ServerContext const *> const & getServerConfigs( void ) const;
        struct ListeningForConnectionException  : public std::exception { const char *what() const throw() { return ("Error Listening for Connections."); } };
        struct EPollEventException              : public std::exception { const char *what() const throw() { return ("Error creating epoll instance."); } };
        struct AcceptingConnectionException     : public std::exception { const char *what() const throw() { return ("Error accepting connections."); } };
        struct CreatingServerSocketException    : public std::exception { const char *what() const throw() { return ("Error accepting connections."); } };
        struct BindSocketServerException        : public std::exception { const char *what() const throw() { return ("Error bindinding socket to this host:port."); } };
        struct ClientSocketReadException        : public std::exception { const char *what() const throw() { return ("Error reading from client socket."); } };
        struct ClientSocketWriteException       : public std::exception { const char *what() const throw() { return ("Error writing to client socket."); } };
        struct AddToEpollSetException           : public std::exception { const char *what() const throw() { return ("Error adding server socket to epoll set."); } };
        struct DeleteFromEpollSetException      : public std::exception { const char *what() const throw() { return ("Error deleting client socket to epoll set."); } };
        struct EpollWaitException               : public std::exception { const char *what() const throw() { return ("Epoll wait Error."); } };
        struct AcceptConnectionException        : public std::exception { const char *what() const throw() { return ("Error accepting connection."); } };
        struct FailedToCloseFdException         : public std::exception { const char *what() const throw() { return ("Error closing file descriptor."); } };
        struct SetSocketNonBlockingexception    : public std::exception { const char *what() const throw() { return ("Error setting Socket non-blocking."); } };
    private:
        std::vector<ConfigParser::ServerContext const *>   _serverConfigs;
};

#endif
