/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketHandle.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 11:51:14 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 11:38:41 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HANDLE_HPP
#define SOCKET_HANDLE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/time.h>

typedef std::vector<char>   buff_t;

class SocketHandle {
public:
    SocketHandle( std::string const& host, std::string const& port, int backlog );
    SocketHandle( int serversocket );
    SocketHandle( SocketHandle const & src );

    ~SocketHandle( void );

    int                 getSocket( void ) const;
    bool                isServer( void ) const;
    std::string const&  getIpString( void ) const;
    std::string const&  getPortString( void ) const;

private:
    bool        _isServer;
    int         _sockfd;
    std::string _ipString;
    std::string _portString;
};

struct SocketCmp {
    bool operator() ( SocketHandle const & a, SocketHandle const & b ) const;
};

#endif