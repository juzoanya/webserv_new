/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 13:20:20 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 09:58:35 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpServer::HttpServer( WsIpPort const & ipPort, Polling & polling )
 : APollEventHandler(polling, false), serverIpPort(ipPort)
{
    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serversocket == -1)
		throw HttpServer::CreatingServerSocketException();

	// reuse the ip:port socket
	int set = 1;
	if (setsockopt(serversocket, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1)
        throw std::runtime_error("setsockopt reuse addr error");

	// Setup server address structure
	// fillSockAddrIn(this->_serverAddress, ip, port);

	// Set the server socket to non-blocking mode
	fcntl(serversocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	
	// Bind the server socket to the specified address and port
	if (bind(serversocket, (struct sockaddr *)&ipPort.getSockaddrIn(), sizeof(struct sockaddr)) == -1)
		throw HttpServer::BindSocketServerException();

	// Listen for incoming connection
	if (listen(serversocket, 10) == -1)
		throw HttpServer::ListeningForConnectionException();
	
    _polling.startMonitoringFd(serversocket, POLLIN, this);
	// this->startMonitoring(this->_serverSocket, POLLIN, NULL);
}


// HttpServer::HttpServer( WsIpPort const & ipPort, Polling & polling, ConfigParser::ServerContext const & serverContext )
//  : APollEventHandler(polling), serverIpPort(ipPort), HttpServer(ipPort, polling)
// {
//     addServerConfig(serverContext);
// }

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
    std::cout << "\n---- getHttpConfig ----" << std::endl;
    if (this->_serverConfigs.size() == 0)
        return (HttpConfig(NULL, NULL, pathDecoded, ""));
    std::string hostname = hostHeader.substr(0, hostHeader.find(':'));
    std::cout << "hostname: " << hostname << std::endl;
    std::cout << "hostheader: " << hostHeader << std::endl;
    std::cout << "pathDecoded: " << pathDecoded << std::endl;
    ConfigParser::ServerContext const * currConfig = this->_serverConfigs[0];
    std::cout << "\n------- select server by hostname <-> server_name -------" << std::endl;
    for (std::size_t i = 0; i != this->_serverConfigs.size(); ++i) {

        debPrintServerContext("curr context at pos: " + toStr(i) + ": ", *this->_serverConfigs[i], false);
        debPrintConfigSelectedDirective("server_names of serverContext at index: " + toStr(i) + ": ", "server_name",  this->_serverConfigs[i]->serverConfig, 0);
        
        ws_config_t::const_iterator it = this->_serverConfigs[i]->serverConfig.find("server_name");
        if (it == this->_serverConfigs[i]->serverConfig.end())
            continue ;
        std::vector<std::string>::const_iterator itServerName = std::find(it->second.begin(), it->second.end(), hostname);
        if (itServerName != it->second.end()) {
            std::cout << "---> servername found: " << *itServerName << std::endl;
            currConfig = this->_serverConfigs[i];
            break ;
        }
    }
    std::cout << "\n------- select location by requestPath -------" << std::endl;
    ws_config_t const* currLoc = NULL;
    std::size_t selectedLocSize = 0;
    for (std::size_t i = 0 ; i != currConfig->locationConfig.size(); ++i) {
        ws_config_t::const_iterator it = currConfig->locationConfig[i].find("location");
        if (it == currConfig->locationConfig[i].end() || it->second.size() == 0)
            continue;
        std::string const &  location = (it->second.size() == 2 && it->second.at(0) == "=") ? it->second.at(1) : it->second.at(0);
        bool                 isAbs    = (it->second.size() == 2 && it->second.at(0) == "=") ? true : false;
        debPrintStrVector("loction: ", it->second, 0);
        if (isAbs && location == pathDecoded) {
            std::cout << "-> location is EQUAL pathDecoded : " << location << std::endl;
            return (HttpConfig(&currConfig->serverConfig, &currConfig->locationConfig[i], pathDecoded,  hostname));
        } else if (pathDecoded.find(location) == 0 && (!currLoc || location.size() > selectedLocSize)) {
            currLoc = &currConfig->locationConfig[i];
            selectedLocSize = location.size();
            std::cout << "-> selected location: " << location << std::endl;
        }
    }
    std::cout << "-----------------------------------" << std::endl;
    return (HttpConfig(&currConfig->serverConfig, currLoc, pathDecoded, hostname));
}

void HttpServer::handleEvent( struct pollfd & pollfd )
{
    std::cout << "HttpServer event: fd: " << pollfd.fd << " | events: " << pollfd.events << " | revents: " << pollfd.revents << std::endl;
    if (pollfd.revents & POLLIN) {
        struct sockaddr	addr;
		socklen_t   	addrlen = sizeof(addr);
		int	clientSocket = accept(pollfd.fd, &addr, &addrlen);
		if (clientSocket == -1)
			throw HttpServer::AcceptConnectionException();
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
			close(clientSocket);
			throw HttpServer::SetSocketNonBlockingexception();
		}
		WsIpPort ipPortData(*reinterpret_cast<struct sockaddr_in*>(&addr));
		std::cout << "[" << ipPortData.getIpStr() << ":" << ipPortData.getPortStr() << "]: new clientsocket: " << clientSocket << std::endl;
        HttpHandler *newHandler = new HttpHandler(ipPortData, _polling, *this);
		_polling.startMonitoringFd(clientSocket, POLLIN | POLLOUT, newHandler);
    }
}

long    HttpServer::operator()( std::string const & host, std::string const & path )
{
    return (getHttpConfig(path, host).getMaxBodySize());
}
