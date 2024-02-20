/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 12:09:36 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 11:53:27 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientData.hpp"

// IpPortData    setIpPort(struct sockaddr_in const & addr, std::string& ipString, std::string& portString)
// {
//     uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
//     std::stringstream ss;
//     ss << ((ip_addr >> 24) & 0xFF) << ".";
//     ss << ((ip_addr >> 16) & 0xFF) << ".";
//     ss << ((ip_addr >> 8) & 0xFF) << ".";
//     ss << (ip_addr & 0xFF);
//     ipString = ss.str();
//     ss.clear();
//     ss << htons(addr.sin_port);
//     portString = ss.str();
// }

// IpPortData::IpPortData( std::string const & ip, std::string const & port ) : ipString(ip), portString(port)
// { }

// IpPortData::~IpPortData( void )
// { }


Content::Content( void ) : data(NULL), size(0)
{ }

Content::Content( const char* dataPtr, int dataSize ) : data(dataPtr), size(dataSize)
{ }

Content::~Content( void )
{ }


HttpServerData::HttpServerData( struct sockaddr_in const & addr, ConfigParser::ServerContext& serverConf ) : serverConfig(serverConf)
{
    // setIpPort(addr, this->ipString, this->portString);
}

HttpServerData::~HttpServerData( void )
{ }


HttpClientData::HttpClientData( struct sockaddr_in const & addr, HttpServerData& servData ) : serverData(servData), toSend(false)
{
    // setIpPort(addr, this->ipString, this->portString);
}

HttpClientData::~HttpClientData( void )
{ }

Content*    HttpClientData::getContent( void )
{
    
}




// HttpData::HttpData( int kq ) : _kq(kq)
// {}

// HttpData::~HttpData( void )
// {}


// int HttpData::addServerData( ConfigParser::ServerContext& serverConfig, int backlog )
// {
//     ws_config_t::iterator it = serverConfig.serverConfig.find("listen");
//     if (it == serverConfig.serverConfig.end()) {
//         std::cout << "no listening host:port for this server" << std::endl;
//         return (-1);
//     }
//     std::size_t pos = it->second[0].find(':');
//     std::string host = pos == std::string::npos ? "0.0.0.0" : it->second[0].substr(0, pos);
//     std::string port = pos == std::string::npos ? it->second[0] : it->second[0].substr(pos + 1, std::string::npos);
//     struct addrinfo hints;
//     struct addrinfo *res = NULL;
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_protocol = IPPROTO_TCP;
//     int serversocket = -1, err = 0;
//     if ((err = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
//         std::cout << "error: getaddrinfo: " << gai_strerror(err) << std::endl;
//     if (err == 0 && (serversocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
//         std::cout << "error: socket: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0 && (err = bind(serversocket, res->ai_addr, res->ai_addrlen)) == -1)
//         std::cout << "error: bind: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0  && (err = listen(serversocket, backlog)) == -1)
//         std::cout << "error: listen: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0  && (err = fcntl(serversocket, F_SETFL, O_NONBLOCK)) == -1)
//         std::cout << "error: fcntl: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0) {
//         struct kevent kev;
//         EV_SET(&kev, serversocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         if (kevent(this->_kq, &kev, 1, NULL, 0, NULL) == -1)
//             std::cout << "error: kqueue: " << strerror(errno) << std::endl;
//         else
//             this->_serverData.insert(std::make_pair(serversocket, HttpServerData(*reinterpret_cast<struct sockaddr_in*>(res->ai_addr), serverConfig)));
//     }
//     if (res)
//         freeaddrinfo(res);
//     if (serversocket > 0 && err != 0)
//         close(serversocket);
// }


// int HttpData::addServerData( ConfigParser::ServerContext& serverConfig, int backlog )
// {
//     ws_config_t::iterator it = serverConfig.serverConfig.find("listen");
//     if (it == serverConfig.serverConfig.end()) {
//         std::cout << "no listening host:port for this server" << std::endl;
//         return (-1);
//     }
//     std::size_t pos = it->second[0].find(':');
//     std::string host = pos == std::string::npos ? "0.0.0.0" : it->second[0].substr(0, pos);
//     std::string port = pos == std::string::npos ? it->second[0] : it->second[0].substr(pos + 1, std::string::npos);
//     std::cout << "create server: " << host << ":" << port << std::endl;
//     struct addrinfo hints;
//     struct addrinfo *res = NULL;
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_protocol = IPPROTO_TCP;
//     int serversocket = -1, err = 0;
//     if ((err = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0)
//         std::cout << "error: getaddrinfo: " << gai_strerror(err) << std::endl;
//     if (err == 0 && (serversocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
//         std::cout << "error: socket: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0 && (err = bind(serversocket, res->ai_addr, res->ai_addrlen)) == -1)
//         std::cout << "error: bind: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0  && (err = listen(serversocket, backlog)) == -1)
//         std::cout << "error: listen: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0  && (err = fcntl(serversocket, F_SETFL, O_NONBLOCK)) == -1)
//         std::cout << "error: fcntl: " << strerror(errno) << std::endl;
//     if (serversocket > 0 && err == 0) {
//         struct kevent kev;
//         EV_SET(&kev, serversocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         if (kevent(this->_kq, &kev, 1, NULL, 0, NULL) == -1) {
//             std::cout << "error: kqueue: " << strerror(errno) << std::endl;
//         } else {
//             this->_serverData.insert(std::make_pair(serversocket, HttpServerData(*reinterpret_cast<struct sockaddr_in*>(res->ai_addr), serverConfig)));
//             std::cout << "created" << std::endl;
//         }
//     }
//     if (res)
//         freeaddrinfo(res);
//     if (serversocket > 0 && err != 0)
//         close(serversocket);
// }

// int HttpData::addClientData( int serversockfd )
// {
//    std::map<int, HttpServerData>::iterator findIt = this->_serverData.find(serversockfd);
//     if (findIt == this->_serverData.end()) {
//         std::cout << "serverData not Found" << std::endl;
//         return (-1);
//     }
//     HttpServerData& currServerData = findIt->second;
//     std::cout << "Server: new Connection: [" << serversockfd << ", " << currServerData.ipString << ":" << currServerData.portString << "]: ";
//     struct sockaddr addr;
//     socklen_t       addrlen = sizeof(addr);
//     int clientsocket = -1, err = 0, set = 1;
//     if ((clientsocket = accept(serversockfd, &addr, &addrlen)) == -1)
//         std::cout << "error: accept: " << strerror(errno) << std::endl;
// #ifdef __APPLE__
//     if (clientsocket > 0 && (err = setsockopt(clientsocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int))) == -1)
//         std::cout << "error: setsockopt: " << strerror(errno) << std::endl;
// #endif
//     if (clientsocket > 0 && err == 0 && (err = fcntl(clientsocket, F_SETFL, O_NONBLOCK)) == -1)
//         std::cout << "error: fcntl: " << strerror(errno) << std::endl;
//     if (clientsocket > 0 && err == 0) {
//         struct kevent kev[2];
//         EV_SET(kev, clientsocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         EV_SET(kev + 1, clientsocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
//         if (kevent(this->_kq, kev, 2, NULL, 0, NULL) == -1)
//             std::cout << "error: kevent: " << strerror(errno) << std::endl;
//         else {
//             this->_clientData.insert(std::make_pair(clientsocket, HttpClientData(*reinterpret_cast<struct sockaddr_in*>(&addr), findIt->second)));
//         }
//     }
//     if (clientsocket > 0 && err != 0)
//         close(serversocket);
//     if (clientsocket == -1 || err != 0)
//         return (-1);
//     return (clientsocket);
// }
