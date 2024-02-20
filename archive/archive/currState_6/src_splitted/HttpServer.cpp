/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServ_new.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:12:12 by juzoanya          #+#    #+#             */
/*   Updated: 2024/02/03 11:22:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

HttpServ::HttpServ( WsIpPort const & ipPort )
 : _serverSocket(-1), _readBuffer(4096), serverConfigs(ipPort)
{
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw HttpServ::CreatingServerSocketException();

	// reuse the ip:port socket
	int set = 1;
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1)
        throw std::runtime_error("setsockopt reuse addr error");

	// Setup server address structure
	// fillSockAddrIn(this->_serverAddress, ip, port);

	// Set the server socket to non-blocking mode
	fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
	// Bind the server socket to the specified address and port
	if (bind(this->_serverSocket, (struct sockaddr *)&this->serverConfigs.ipPortData.getSockaddrIn(), sizeof(struct sockaddr)) == -1)
		throw HttpServ::BindSocketServerException();

	// Listen for incoming connection
	if (listen(this->_serverSocket, 10) == -1)
		throw HttpServ::ListeningForConnectionException();
	
	this->addMonitoring(this->_serverSocket, POLLIN, NULL);
}

HttpServ::~HttpServ()
{ }

int	HttpServ::addMonitoring( int fd, short events, AHttpHandler* data, int otherIndex )
{
	std::size_t i = 0;
	while (i != this->_pollFds.size() && this->_pollFds[i].fd != -1)
		i++;
	if (i == this->_pollFds.size()) {
		this->_pollFds.push_back(pollfd());
		this->_clients.push_back(NULL);
		this->_otherIndex.push_back(otherIndex);
	}
	std::cout << "add Monitoring: " << "fd: " << fd << " | index: " << i << " | other index: " << otherIndex << std::endl;
	this->_pollFds[i].fd = fd;
	this->_pollFds[i].events = events;
	this->_pollFds[i].revents = 0;
	this->_clients[i] = data;
	this->_otherIndex[i] = otherIndex;
	return (i);
}

bool	HttpServ::stopMonitoring( std::size_t index )
{
	std::cout << "stop monitoring at intex: " << index << " | pollFd vec size: " << this->_pollFds.size() << " | clients vec size: " << this->_clients.size() << std::endl;
	if (index >= 0 && index < this->_pollFds.size() && index < this->_clients.size() && this->_pollFds[index].fd != -1) {
		close(this->_pollFds[index].fd);
		this->_pollFds[index].fd = -1;
		if (this->_clients[index]) {
			delete this->_clients[index];
			this->_clients[index] = NULL;
		}
		if (this->_otherIndex[index] != -1)
			stopMonitoring(this->_otherIndex[index]);
		return (true);
	}
	return (false);
}

int	HttpServ::handleAccept( void )
{
	while (true) {
		struct sockaddr	addr;
		socklen_t   	addrlen = sizeof(addr);
		int	clientSocket = accept(this->_serverSocket, &addr, &addrlen);
		if (clientSocket == -1)
			return (-1);
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
			close(clientSocket);
			return (-1);
		}
		WsIpPort ipPortData(*reinterpret_cast<struct sockaddr_in*>(&addr));
		this->addMonitoring(clientSocket, POLLIN | POLLOUT, new HttpHandler(this->serverConfigs, ipPortData));
		std::cout << "[" << ipPortData.getIpStr() << ":" << ipPortData.getPortStr() << "]: new clientsocket: " << clientSocket << std::endl;
	}
	return (0);
}

void	HttpServ::handleRead( std::size_t index )
{
	ssize_t readByte = recv(this->_pollFds[index].fd, this->_readBuffer.data(), this->_readBuffer.size(), 0);
	if (readByte == -1) {
		stopMonitoring(index);
		throw HttpServ::ClientSocketReadException();
	} else if (readByte == 0) {
		stopMonitoring(index);
	} else {
		ws_http::statuscodes_t status = this->_clients[index]->handleData(this->_readBuffer.begin(), this->_readBuffer.begin() + readByte);
		if (status == ws_http::STATUS_1001_INTERNAL_PROCESS_CGI) {
			HttpHandler::CgiHandler *cgiHandler = dynamic_cast<HttpHandler*>(this->_clients[index])->getCgiProcessInstance();
			std::cout << "create child process\n";
			int cgiFd = cgiHandler->createChildProcess();
			if (cgiFd == -1) {
				std::cout << "cgiFd == -1\n";
				stopMonitoring(index);
			} else {
				std::cout << "cgi ok, add monitoring\n";
				this->_otherIndex[index] = addMonitoring(cgiFd, POLLIN | POLLOUT, cgiHandler, index);
			}
		}
	}
}

void	HttpServ::handleWrite( std::size_t index )
{
	int sendres = this->_clients[index]->httpContent.sendDataToSocket(this->_pollFds[index].fd, 0);
	if (sendres == -1) {
		stopMonitoring(index);
		throw HttpServ::ClientSocketWriteException();
	} else if (sendres == 0) {
		std::cout << "clientSocket: " << this->_pollFds[index].fd << " closed the connection" << std::endl;
		stopMonitoring(index);
	}
}

void	HttpServ::runPoll( void )
{
	int ready = poll(this->_pollFds.data(), this->_pollFds.size(), -1);
	if (ready == -1)
		throw HttpServ::EpollWaitException();
	if (this->_pollFds[0].revents & POLLIN) {
		std::cout << "---------- HANDLE ACCEPT ----------" << std::endl;
		handleAccept();
		std::cout << "---------------------------------" << std::endl;
	}
	std::size_t currSize = this->_pollFds.size();
	for (std::size_t i = 1; i < currSize; ++i)
	{
		if (this->_pollFds[i].revents == 0 || this->_pollFds[i].fd == -1)
			continue;
		if (this->_pollFds[i].revents & (POLLIN | POLLHUP) && this->_clients[i]) {
			std::cout << "---------- HANDLE READ ----------" << std::endl;
			handleRead(i);
			std::cout << "---------------------------------" << std::endl;
		}
		if (this->_pollFds[i].revents & POLLOUT && this->_clients[i] && !this->_clients[i]->httpContent.empty()) {
			handleWrite(i);
		}
	}
}



// void	HttpServ::handleWrite( std::size_t index )
// {
// 	if (!this->_clients[index] || index >= this->_pollFds.size() || index >= this->_clients.size()) {
// 		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
// 		return ;
// 	}
// 	if (this->_clients[index]->dataSize == 0 && this->_clients[index]->headerSize == 0) {
// 		// std::cout << "dataSize is 0!!\n";
// 		return ;
// 	}
// 	std::size_t writeByte = send(this->_pollFds[index].fd, this->_clients[index]->dataPtr, this->_clients[index]->dataSize, 0);
// 	std::cout << writeByte << " Bytes of data written to clientSocket: " << this->_pollFds[index].fd << std::endl;
// 	if (writeByte == -1) {
// 		stopMonitoring(index);
// 		throw HttpServ::ClientSocketWriteException();
// 	} else if (writeByte == 0) {
// 		std::cout << "clientSocket: " << this->_pollFds[index].fd << " closed the connection" << std::endl;
// 		stopMonitoring(index);
// 	} else {
// 		this->_clients[index]->dataPtr += writeByte;
// 		this->_clients[index]->dataSize -= writeByte;
// 		std::cout << "new dataSize: " << this->_clients[index]->dataSize << std::endl;
// 		if (this->_clients[index]->dataSize == 0)
// 			stopMonitoring(index);
// 	}
// }

// void	HttpServ::handleWrite( std::size_t index )
// {
// 	if (!this->_clients[index] || index >= this->_pollFds.size() || index >= this->_clients.size()) {
// 		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
// 		return ;
// 	}
// 	if (this->_clients[index]->dataSize == 0 && this->_clients[index]->headerSize == 0) {
// 		// std::cout << "dataSize is 0!!\n";
// 		return ;
// 	}
// 	std::size_t writeByte = send(this->_pollFds[index].fd, this->_clients[index]->dataPtr, this->_clients[index]->dataSize, 0);
// 	std::cout << writeByte << " Bytes of data written to clientSocket: " << this->_pollFds[index].fd << std::endl;
// 	if (writeByte == -1) {
// 		stopMonitoring(index);
// 		throw HttpServ::ClientSocketWriteException();
// 	} else if (writeByte == 0) {
// 		std::cout << "clientSocket: " << this->_pollFds[index].fd << " closed the connection" << std::endl;
// 		stopMonitoring(index);
// 	} else {
// 		this->_clients[index]->dataPtr += writeByte;
// 		this->_clients[index]->dataSize -= writeByte;
// 		std::cout << "new dataSize: " << this->_clients[index]->dataSize << std::endl;
// 		if (this->_clients[index]->dataSize == 0)
// 			stopMonitoring(index);
// 	}
// }


// void	HttpServ::runPoll( void )
// {
// 	int ready = poll(this->_pollFds.data(), this->_pollFds.size(), -1);
// 	if (ready == -1)
// 		throw HttpServ::EpollWaitException();
// 	if (this->_pollFds[0].revents & POLLIN) {
// 		std::cout << "---------- HANDLE ACCEPT ----------" << std::endl;
// 		handleAccept();
// 		std::cout << "---------------------------------" << std::endl;
// 	}
// 	std::size_t currSize = this->_pollFds.size();
// 	for (std::size_t i = 1; i < currSize; ++i)
// 	{
// 		if (this->_pollFds[i].revents == 0 || this->_pollFds[i].fd == -1)
// 			continue;
// 		if (this->_pollFds[i].revents & (POLLIN | POLLHUP) && this->_clients[i]) {
// 			std::cout << "---------- HANDLE READ ----------" << std::endl;
// 			handleRead(i);
// 			std::cout << "---------------------------------" << std::endl;
// 		}
// 		if (this->_pollFds[i].revents & POLLOUT && this->_clients[i] && !this->_clients[i]->dataContent.empty()) {
// 			// std::cout << "---------- HANDLE WRITE ----------" << std::endl;
// 			int sendres = this->_clients[i]->dataContent.sendDataToSocket(this->_pollFds[i].fd, 0);
// 			if (sendres == -1) {
// 				stopMonitoring(i);
// 				throw HttpServ::ClientSocketWriteException();
// 			} else if (sendres == 0) {
// 				std::cout << "clientSocket: " << this->_pollFds[i].fd << " closed the connection" << std::endl;
// 				stopMonitoring(i);
// 			}

// 			// if (this->_clients[i]->headerSize > 0)
// 			// 	handleWrite(i);
// 			// else if (this->_clients[i]->dataSize > 0)
// 			// 	handleWrite(i);
// 			// if (this->_clients[i]->dataSize == 0 && this->_clients[i]->headerSize == 0)
// 			// 	stopMonitoring(i);



// 			// std::cout << "---------------------------------" << std::endl;
// 		}
// 	}
// }



// std::string	getPollEventString(short event)
// {
// 	std::string str;
// 	if (event & POLLIN)
// 		str += " POLLIN ";
// 	if (event & POLLOUT)
// 		str += " POLLOUT ";
// 	if (event & POLLERR)
// 		str += " POLLRR ";
// 	if (event & POLLHUP)
// 		str += " POLLHUP ";
// 	if (event & POLLNVAL)
// 		str += " POLLNVAL ";
// 	if (event & POLLPRI)
// 		str += " POLLPRI ";
// 	if (event & POLLRDBAND)
// 		str += " POLLRDBAND ";
// 	if (event & POLLRDNORM)
// 		str += " POLLRDNORM ";
// 	if (event & POLLWRBAND)
// 		str += " POLLWRBAND ";
// 	if (event & POLLWRNORM)
// 		str += " POLLWRNORM ";
// 	return (str);
// }

// // check the pollFd Array for events on a Filedescriptor
// void	HttpServ::start(void)
// {
// 	int ready = poll(this->_fds, this->_nfds, -1);
// 	if (ready == -1)
// 		throw HttpServ::EpollWaitException();


// 	if (this->_fds[0].revents & POLLIN) {
// 		std::cout << "--------- HANDLE ACCEPT -------" << std::endl;
// 		handleAccept();
// 		std::cout << "-------------------------------" << std::endl;
// 		return ;
// 	}
	
// 	// Set number of fds to a new variable to enable increamenting nfds freely
// 	int currSize = this->_nfds;

// 	for (int i = 1; i < currSize; ++i)
// 	{
// 		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
// 			continue;
// 		if (this->_fds[i].revents & (POLLIN | POLLHUP)) {
// 			std::cout << "--------- HANDLE READ -------" << std::endl;
// 			handleRead(this->_fds[i].fd, i);
// 			std::cout << "-------------------------------" << std::endl;
// 		}
// 		if (this->_fds[i].revents & POLLOUT) {
// 			// std::cout << "--------- HANDLE WRITE -------" << std::endl;
// 			handleWrite(this->_fds[i].fd, i);
// 			// std::cout << "-------------------------------" << std::endl;
// 		}
// 	}
// }




// // check the pollFd Array for events on a Filedescriptor
// void	HttpServ::start(void)
// {
// 	// Initialize poll function
// 	// std::cout << "\n\n ---- HttpServ Poll ---- " << std::endl;
// 	int ready = poll(this->_fds, this->_nfds, -1);
// 	// int ready = poll(this->_fds, this->_nfds, 50);
// 	// std::cout << "poll return val: " << ready << std::endl; 
// 	if (ready == -1)
// 		throw HttpServ::EpollWaitException();


// 	if (this->_fds[0].revents & POLLIN)
	
// 	// Set number of fds to a new variable to enable increamenting nfds freely
// 	int currSize = this->_nfds;

// 	//
// 	for (int i = 0; i < currSize; ++i)
// 	{
// 		// std::cout << "POLLARR LOOP - POS: " << i << " | FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
// 		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
// 			continue;
// 		// if (!(this->_fds[i].revents & POLLOUT))
// 		// 	std::cout << "FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
// 		if (this->_fds[i].fd == this->_serverSocket) {
// 			std::cout << "--------- HANDLE ACCEPT -------" << std::endl;
// 			handleAccept();
// 			std::cout << "-------------------------------" << std::endl;
// 		} else {
// 			if (this->_fds[i].revents & (POLLIN | POLLHUP)) {
// 				std::cout << "--------- HANDLE READ -------" << std::endl;
// 				handleRead(this->_fds[i].fd);
// 				std::cout << "-------------------------------" << std::endl;
// 			}
// 			if (this->_fds[i].revents & POLLOUT) {
// 				// std::cout << "r";
// 				// std::cout << "--------- HANDLE WRITE -------" << std::endl;
// 				handleWrite(this->_fds[i].fd);
// 				// std::cout << "-------------------------------" << std::endl;
// 			}
// 		}
// 	}
// }




// ServerWasCreated::ServerWasCreated( ws_config_t& serverConfig )
// {
// 	ws_config_t::iterator it = serverConfig.find("listen");
// 	std::size_t pos = std::string::npos;
// 	if (it != serverConfig.end())
// 		pos = it->second[0].find(':');
// 	if (it == serverConfig.end() || it->second.size() == 0) {
// 		this->ipStr = "0.0.0.0";
// 		this->portStr = "8080";
// 	} else if (pos == std::string::npos) {
// 		this->ipStr = "0.0.0.0";
// 		this->portStr = it->second[0];
// 	} else {
// 		this->ipStr = it->second[0].substr(0, pos);
// 		this->portStr = it->second[0].substr(pos + 1, std::string::npos);
// 	}
// }

// ServerWasCreated::~ServerWasCreated( void )
// { }

// bool ServerWasCreated::operator()( HttpServ const & server )
// {
// 	if (server.serverConfigs.serverIp == this->ipStr && server.serverConfigs.serverPort == this->portStr)
// 		return (true);
// 	return (false);
// }


// std::string getPortStr(struct sockaddr_in const & addr)
// {
//     std::stringstream ss;
//     ss << htons(addr.sin_port);
//     return (ss.str());
// }

// std::string getIpStr(struct sockaddr_in const & addr)
// {
//     uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
//     std::stringstream ss;
//     ss << ((ip_addr >> 24) & 0xFF) << ".";
//     ss << ((ip_addr >> 16) & 0xFF) << ".";
//     ss << ((ip_addr >> 8) & 0xFF) << ".";
//     ss << (ip_addr & 0xFF);
//     return (ss.str());
// }

// void  fillSockAddrIn(struct sockaddr_in& addr, std::string const & ipStr, std::string const & portStr)
// {
//     memset(&addr, 0, sizeof(struct sockaddr_in));
// 	uint32_t ip = 0;
// 	unsigned int ipPartNbr;
//     // std::size_t pos_start = 0, pos = 0;
//     const char* ipPtr = ipStr.data();
//     char* rest;
// 	for (int i = 0; i != 4; ++i) {
//         ipPartNbr = strtol(ipPtr, &rest, 10);
//         if ((i == 3 && *rest != 0) || (i < 3 && *rest != '.') || (i < 3 && !std::isdigit(*(rest + 1))))
//             throw::std::runtime_error("invalid ip address");
//         ipPtr = rest + 1;
// 		ip |= (ipPartNbr << (i * 8));
// 	}
//     unsigned int port = strtol(portStr.c_str(), &rest, 10);
//     if (port == 0 || *rest != 0)
//         throw::std::runtime_error("invalid port");
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = ip;
//     addr.sin_port = htons(port);
// }


// HttpServ::HttpServ(std::string const & ip, std::string const & port)
//  : _serverSocket(-1), _nfds(0), serverConfigs(ip, port)
// {
// 	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (this->_serverSocket == -1)
// 		throw HttpServ::CreatingServerSocketException();

// 	// reuse the ip:port socket
// 	int set = 1;
// 	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1)
//         throw std::runtime_error("setsockopt reuse addr error");

// 	// Setup server address structure
// 	fillSockAddrIn(this->_serverAddress, ip, port);

// 	// Set the server socket to non-blocking mode
// 	fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
// 	// Bind the server socket to the specified address and port
// 	if (bind(this->_serverSocket, (struct sockaddr *)&this->_serverAddress, sizeof(this->_serverAddress)) == -1)
// 		throw HttpServ::BindSocketServerException();

// 	// Listen for incoming connection
// 	if (listen(this->_serverSocket, 10) == -1)
// 		throw HttpServ::ListeningForConnectionException();
	
// 	// Initialize number of file descriptors to 1 and set memory allocation for pollfd structure
// 	this->_nfds = 1;
// 	memset(this->_fds, 0, sizeof(this->_fds));
// 	// Add the server socket to the pollfd array
// 	this->_fds[0].fd = this->_serverSocket;
// 	this->_fds[0].events = POLLIN;
// }

// HttpServ::~HttpServ()
// { }

// // HttpServ::HttpServ(const HttpServ& src) { }

// // HttpServ& HttpServ::operator=(const HttpServ& rhs) { }



// unsigned long getCurrTimeMs( void )
// {
//     struct timespec spec;
//     clock_gettime(CLOCK_MONOTONIC, &spec);
//     return (spec.tv_sec * 1000 + spec.tv_nsec / 1000000);
// }

// // Stop monitoring a file decriptor by removing it from the pollfd array
// void	HttpServ::stopMonitoring(int socketFd, int clientIndex)
// {
// 	std::cout << "--> stop monitoring socket: " << socketFd << " and delete instance of HttpStatic, if clientSocket" << std::endl;
// 	for (int i = 0; i < this->_nfds; ++i)
// 	{
// 		if (this->_fds[i].fd == socketFd) {
// 			close(this->_fds[i].fd);
// 			this->_fds[i].fd = -1;
// 			delete this->_clients[i];
// 			this->_clients[i] = NULL;
// 			return ;
// 		}
// 	}
// }

// void	HttpServ::handleRead(int socketFd, int clientIndex)
// {
// 	buff_t	buffer(4096);

// 	if (!this->_clients[clientIndex]) {
// 		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
// 		return ;
// 	}

// 	ssize_t readByte = recv(socketFd, buffer.data(), buffer.size(), 0);
// 	std::cout << readByte << " Bytes of data recieved from socket: " << socketFd << std::endl;
// 	if (readByte == -1) {
// 		std::cout << "Error reading from client socket." << strerror(errno) << std::endl;
// 		stopMonitoring(socketFd, clientIndex);
// 		throw HttpServ::ClientSocketReadException();
// 	} else if (readByte == 0) {
// 		std::cout << "socket: " << socketFd << " closed his write side, no more data to read" << std::endl;
// 		stopMonitoring(socketFd, clientIndex);
// 	} else {
// 		if (this->_clients[clientIndex]->clientSocket == socketFd) {
// 			this->_clients[clientIndex]->handleData(buffer.begin(), buffer.begin() + readByte);
// 		}
// 	}
// }

// void	HttpServ::handleWrite(int socketFd, int clientIndex)
// {
// 	if (!this->_clients[clientIndex]) {
// 		std::cout << "no HttpStatic instance for this ClientSocket in the Map\n";
// 		return ;
// 	}
// 	if (this->_clients[clientIndex]->dataSize == 0)
// 		return ;

// 	ssize_t writeByte = send(socketFd, this->_clients[clientIndex]->dataPtr, this->_clients[clientIndex]->dataSize, 0);
// 	std::cout << writeByte << " Bytes of data written to clientSocket: " << socketFd << std::endl;
// 	if (writeByte == -1) {
// 		stopMonitoring(socketFd, clientIndex);
// 		throw HttpServ::ClientSocketWriteException();
// 	} else if (writeByte == 0) {
// 		std::cout << "clientSocket: " << socketFd << " closed the connection" << std::endl;
// 		stopMonitoring(socketFd, clientIndex);
// 	} else {
// 		this->_clients[clientIndex]->dataPtr += writeByte;
// 		this->_clients[clientIndex]->dataSize -= writeByte;
// 		if (this->_clients[clientIndex]->dataSize == 0)
// 			stopMonitoring(socketFd, clientIndex);
// 	}
// }


// int	findUnusedPoll(struct pollfd pollArr[], int arrSize)
// {
// 	std::cout << "find unused poll, pollArr Size: " << arrSize << std::endl;
// 	for (int i = 0; i != arrSize; ++i) {
// 		std::cout << "curr fd no: " << pollArr[i].fd << std::endl;
// 		if (pollArr[i].fd == -1)
// 			return (i);
// 	}
// 	return (arrSize);
// }

// void	HttpServ::handleAccept()
// {
// 	while (true) {
// 		// std::cout << "client accept loop: ";
// 		//Accept all incoming connections
// 		struct sockaddr	addr;
// 		socklen_t   	addrlen = sizeof(addr);
// 		int	clientSocket = accept(this->_serverSocket, &addr, &addrlen);

// 		if (clientSocket == -1) {
// 			// std::cout << "unable to accept client Connection: " << strerror(errno) << std::endl;
// 			return ;
// 		}
		
// 		// Set client socket to non-blocking
// 		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
// 			close(clientSocket);
// 			std::cout << "unable to set clientSocket non blocking" << std::endl;
// 			return ;
// 		}
		
// 		std::string clientIpStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
// 		std::string clientPortStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
// 		HttpStatic* newClientData = new HttpStatic(this->serverConfigs, clientIpStr, clientPortStr, clientSocket);

// 		int unusedPoll = findUnusedPoll(this->_fds, this->_nfds);
// 		if (unusedPoll != this->_nfds) {
// 			this->_fds[unusedPoll].fd = clientSocket;
// 			this->_clients[unusedPoll] = newClientData;
// 		} else {
// 			this->_fds[this->_nfds].fd = clientSocket;
// 			this->_fds[this->_nfds].events = POLLIN | POLLOUT;
// 			this->_clients[this->_nfds] = newClientData;
// 			this->_nfds++;
// 		}
// 		std::cout << "[" << clientIpStr << ":" << clientPortStr << "]: new clientsocket: " << clientSocket << std::endl;
// 	}
// }


// std::string	getPollEventString(short event)
// {
// 	std::string str;
// 	if (event & POLLIN)
// 		str += " POLLIN ";
// 	if (event & POLLOUT)
// 		str += " POLLOUT ";
// 	if (event & POLLERR)
// 		str += " POLLRR ";
// 	if (event & POLLHUP)
// 		str += " POLLHUP ";
// 	if (event & POLLNVAL)
// 		str += " POLLNVAL ";
// 	if (event & POLLPRI)
// 		str += " POLLPRI ";
// 	if (event & POLLRDBAND)
// 		str += " POLLRDBAND ";
// 	if (event & POLLRDNORM)
// 		str += " POLLRDNORM ";
// 	if (event & POLLWRBAND)
// 		str += " POLLWRBAND ";
// 	if (event & POLLWRNORM)
// 		str += " POLLWRNORM ";
// 	return (str);
// }



// // check the pollFd Array for events on a Filedescriptor
// void	HttpServ::start(void)
// {
// 	int ready = poll(this->_fds, this->_nfds, -1);
// 	if (ready == -1)
// 		throw HttpServ::EpollWaitException();


// 	if (this->_fds[0].revents & POLLIN) {
// 		std::cout << "--------- HANDLE ACCEPT -------" << std::endl;
// 		handleAccept();
// 		std::cout << "-------------------------------" << std::endl;
// 		return ;
// 	}
	
// 	// Set number of fds to a new variable to enable increamenting nfds freely
// 	int currSize = this->_nfds;

// 	for (int i = 1; i < currSize; ++i)
// 	{
// 		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
// 			continue;
// 		if (this->_fds[i].revents & (POLLIN | POLLHUP)) {
// 			std::cout << "--------- HANDLE READ -------" << std::endl;
// 			handleRead(this->_fds[i].fd, i);
// 			std::cout << "-------------------------------" << std::endl;
// 		}
// 		if (this->_fds[i].revents & POLLOUT) {
// 			// std::cout << "--------- HANDLE WRITE -------" << std::endl;
// 			handleWrite(this->_fds[i].fd, i);
// 			// std::cout << "-------------------------------" << std::endl;
// 		}
// 	}
// }




// // // check the pollFd Array for events on a Filedescriptor
// // void	HttpServ::start(void)
// // {
// // 	// Initialize poll function
// // 	// std::cout << "\n\n ---- HttpServ Poll ---- " << std::endl;
// // 	int ready = poll(this->_fds, this->_nfds, -1);
// // 	// int ready = poll(this->_fds, this->_nfds, 50);
// // 	// std::cout << "poll return val: " << ready << std::endl; 
// // 	if (ready == -1)
// // 		throw HttpServ::EpollWaitException();


// // 	if (this->_fds[0].revents & POLLIN)
	
// // 	// Set number of fds to a new variable to enable increamenting nfds freely
// // 	int currSize = this->_nfds;

// // 	//
// // 	for (int i = 0; i < currSize; ++i)
// // 	{
// // 		// std::cout << "POLLARR LOOP - POS: " << i << " | FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
// // 		if (this->_fds[i].revents == 0 || this->_fds[i].fd == -1)
// // 			continue;
// // 		// if (!(this->_fds[i].revents & POLLOUT))
// // 		// 	std::cout << "FD: " << this->_fds[i].fd << " | current SET: " << std::setw(40) << getPollEventString(this->_fds[i].events) << "\t\tREC: " << std::setw(30) << getPollEventString(this->_fds[i].revents) << std::endl; 
// // 		if (this->_fds[i].fd == this->_serverSocket) {
// // 			std::cout << "--------- HANDLE ACCEPT -------" << std::endl;
// // 			handleAccept();
// // 			std::cout << "-------------------------------" << std::endl;
// // 		} else {
// // 			if (this->_fds[i].revents & (POLLIN | POLLHUP)) {
// // 				std::cout << "--------- HANDLE READ -------" << std::endl;
// // 				handleRead(this->_fds[i].fd);
// // 				std::cout << "-------------------------------" << std::endl;
// // 			}
// // 			if (this->_fds[i].revents & POLLOUT) {
// // 				// std::cout << "r";
// // 				// std::cout << "--------- HANDLE WRITE -------" << std::endl;
// // 				handleWrite(this->_fds[i].fd);
// // 				// std::cout << "-------------------------------" << std::endl;
// // 			}
// // 		}
// // 	}
// // }
