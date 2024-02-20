/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:12 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/22 16:59:58 by mberline         ###   ########.fr       */
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


std::string getPortStr(struct sockaddr_in const & addr)
{
    std::stringstream ss;
    ss << htons(addr.sin_port);
    return (ss.str());
}

std::string getIpStr(struct sockaddr_in const & addr)
{
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    std::stringstream ss;
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
    return (ss.str());
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

// HttpServer::HttpServer(const HttpServer& src) { }

// HttpServer& HttpServer::operator=(const HttpServer& rhs) { }


// Stop monitoring a file decriptor by removing it from the pollfd array
void	HttpServer::stopMonitoring(int clientSocket)
{
	std::cout << "--> stop monitoring socket: " << clientSocket << " and delete instance of HttpStatic" << std::endl;
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
	buff_t	buffer(4096);

	std::map<int, HttpStatic>::iterator clientIt = this->_clients.find(clientSocket);
	if (clientIt == this->_clients.end()) {
		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
		return ;
	}

	ssize_t readByte = recv(clientSocket, buffer.data(), buffer.size(), 0);
	std::cout << readByte << " Bytes of data recieved from clientSocket: " << clientSocket << std::endl;
	if (readByte == -1) {
		std::cout << "Error reading from client socket." << strerror(errno) << std::endl;
		stopMonitoring(clientSocket);
		throw HttpServer::ClientSocketReadException();
	} else if (readByte == 0) {
		std::cout << "clientSocket: " << clientSocket << " closed his write side, no more data to read" << std::endl;
		stopMonitoring(clientSocket);
	} else {
		clientIt->second.handleData(buffer.begin(), buffer.begin() + readByte);
	}
}

void	HttpServer::handleWrite(int clientSocket)
{
	std::map<int, HttpStatic>::iterator clientIt = this->_clients.find(clientSocket);
	if (clientIt == this->_clients.end()) {
		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
		return ;
	}
	if (clientIt->second.dataSize == 0)
		return ;

	ssize_t writeByte = send(clientSocket, clientIt->second.dataPtr, clientIt->second.dataSize, 0);
	std::cout << writeByte << " Bytes of data written to clientSocket: " << clientSocket << std::endl;
	if (writeByte == -1) {
		stopMonitoring(clientSocket);
		throw HttpServer::ClientSocketWriteException();
	} else if (writeByte == 0) {
		std::cout << "clientSocket: " << clientSocket << " closed the connection" << std::endl;
		stopMonitoring(clientSocket);
	} else {
		clientIt->second.dataPtr += writeByte;
		clientIt->second.dataSize -= writeByte;
		if (clientIt->second.dataSize == 0)
			stopMonitoring(clientSocket);
	}
}

int	findUnusedPoll(struct pollfd pollArr[], int arrSize)
{
	std::cout << "find unused poll, pollArr Size: " << arrSize << std::endl;
	for (int i = 0; i != arrSize; ++i) {
		std::cout << "curr fd no: " << pollArr[i].fd << std::endl;
		if (pollArr[i].fd == -1)
			return (i);
	}
	return (arrSize);
}

// void	HttpServer::handleAccept()
// {
// 	// std::cout << "\n\n --------- HANDLE ACCEPT -------\n" << std::endl;
// 	// std::cout << "--------- HANDLE ACCEPT -------" << std::endl;

// 	//Accept all incoming connections
// 	struct sockaddr	addr;
//     socklen_t   	addrlen = sizeof(addr);
// 	int	clientSocket = accept(this->_serverSocket, &addr, &addrlen);

// 	if (clientSocket == -1)
// 		throw HttpServer::AcceptConnectionException();
	
// 	// Set client socket to non-blocking
// 	fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
// 	for (int i = 0; i != this->_nfds; ++i) {
// 		if (this->_fds[i].fd == clientSocket) {
// 			std::cout << "DOUBLE!!\n";
// 			exit(1);				
// 		}
// 	}

// 	int unusedPoll = findUnusedPoll(this->_fds, this->_nfds);
// 	if (unusedPoll != this->_nfds) {
// 		this->_fds[unusedPoll].fd = clientSocket;
// 	} else {
// 		this->_fds[this->_nfds].fd = clientSocket;
// 		this->_fds[this->_nfds].events = POLLIN | POLLOUT;
// 		this->_nfds++;
// 	}
// 	std::string clientIpStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
//     std::string clientPortStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
// 	std::cout << "[" << clientIpStr << ":" << clientPortStr << "]: new clientsocket: " << clientSocket << std::endl;
// 	this->_clients.insert(std::make_pair(clientSocket, HttpStatic(this->serverConfigs, clientIpStr, clientPortStr)));
// }


void	HttpServer::handleAccept()
{
	while (true) {
		// std::cout << "client accept loop: ";
		//Accept all incoming connections
		struct sockaddr	addr;
		socklen_t   	addrlen = sizeof(addr);
		int	clientSocket = accept(this->_serverSocket, &addr, &addrlen);

		if (clientSocket == -1) {
			// std::cout << "unable to accept client Connection: " << strerror(errno) << std::endl;
			return ;
		}
		
		// Set client socket to non-blocking
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
			std::cout << "unable to set clientSocket non blocking" << std::endl;
			return ;
		}
		
		int unusedPoll = findUnusedPoll(this->_fds, this->_nfds);
		if (unusedPoll != this->_nfds) {
			this->_fds[unusedPoll].fd = clientSocket;
		} else {
			this->_fds[this->_nfds].fd = clientSocket;
			this->_fds[this->_nfds].events = POLLIN | POLLOUT;
			this->_nfds++;
		}
		std::string clientIpStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
		std::string clientPortStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
		std::cout << "[" << clientIpStr << ":" << clientPortStr << "]: new clientsocket: " << clientSocket << std::endl;
		this->_clients.insert(std::make_pair(clientSocket, HttpStatic(this->serverConfigs, clientIpStr, clientPortStr)));
	}
}


std::string	getPollEventString(short event)
{
	std::string str;
	if (event & POLLIN)
		str += " POLLIN ";
	if (event & POLLOUT)
		str += " POLLOUT ";
	if (event & POLLERR)
		str += " POLLRR ";
	if (event & POLLHUP)
		str += " POLLHUP ";
	if (event & POLLNVAL)
		str += " POLLNVAL ";
	if (event & POLLPRI)
		str += " POLLPRI ";
	if (event & POLLRDBAND)
		str += " POLLRDBAND ";
	if (event & POLLRDNORM)
		str += " POLLRDNORM ";
	if (event & POLLWRBAND)
		str += " POLLWRBAND ";
	if (event & POLLWRNORM)
		str += " POLLWRNORM ";
	return (str);
}

// check the pollFd Array for events on a Filedescriptor
void	HttpServer::start(void)
{
	// Initialize poll function
	// std::cout << "\n\n ---- HttpServer Poll ---- " << std::endl;
	int ready = poll(this->_fds, this->_nfds, -1);
	// int ready = poll(this->_fds, this->_nfds, 50);
	// std::cout << "poll return val: " << ready << std::endl; 
	if (ready == -1)
		throw HttpServer::EpollWaitException();

	// Set number of fds to a new variable to enable increamenting nfds freely
	int currSize = this->_nfds;

	//
	for (int i = 0; i < currSize; ++i)
	{
		// std::cout << "POLLARR LOOP - POS: " << i << " | FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
			continue;
		// if (!(this->_fds[i].revents & POLLOUT))
		// 	std::cout << "FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
		if (this->_fds[i].fd == this->_serverSocket) {
			std::cout << "--------- HANDLE ACCEPT -------" << std::endl;
			handleAccept();
			std::cout << "-------------------------------" << std::endl;
		} else {
			if (this->_fds[i].revents & (POLLIN | POLLHUP)) {
				std::cout << "--------- HANDLE READ -------" << std::endl;
				handleRead(this->_fds[i].fd);
				std::cout << "-------------------------------" << std::endl;
			}
			if (this->_fds[i].revents & POLLOUT) {
				// std::cout << "r";
				// std::cout << "--------- HANDLE WRITE -------" << std::endl;
				handleWrite(this->_fds[i].fd);
				// std::cout << "-------------------------------" << std::endl;
			}
		}
	}
}
