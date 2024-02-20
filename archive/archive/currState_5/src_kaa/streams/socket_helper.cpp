/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_helper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/29 15:12:56 by mberline          #+#    #+#             */
/*   Updated: 2024/02/07 07:57:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>

#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netdb.h>

#include <fcntl.h>

#include <unistd.h>

#include "socket_helper.hpp"

int select_values(int n)
{
    switch (n)
    {
    case WS_SOCKET_HELPER_FAMILY_INET:
        return (AF_INET);
    case WS_SOCKET_HELPER_FAMILY_UNIX:
        return (AF_UNIX);
    case WS_SOCKET_HELPER_TYPE_TCP:
        return (SOCK_STREAM);
    case WS_SOCKET_HELPER_TYPE_UDP:
        return (SOCK_DGRAM);
    }
    return (-1);
}

int listen_socket_priv(ws_socket_helper_type_t socket_type, struct sockaddr *addr, socklen_t addrlen, int backlog, bool noblock)
{
    int family, socktype;
    family = select_values(socket_type & WS_SOCKET_HELPER_FAMILY_REF);
    socktype = select_values(socket_type & WS_SOCKET_HELPER_TYPE_REF);
    if (family == -1 || socktype == -1)
        return (-1);
    int socketfd = socket(family, socktype, 0);
    if (socketfd == -1) {
        std::cerr << "error: socket: " << strerror(errno) << std::endl;
        return (-1);
    }
    int set = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (void *)&set, sizeof(int)) == -1)
        return (-1);
    if (bind(socketfd, addr, addrlen) == -1) {
        close(socketfd);
        std::cerr << "error: bind: " << strerror(errno) << std::endl;
        return (-1);
    }
    if (listen(socketfd, backlog) == -1) {
        close(socketfd);
        std::cerr << "error: listen: " << strerror(errno) << std::endl;
        return (-1);
    }
    if (noblock && fcntl(socketfd, F_SETFL, O_NONBLOCK) == -1) {
        close(socketfd);
        std::cerr << "error: fcntl: " << strerror(errno) << std::endl;
        return (-1);
    }
    return (socketfd);
}

int set_tcp_addrinfo(const char *hostname, const char *service, struct addrinfo *info)
{
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int gaierr = getaddrinfo(hostname, service, &hints, &res);
    if (gaierr != 0) {
        std::cerr << "error: getaddrinfo: " << gai_strerror(gaierr) << std::endl;
        return (-1);
    }
    if (!res)
        return (-1);
    std::memcpy(info, res, sizeof(struct addrinfo));
    freeaddrinfo(res);
    return (0);
}

// supports only INET_TCP
int make_socket(const char *hostname, const char *service, int backlog, bool noblock)
{
    struct addrinfo info;
    if(set_tcp_addrinfo(hostname, service, &info) == -1)
        return (-1);
    return (listen_socket_priv(WS_SOCKET_HELPER_INET_TCP, info.ai_addr, info.ai_addrlen, backlog, noblock));
}

// supports only UNIX_TCP
int make_socket(const char *domain_sock_path, int backlog, bool noblock)
{
    struct sockaddr_un un;

    std::memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    std::strncpy(un.sun_path, domain_sock_path, std::strlen(domain_sock_path));
    return (listen_socket_priv(WS_SOCKET_HELPER_UNIX_TCP, (struct sockaddr *)&un, sizeof(un), backlog, noblock));
}

int  cmpSock(struct sockaddr *a, struct sockaddr *b)
{
    if (a->sa_family != b->sa_family) {
        return (a->sa_family - b->sa_family);
    }
    if (a->sa_family == AF_INET) {
        return (((struct sockaddr_in *)a)->sin_addr.s_addr - ((struct sockaddr_in *)b)->sin_addr.s_addr);
    }
    if (a->sa_family == AF_UNIX) {
        return (std::strcmp(((struct sockaddr_un *)a)->sun_path, ((struct sockaddr_un *)b)->sun_path));
    }
    return (-1);
}

int  cmpSock(int a, struct sockaddr *b)
{
    struct sockaddr addr;
    socklen_t   addrlen = sizeof(struct sockaddr);
    if (getsockname(a, &addr, &addrlen) == -1) {
        return (-1);
    }
    return (cmpSock(&addr, b));
}

int  cmpSock(int a,  int b)
{
    struct sockaddr addr;
    socklen_t   addrlen = sizeof(struct sockaddr);
    if (getsockname(b, &addr, &addrlen) == -1) {
        return (-1);
    }
    return (cmpSock(a, &addr));
}


void ass(int err)
{
    if (err == -1) {
        perror("error: ");
        exit(1);
    }
}

// std::string getIp4String( struct sockaddr_in const & addr )
// {
//     uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
//     std::stringstream ss;
//     ss << ((ip_addr >> 24) & 0xFF) << ".";
//     ss << ((ip_addr >> 16) & 0xFF) << ".";
//     ss << ((ip_addr >> 8) & 0xFF) << ".";
//     ss << (ip_addr & 0xFF);
//     return (ss.str());
// }

// std::string getIp4String( int socketfd )
// {
//     struct sockaddr_in  addr;
//     socklen_t           addr_len = sizeof(addr);
//     if (getsockname(socketfd, (struct sockaddr*)&addr, &addr_len) == -1)
//         return ("");
//     return (getIp4String(addr));
// }

std::string getIp4String( struct sockaddr const & addr )
{
    if (addr.sa_family != AF_INET)
        return ("");
    struct sockaddr_in const * addriPtr = reinterpret_cast<struct sockaddr_in const *>(&addr);
    uint32_t ip_addr = ntohl(addriPtr->sin_addr.s_addr);
    std::stringstream ss;
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
    return (ss.str());
}

std::string getIp4String( int socketfd )
{
    struct sockaddr addr;
    socklen_t       addr_len = sizeof(addr);
    if (getsockname(socketfd, &addr, &addr_len) == -1)
        return ("");
    return (getIp4String(addr));
}

std::string getAddrString( int socketfd )
{
    struct sockaddr addr;
    socklen_t       addr_len = sizeof(addr);
    if (getsockname(socketfd, &addr, &addr_len) == -1)
        return ("");
    if (addr.sa_family == AF_INET)
        return (getIp4String(addr));
    else if (addr.sa_family == AF_UNIX) {
        struct sockaddr_un *addr_un = reinterpret_cast<struct sockaddr_un *>(&addr);
        int pathLen = sizeof(*(addr_un)) - sizeof((addr_un)->sun_path) + strlen((addr_un)->sun_path);
        return (std::string(addr_un->sun_path, pathLen));
    }
    return ("");
}

std::string getPortString( struct sockaddr const & addr )
{
    if (addr.sa_family != AF_INET)
        return ("");
    struct sockaddr_in const * addriPtr = reinterpret_cast<struct sockaddr_in const *>(&addr);
    std::stringstream ss;
    ss << htons(addriPtr->sin_port);
    return (ss.str());
}

std::string getPortString( int socketfd )
{
    struct sockaddr addr;
    socklen_t       addr_len = sizeof(addr);
    if (getsockname(socketfd, &addr, &addr_len) == -1)
        return ("");
    return (getPortString(addr));
}

int getSockFamily(int socketfd)
{
    struct sockaddr addr;
    socklen_t       addr_len = sizeof(addr);
    int ret = getsockname(socketfd, &addr, &addr_len);
    ass(ret);
    if (ret == -1)
        return (-1);
    return (addr.sa_family); 
}
