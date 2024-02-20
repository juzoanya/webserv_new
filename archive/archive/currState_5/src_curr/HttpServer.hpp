/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:20 by juzoanya          #+#    #+#             */
/*   Updated: 2024/02/08 15:44:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "headers.hpp"

#define WS_HTTP_SERVER_MAX_CONNECTIONS 1024

class ConfigParser;
class RequestHandler;

// union USocketData {
//     HttpMessage*    httpData;
//     int             correspondingHttpDataIndex;
// };

class SocketData {
    public:
        SocketData( bool isclient, int index );
        SocketData( bool isclient, int index, HttpMessage* HttpMessage );
        ~SocketData( void );
        // USocketData data;
        HttpMessage& getHttpMessage( void ) const;
        const bool      isClient;
        const int       arrIndex;
        int             relatedIndex;
    private:
        HttpMessage*    httpMessage;
};

class HttpServ
{
private:
    int                         _serverSocket;
    std::vector<struct pollfd>  _pollFds;
    std::vector<HttpMessage*>   _clients;
    std::vector<SocketData>     _data;
    buff_t                      _readBuffer;

public:
    HttpServ( WsIpPort const & ipPort );
    ~HttpServ();

    void    runPoll( void );
    void    handleRead( std::size_t index );
    // void    addMonitoring( int fd, short events, HttpMessage* data );
    void    addMonitoring( int fd, short events, bool isClient );
    void    handleWrite( std::size_t index );
    int     handleAccept( void );
    void    stopMonitoring( std::size_t index );
    ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, bool internal );
    
    ConfigHandler serverConfigs;

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
};

// class HttpServer
// {
// 	private:
// 		int							_serverSocket;
// 		struct sockaddr_in			_serverAddress;
// 		struct pollfd				_fds[1024];
// 		int							_nfds;
// 		std::string					_response;
// 		std::map<int, HttpStatic>	_clients;

// 	public:
// 		HttpServer(std::string const & ip, std::string const & port);
// 		~HttpServer();
// 		// HttpServer(const HttpServer& src);
// 		// HttpServer& operator=(const HttpServer& rhs);

// 		void	start();
// 		void	handleRead(int clientSocket);
// 		void	handleWrite(int clientSocket);
// 		void	handleAccept();
// 		void	handleClientEvent(int clientSocket);
// 		void	stopMonitoring(int clientSocket);
// 		ConfigHandler		serverConfigs;

// 		class ListeningForConnectionException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error Listening for Connections.");
// 				}
// 		};

// 		class EPollEventException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error creating epoll instance.");
// 				}
// 		};

// 		class AcceptingConnectionException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error accepting connections.");
// 				}
// 		};

// 		class CreatingServerSocketException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error accepting connections.");
// 				}
// 		};

// 		class BindSocketServerException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error bindinding socket to this host:port.");
// 				}
// 		};

// 		class ClientSocketReadException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error reading from client socket.");
// 				}
// 		};

// 		class ClientSocketWriteException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error writing to client socket.");
// 				}
// 		};

// 		class AddToEpollSetException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error adding server socket to epoll set.");
// 				}
// 		};

// 		class DeleteFromEpollSetException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error deleting client socket to epoll set.");
// 				}
// 		};

// 		class EpollWaitException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Epoll wait Error.");
// 				}
// 		};

// 		class AcceptConnectionException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error accepting connection.");
// 				}
// 		};

// 		class FailedToCloseFdException : public std::exception
// 		{
// 			public:
// 				const char* what() const throw(){
// 					return("Error closing file descriptor.");
// 				}
// 		};
// };

#endif