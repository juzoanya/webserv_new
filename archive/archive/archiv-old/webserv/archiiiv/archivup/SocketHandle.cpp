/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandle.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 11:51:23 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 11:38:21 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketHandle.hpp"

void    setIpPort(struct sockaddr_in const & addr, std::string& ipString, std::string& portString)
{
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    std::stringstream ss;
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
    ipString = ss.str();
    ss.clear();
    ss << htons(addr.sin_port);
    portString = ss.str();
}

SocketHandle::SocketHandle( std::string const& host, std::string const& port, int backlog ) : _isServer(true)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int gaierr = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (gaierr != 0)
        throw std::runtime_error(gai_strerror(gaierr));
    memcpy(&hints, res, sizeof(struct addrinfo));
    freeaddrinfo(res);
    int serversocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (serversocket == -1)
        throw std::runtime_error(strerror(errno));
    int err = bind(serversocket, res->ai_addr, res->ai_addrlen);
    if (err == 0)
        err = listen(serversocket, backlog);
    if (err == 0)
        err = fcntl(serversocket, F_SETFL, O_NONBLOCK);
    if (err == 0) {
        struct kevent   kev;
        EV_SET(&kev, serverHandle.getSocket(), EVFILT_READ, EV_ADD, 0, 0, const_cast<SocketHandle*>(&serverData.find(serverHandle)->first));
        if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
            serverData.erase(serverHandle);
            throw std::runtime_error(strerror(errno));
        }
    }
    if (err != 0) {
        close(serversocket);
        throw std::runtime_error(strerror(errno));
    }
    this->_sockfd = serversocket;
    setIpPort(*reinterpret_cast<struct sockaddr_in*>(&hints.ai_addr), this->_ipString, this->_portString);
}

SocketHandle::SocketHandle( int serversocket ) : _isServer(false)
{
    struct sockaddr addr;
    socklen_t       addrlen = sizeof(addr);
    int clientsocket = accept(serversocket, &addr, &addrlen);
    if (clientsocket == -1)
        throw std::runtime_error(strerror(errno));
    int err = fcntl(clientsocket, F_SETFL, O_NONBLOCK);
#ifdef __APPLE__
    int set = 1;
    if (err == 0)
        err = setsockopt(clientsocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
    if (err != 0) {
        close(clientsocket);
        throw std::runtime_error(strerror(errno));
    }
    this->_sockfd = clientsocket;
    setIpPort(*reinterpret_cast<struct sockaddr_in*>(&addr), this->_ipString, this->_portString);
}

SocketHandle::SocketHandle( SocketHandle const & src )
 : _sockfd(src._sockfd), _ipString(src._ipString), _portString(src._portString)
{ }

SocketHandle::~SocketHandle( void )
{
    close(this->_sockfd);
}

bool    SocketHandle::isServer( void ) const { return (this->_isServer); }

int SocketHandle::getSocket( void ) const { return (this->_sockfd); }

std::string const&  SocketHandle::getIpString( void ) const { return (this->_ipString); }

std::string const&  SocketHandle::getPortString( void ) const { return (this->_portString); }




bool    SocketCmp::operator()( SocketHandle const & a, SocketHandle const & b ) const
{
    if (a.getSocket() < b.getSocket())
        return (true);
    return (false);
}
