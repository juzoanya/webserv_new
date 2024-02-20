/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_helper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/29 15:12:29 by mberline          #+#    #+#             */
/*   Updated: 2023/11/04 09:24:23 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HELPER_HPP
#define SOCKET_HELPER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <string>

typedef enum {
    WS_SOCKET_HELPER_FAMILY_INET = 0x1,
    WS_SOCKET_HELPER_FAMILY_UNIX = 0x2,
    WS_SOCKET_HELPER_FAMILY_REF = 0xF
}   ws_socket_helper_socket_family_t;

typedef enum {
    WS_SOCKET_HELPER_TYPE_TCP = 0x10,
    WS_SOCKET_HELPER_TYPE_UDP = 0x20,
    WS_SOCKET_HELPER_TYPE_REF = 0xF0
}   ws_socket_helper_socket_type_t;

typedef enum {
    WS_SOCKET_HELPER_INET_TCP = WS_SOCKET_HELPER_FAMILY_INET | WS_SOCKET_HELPER_TYPE_TCP,
    WS_SOCKET_HELPER_UNIX_TCP = WS_SOCKET_HELPER_FAMILY_UNIX | WS_SOCKET_HELPER_TYPE_TCP
}   ws_socket_helper_type_t;

int make_socket(const char *hostname, const char *service, int backlog, bool noblock);
int make_socket(const char *domain_sock_path, int backlog, bool noblock);
int cmpSock(struct sockaddr *a, struct sockaddr *b);
int cmpSock(int a, struct sockaddr *b);
int cmpSock(int a,  int b);

std::string getIp4String( struct sockaddr const & addr );
std::string getIp4String( int socketfd );
std::string getAddrString( int socketfd );
std::string getPortString( struct sockaddr const & addr );
std::string getPortString( int socketfd );
int getSockFamily(int socketfd);


struct SockAddress {
    int	            family;      /* PF_xxx */
	int	            socktype;	/* SOCK_xxx */
	int	            protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
    struct sockaddr binary_address;
    socklen_t       binary_address_len;
};

#endif
