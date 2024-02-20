/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:12 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/21 22:20:27 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

// HttpServer::HttpServer() : _serverSocket(-1)
// {
// 	// SocketAddr_in	serverAddress;
// }

ServerWasCreated::ServerWasCreated( ws_config_t& serverConfig )
{
	ws_config_t::iterator it = serverConfig.find("listen");
	std::size_t pos = std::string::npos;
	if (it != serverConfig.end())
		pos = it->second[0].find(':');
	if (it == serverConfig.end() || it->second.size() == 0) {
		this->ipStr = "0.0.0.0";
		this->portStr = "8080";
	} else if (pos == std::string::npos) {
		this->ipStr = "0.0.0.0";
		this->portStr = it->second[0];
	} else {
		this->ipStr = it->second[0].substr(0, pos);
		this->portStr = it->second[0].substr(pos + 1, std::string::npos);
	}
}

ServerWasCreated::~ServerWasCreated( void )
{ }

bool ServerWasCreated::operator()( HttpServer const & server )
{
	if (server.serverConfigs.serverIp == this->ipStr && server.serverConfigs.serverPort == this->portStr)
		return (true);
	return (false);
}


void  fillSockAddrIn(struct sockaddr_in& addr, std::string const & ipStr, std::string const & portStr)
{
    memset(&addr, 0, sizeof(struct sockaddr_in));
	uint32_t ip = 0;
	unsigned int ipPartNbr;
    // std::size_t pos_start = 0, pos = 0;
    const char* ipPtr = ipStr.data();
    char* rest;
	for (int i = 0; i != 4; ++i) {
        ipPartNbr = strtol(ipPtr, &rest, 10);
        if ((i == 3 && *rest != 0) || (i < 3 && *rest != '.') || (i < 3 && !std::isdigit(*(rest + 1))))
            throw::std::runtime_error("invalid ip address");
        ipPtr = rest + 1;
		ip |= (ipPartNbr << (i * 8));
	}
    unsigned int port = strtol(portStr.c_str(), &rest, 10);
    if (port == 0 || *rest != 0)
        throw::std::runtime_error("invalid port");
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip;
    addr.sin_port = htons(port);
}

HttpServer::HttpServer(std::string const & ip, std::string const & port)
 : _serverSocket(-1), _nfds(0), _response(""), serverConfigs(ip, port)
{
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw HttpServer::CreatingServerSocketException();

	// reuse the ip:port socket
	int set = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1)
        throw std::runtime_error("setsockopt reuse addr error");

	// Setup server address structure
	fillSockAddrIn(this->_serverAddress, ip, port);

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

HttpServer::HttpServer(/*ConfigParser::ServerContext serverConfig, std::map<std::string, std::vector<std::string> > httpConfig*/)
 : _serverSocket(-1), _nfds(0), _response(""), serverConfigs("0.0.0.0", "8082")
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
	// Initialize poll function
	int ready = poll(this->_fds, this->_nfds, -1);
	if (ready == -1)
		throw HttpServer::EpollWaitException();

	// Set number of fds to a new variable to enable increamenting nfds freely
	int currSize = this->_nfds;

	//
	for (int i = 0; i < currSize; ++i)
	{
		// if (this->_fds[i].revents & POLLHUP && this->_fds[i].fd != -1) {
		// 	std::cout << "POLLHUP!\n";
		// 	exit(1);
		// }
		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
			continue;
		if (this->_fds[i].fd == this->_serverSocket) {
			handleAccept();
		} else {
			if (this->_fds[i].revents & (POLLIN | POLLHUP))
				handleRead(this->_fds[i].fd);
			if (this->_fds[i].revents & POLLOUT)
				handleWrite(this->_fds[i].fd);
		}
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
	if (close(clientSocket) == -1)
		throw HttpServer::FailedToCloseFdException();
	this->_clients.erase(clientSocket);
}


void	HttpServer::handleRead(int clientSocket)
{
	std::cout << "\n\n --------- HANDLE READ -------\n" << std::endl;
	buff_t	buffer(4096);

	std::map<int, HttpStatic>::iterator clientIt = this->_clients.find(clientSocket);
	if (clientIt == this->_clients.end())
		return ;

	ssize_t readByte = recv(clientSocket, buffer.data(), buffer.size(), 0);
	std::cout << readByte << " Bytes of data recieved from " << clientSocket << std::endl;
	if (readByte == -1) {
		std::cout << "Error reading from client socket." << strerror(errno) << std::endl;
		stopMonitoring(clientSocket);
		throw HttpServer::ClientSocketReadException();
	} else if (readByte == 0) {
		// Connection closed by client
		stopMonitoring(clientSocket);
	} else {
		clientIt->second.handleData(buffer.begin(), buffer.begin() + readByte);
	}
}

void	HttpServer::handleWrite(int clientSocket)
{
	// std::cout << "\n\n --------- HANDLE WRITE -------\n" << std::endl;
	// std::cout << "HANDLE WRITE at: " << clientSocket << std::endl;
	// static int ii = 0;
	// if (ii++ == 100)
	// 	exit(1);
	std::map<int, HttpStatic>::iterator clientIt = this->_clients.find(clientSocket);
	if (clientIt == this->_clients.end() || !clientIt->second.dataPtr || clientIt->second.dataSize == 0)
		return ;

	ssize_t writeByte = send(clientSocket, clientIt->second.dataPtr, clientIt->second.dataSize, 0);
	if (writeByte == -1) {
		stopMonitoring(clientSocket);
		throw HttpServer::ClientSocketWriteException();
	} else if (writeByte == 0) {
		stopMonitoring(clientSocket);
	} else {
		clientIt->second.dataPtr += writeByte;
		clientIt->second.dataSize -= writeByte;
		std::cout << "written - dataSize: " << clientIt->second.dataSize << std::endl;
		if (clientIt->second.dataSize == 0)
			stopMonitoring(clientSocket);
	}
}

int	findUnusedPoll(struct pollfd statArr[], int arrSize)
{
	for (int i = 0; i != arrSize; ++i) {
		if (statArr[i].fd == -1)
			return (i);
	}
	return (arrSize);
}

void	HttpServer::handleAccept()
{
	std::cout << "\n\n --------- HANDLE ACCEPT -------\n" << std::endl;
	// while (true)
	// {
		//Accept all incoming connections
		int	clientSocket = accept(this->_serverSocket, NULL, NULL);

		if (clientSocket == -1)
			throw HttpServer::AcceptConnectionException();
		
		// Set client socket to non-blocking
		fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

		// Add client socket to pollfd array
		// for (int i = 0; i != this->_nfds; ++i)
		// {
		// 	if (this->_fds[i].fd == -1){
		// 		this->_fds[i].fd = clientSocket;
		// 		std::cout<< "new clientsocket - old reused: " << clientSocket << " - at: " << i << std::endl;
		// 		this->_clients.insert(std::make_pair(clientSocket, HttpStatic(this->serverConfigs)));
		// 		break;
		// 	}
		// }

		for (int i = 0; i != this->_nfds; ++i) {
			if (this->_fds[i].fd == clientSocket) {
				std::cout << "DOUBLE!!\n";
				exit(1);				
			}
		}

		int unusedPoll = findUnusedPoll(this->_fds, this->_nfds);
		if (unusedPoll != this->_nfds) {
			this->_fds[unusedPoll].fd = clientSocket;
		} else {
			this->_fds[this->_nfds].fd = clientSocket;
			this->_fds[this->_nfds].events = POLLIN | POLLOUT;
			this->_nfds++;
		}
		std::cout<< "new clientsocket: " << clientSocket << std::endl;
		this->_clients.insert(std::make_pair(clientSocket, HttpStatic(this->serverConfigs)));
		// break;
	// }
}




// void	HttpServer::handleRead(int clientSocket)
// {
// 	char	buffer[4096];
// 	// buff_t	buffer(4096);

// 	ssize_t readByte = recv(clientSocket, buffer, sizeof(buffer), 0);
// 	std::cout << "\n\n --------- HANDLE READ -------\n" << std::endl;
// 	std::cout << readByte << " Bytes of data recieved from " << clientSocket << std::endl;
// 	if (readByte == -1)
// 	{
// 		std::cout << "Error reading from client socket." << strerror(errno) << std::endl;
// 		throw HttpServer::ClientSocketReadException();
// 	}
// 	else if (readByte == 0)
// 	{
// 		// Connection closed by client
// 		if (close(clientSocket) == -1)
// 			throw HttpServer::FailedToCloseFdException();
// 		stopMonitoring(clientSocket);
// 	}
// 	else
// 	{
// 		// std::cout << buffer << std::endl;
// 		RequestHandler	handler;
// 		std::string	request(buffer, readByte);
// 		std::cout << request << std::endl;
// 		// std::cout << "<______________>" << std::endl;
// 		// this->_response = handler.handleRequest(request);

// 		// std::string reqline;
// 		// std::stringstream ss(request);
// 		// std::getline(ss, reqline);
// 		// std::string path = reqline.substr(reqline.find_first_of(' ') + 1, + );
// 		std::string path = handler.getRequestPath(request);
// 		std::size_t pos = request.find("host: ");
// 		if (pos == std::string::npos)
// 			pos = request.find("Host: ");
// 		std::string host = request.substr(pos + 6, request.find("\r\n", pos + 6) - (pos + 6));
// 		std::cout << "\n\n --------- CONFIG DATA ------- \n" << std::endl;
// 		std:: cout << "path: " << path << std::endl;
// 		std::cout << "host: " << host << std::endl;
		
// 		HttpConfig currConf = this->serverConfigs.getHttpConfig(path, host, "/var");
		
// 		std::cout << "currConf serverName: " << currConf.getServerName() << std::endl;
// 		std::cout << "currConf root: " << currConf.getFilePath() << std::endl;


// 		if (close(clientSocket) == -1)
// 			throw HttpServer::FailedToCloseFdException();
// 		stopMonitoring(clientSocket);
// 	}
// }

// void	HttpServer::handleWrite(int clientSocket)
// {
// 	(void)clientSocket;
// 	// std::cout << "\n\n --------- HANDLE WRITE -------\n" << std::endl;
// 	// const char*	response = /*this->_response.c_str();*/ "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World.";

// 	// std::cout << "Start Sending..." << std::endl;
// 	// std::cout << response << std::endl;
	
// 	// ssize_t writeByte = send(clientSocket, response, strlen(response), 0);
// 	// if (writeByte == -1)
// 	// 	throw HttpServer::ClientSocketWriteException();
// 	// else if (writeByte == 0)
// 	// {
// 	// 	// Connection closed by client
// 	// 	if (close(clientSocket) == -1)
// 	// 		throw HttpServer::FailedToCloseFdException();
// 	// 	stopMonitoring(clientSocket);
// 	// }
// 	// else
// 	// {
// 	// 	if (close(clientSocket) == -1)
// 	// 		throw HttpServer::FailedToCloseFdException();
// 	// 	stopMonitoring(clientSocket);
// 	// }
// }

// void	HttpServer::handleAccept()
// {
// 	std::cout << "\n\n --------- HANDLE ACCEPT -------\n" << std::endl;
// 	while (true)
// 	{
// 		//Accept all incoming connections
// 		int	clientSocket = accept(this->_serverSocket, NULL, NULL);

// 		if (clientSocket == -1)
// 			throw HttpServer::AcceptConnectionException();
		
// 		// Set client socket to non-blocking
// 		fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

// 		// Add client socket to pollfd array
// 		for (int i = 0; i != this->_nfds; ++i)
// 		{
// 			if (this->_fds[i].fd == -1){
// 				this->_fds[this->_nfds].fd = clientSocket;
// 				break;
// 			}
// 		}
// 		this->_fds[this->_nfds].fd = clientSocket;
// 		this->_fds[this->_nfds].events = POLLIN | POLLHUP | POLLOUT;
// 		this->_nfds++;
// 		break;
// 	}
// }

