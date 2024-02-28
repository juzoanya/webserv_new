/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:20 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 22:59:11 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpServer::HttpServer( WsIpPort const & ipPort, Polling & polling )
 : APollEventHandler(polling, false), serverIpPort(ipPort)
{
	int serversocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serversocket == -1)
		throw std::runtime_error(std::string("server socket: socket: ") + strerror(errno));


	// reuse the ip:port socket
	int set = 1;
	if (setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1) {
		close(serversocket);
		throw std::runtime_error(std::string("server socket: set socket reuse address: ") + strerror(errno));
	}
	if (fcntl(serversocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
		close(serversocket);
		throw std::runtime_error(std::string("server socket: set socket non blocking: ") + strerror(errno));
	}
	if (bind(serversocket, (struct sockaddr *)&ipPort.getSockaddrIn(), sizeof(struct sockaddr)) == -1) {
		close(serversocket);
		throw std::runtime_error(std::string("server socket: bind: ")  + strerror(errno));
	}
	if (listen(serversocket, 100) == -1) {
		close(serversocket);
		throw std::runtime_error(std::string("server socket: listen: ")  + strerror(errno));
	}
	_polling.startMonitoringFd(serversocket, POLLIN, this, false);
}


HttpServer::~HttpServer( void )
{ }

void    HttpServer::addServerConfig( ConfigParser::ServerContext const & serverContext )
{
	this->_serverConfigs.push_back(&serverContext);
}

std::vector<ConfigParser::ServerContext const *> const & HttpServer::getServerConfigs( void ) const
{
	return (this->_serverConfigs);
}

HttpConfig HttpServer::getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader )
{
	if (this->_serverConfigs.size() == 0)
		return (HttpConfig(NULL, NULL, pathDecoded, ""));
	std::string hostname = hostHeader.substr(0, hostHeader.find(':'));
	ConfigParser::ServerContext const * currConfig = this->_serverConfigs[0];
	for (std::size_t i = 0; i != this->_serverConfigs.size(); ++i) {
		ws_config_t::const_iterator it = this->_serverConfigs[i]->serverConfig.find("server_name");
		if (it == this->_serverConfigs[i]->serverConfig.end())
			continue ;
		std::vector<std::string>::const_iterator itServerName = std::find(it->second.begin(), it->second.end(), hostname);
		if (itServerName != it->second.end()) {
			currConfig = this->_serverConfigs[i];
			break ;
		}
	}
	ws_config_t const* currLoc = NULL;
	std::size_t selectedLocSize = 0;
	for (std::size_t i = 0 ; i != currConfig->locationConfig.size(); ++i) {
		ws_config_t::const_iterator it = currConfig->locationConfig[i].find("location");
		if (it == currConfig->locationConfig[i].end() || it->second.size() == 0)
			continue;
		std::string const &  location = (it->second.size() == 2 && it->second.at(0) == "=") ? it->second.at(1) : it->second.at(0);
		bool                 isAbs    = (it->second.size() == 2 && it->second.at(0) == "=") ? true : false;
		if (isAbs && location == pathDecoded) {
			return (HttpConfig(&currConfig->serverConfig, &currConfig->locationConfig[i], pathDecoded,  hostname));
		} else if (pathDecoded.find(location) == 0 && (!currLoc || location.size() > selectedLocSize)) {
			currLoc = &currConfig->locationConfig[i];
			selectedLocSize = location.size();
		}
	}
	return (HttpConfig(&currConfig->serverConfig, currLoc, pathDecoded, hostname));
}

void HttpServer::handleEvent( struct pollfd pollfd )
{
	if (!(pollfd.revents & POLLIN))
		return ;
	try {
		while (true) {
			struct sockaddr	addr;
			socklen_t		addrlen = sizeof(addr);
			int	clientSocket = accept(pollfd.fd, &addr, &addrlen);
			if (clientSocket == -1)
			{
				if (errno == EWOULDBLOCK)
					return ;
				throw std::runtime_error(std::string("client socket: accept: ") + strerror(errno));
			}
			if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
				close(clientSocket);
				throw std::runtime_error(std::string("client socket: make socket non blocking: ") + strerror(errno));
			}
			WsIpPort ipPortData(*reinterpret_cast<struct sockaddr_in*>(&addr));
			HttpHandler *newHandler = new HttpHandler(ipPortData, _polling, *this);
			_polling.startMonitoringFd(clientSocket, POLLIN | POLLOUT, newHandler, true);
		}
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}

long    HttpServer::operator()( std::string const & host, std::string const & path )
{
	return (getHttpConfig(path, host).getMaxBodySize());
}
