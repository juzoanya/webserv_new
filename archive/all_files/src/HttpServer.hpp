/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:20 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/19 22:50:46 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "headers.hpp"

class ConfigParser;
class RequestHandler;

class HttpServer;

struct ServerWasCreated {
	ServerWasCreated( ws_config_t& serverConfig );
	~ServerWasCreated( void );
	bool operator()( HttpServer const & server );
	std::string ipStr;
	std::string portStr;
};

class HttpServer
{
	private:
		int							_serverSocket;
		struct sockaddr_in			_serverAddress;
		struct pollfd				_fds[1024];
		int							_nfds;
		std::string					_response;
		// std::map<int, HttpStatic>	_clients;
	
	public:
		HttpServer();
		HttpServer(std::string const & ip, std::string const & port);
		//HttpServer(/*ConfigParser::ServerContext serverConfig, std::map<std::string, std::vector<std::string>> httpConfig*/);
		~HttpServer();
		// HttpServer(const HttpServer& src);
		// HttpServer& operator=(const HttpServer& rhs);

		void	start();
		void	handleRead(int clientSocket);
		void	handleWrite(int clientSocket);
		void	handleAccept();
		void	handleClientEvent(int clientSocket);
		void	stopMonitoring(int clientSocket);
		ConfigHandler		serverConfigs;

		class ListeningForConnectionException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error Listening for Connections.");
				}
		};
		
		class EPollEventException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error creating epoll instance.");
				}
		};

		class AcceptingConnectionException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connections.");
				}
		};

		class CreatingServerSocketException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connections.");
				}
		};
		
		class BindSocketServerException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error bindinding socket to this host:port.");
				}
		};

		class ClientSocketReadException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error reading from client socket.");
				}
		};

		class ClientSocketWriteException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error writing to client socket.");
				}
		};

		class AddToEpollSetException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error adding server socket to epoll set.");
				}
		};

		class DeleteFromEpollSetException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error deleting client socket to epoll set.");
				}
		};

		class EpollWaitException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Epoll wait Error.");
				}
		};

		class AcceptConnectionException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connection.");
				}
		};

		class FailedToCloseFdException : public std::exception
		{
			public:
				const char* what() const throw(){
					return("Error closing file descriptor.");
				}
		};
};



#endif