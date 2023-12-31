/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:12 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/05 09:29:03 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

// HttpServer::HttpServer() : _serverSocket(-1)
// {
// 	// SocketAddr_in	serverAddress;
// }

HttpServer::HttpServer(/*ConfigParser::ServerContext serverConfig, std::map<std::string, std::vector<std::string> > httpConfig*/) : _serverSocket(-1), _nfds(0), _response("")
{
	unsigned short	port = 8082;
	// TODO: Config handler here to get info for creating all the servers

	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw HttpServer::CreatingServerSocketException();

	// Setup server address structure
	memset(&this->_serverAddress, 0, sizeof(this->_serverAddress));
	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(port);

	// Set the server socket to non-blocking mode
	fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
	// Bind the server socket to the specified address and port
	if (bind(this->_serverSocket, (struct sockaddr *)&this->_serverAddress, sizeof(this->_serverAddress)) == -1)
		throw HttpServer::BindSocketServerException();

	// Listen for incoming connection
	if (listen(this->_serverSocket, 10) == -1)
		throw HttpServer::ListeningForConnectionException();

	// Initialize number of file descriptors to 1 and set memory allocation for pollfd structure
	this->_nfds = 1;
	memset(this->_fds, 0, sizeof(this->_fds));
	// Add the server socket to the pollfd array
	this->_fds[0].fd = this->_serverSocket;
	this->_fds[0].events = POLLIN;
}

HttpServer::~HttpServer()
{

}

// HttpServer::HttpServer(const HttpServer& src)
// {

// }

// HttpServer& HttpServer::operator=(const HttpServer& rhs)
// {

// }

void	HttpServer::start(void)
{
	while (true)
	{
		// Initialize poll function
		int ready = poll(this->_fds, this->_nfds, -1);
		if (ready == -1)
			throw HttpServer::EpollWaitException();

		// Set number of fds to a new variable to enable increamenting nfds freely
		int currSize = this->_nfds;

		//
		for (int i = 0; i < currSize; ++i)
		{
			if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
				continue;
			if (this->_fds[i].fd == this->_serverSocket)
				handleAccept();
			else
			{
				if (this->_fds[i].revents & (POLLIN | POLLHUP))
					handleRead(this->_fds[i].fd);
				if (this->_fds[i].revents & POLLOUT)
					handleWrite(this->_fds[i].fd);
			}
		}
	}
}

void	HttpServer::handleRead(int clientSocket)
{
	char	buffer[1024];

	ssize_t readByte = recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << readByte << " Bytes of data recieved from " << clientSocket << std::endl;
	if (readByte == -1)
	{
		std::cout << "Error reading from client socket." << strerror(errno) << std::endl;
		throw HttpServer::ClientSocketReadException();
	}
	else if (readByte == 0)
	{
		// Connection closed by client
		if (close(clientSocket) == -1)
			throw HttpServer::FailedToCloseFdException();
		stopMonitoring(clientSocket);
	}
	else
	{
		std::cout << buffer << std::endl;
		// RequestHandler	handler;
		// std::string	request(buffer, readByte);
		// std::cout << "<______________>" << std::endl;
		// this->_response = handler.handleRequest(request);
	}
}

void	HttpServer::handleWrite(int clientSocket)
{
	const char*	response = /*this->_response.c_str();*/ "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World.";
	// size_t	sentByte = 0;
	// size_t	totalByte = strlen(response);

	std::cout << "Start Sending..." << std::endl;
	std::cout << response << std::endl;
	
	ssize_t writeByte = send(clientSocket, response, strlen(response), 0);

	// while (sentByte < totalByte)
	// {
	// 	size_t	remByte = totalByte - sentByte;
	// 	size_t	batchByte;
	// 	if (remByte < totalByte)
	// 		batchByte = remByte;
	// 	else
	// 		batchByte = totalByte;
	// 	ssize_t writeByte = send(clientSocket, response, batchByte, 0);
		
	// 	if (writeByte == -1)
	// 		throw HttpServer::ClientSocketWriteException();
	// 	else if (writeByte == 0)
	// 	{
	// 		// Connection closed by client
	// 		if (close(clientSocket) == -1)
	// 			throw HttpServer::FailedToCloseFdException();
	// 		stopMonitoring(clientSocket);
	// 	}
	// 	sentByte += static_cast<int>(writeByte);
	// }
	
	if (writeByte == -1)
		throw HttpServer::ClientSocketWriteException();
	else if (writeByte == 0)
	{
		// Connection closed by client
		if (close(clientSocket) == -1)
			throw HttpServer::FailedToCloseFdException();
		stopMonitoring(clientSocket);
	}
	else
	{
		if (close(clientSocket) == -1)
			throw HttpServer::FailedToCloseFdException();
		stopMonitoring(clientSocket);
	}
}

void	HttpServer::handleAccept()
{
	while (true)
	{
		//Accept all incoming connections
		int	clientSocket = accept(this->_serverSocket, NULL, NULL);

		if (clientSocket == -1)
			throw HttpServer::AcceptConnectionException();
		
		// Set client socket to non-blocking
		fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

		// Add client socket to pollfd array
		for (int i = 0; i != this->_nfds; ++i)
		{
			if (this->_fds[i].fd == -1){
				this->_fds[this->_nfds].fd = clientSocket;
				break;
			}
		}
		this->_fds[this->_nfds].fd = clientSocket;
		this->_fds[this->_nfds].events = POLLIN | POLLHUP | POLLOUT;
		this->_nfds++;
		break;
	}
}

// Stop monitoring a file decriptor by removing it from the pollfd array
void	HttpServer::stopMonitoring(int clientSocket)
{
	int	i;

	for (i = 0; i < this->_nfds; ++i)
	{
		if (this->_fds[i].fd == clientSocket)
			break;
	}
	this->_fds[i].fd = -1;
}

