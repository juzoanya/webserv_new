


#pragma once
#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "headers.hpp"

class ConfigParser;
class RequestHandler;

class HttpServer
{
	private:
		int					_serverSocket;
		struct sockaddr_in	_serverAddress;
		struct pollfd		_fds[1024];
		int					_nfds;
	
	public:
		HttpServer();
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

		class ListeningForConnectionException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error Listening for Connections.");
				}
		};
		
		class EPollEventException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error creating epoll instance.");
				}
		};

		class AcceptingConnectionException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connections.");
				}
		};

		class CreatingServerSocketException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connections.");
				}
		};
		
		class BindSocketServerException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connections.");
				}
		};

		class ClientSocketReadException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error reading from client socket.");
				}
		};

		class ClientSocketWriteException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error writing to client socket.");
				}
		};

		class AddToEpollSetException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error adding server socket to epoll set.");
				}
		};

		class DeleteFromEpollSetException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error deleting client socket to epoll set.");
				}
		};

		class EpollWaitException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Epoll wait Error.");
				}
		};

		class AcceptConnectionException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error accepting connection.");
				}
		};

		class FailedToCloseFdException : std::exception
		{
			public:
				const char* what() const throw(){
					return("Error closing file descriptor.");
				}
		};
};



#endif