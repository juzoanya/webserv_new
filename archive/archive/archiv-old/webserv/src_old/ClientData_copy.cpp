/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientData_copy.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 12:09:36 by mberline          #+#    #+#             */
/*   Updated: 2023/12/28 15:45:43 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientData_copy.hpp"

IpPortData    setIpPort(struct sockaddr_in const & addr, std::string& ipString, std::string& portString)
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

Content::Content( void ) : data(NULL), size(0)
{ }

Content::Content( const char* dataPtr, int dataSize ) : data(dataPtr), size(dataSize)
{ }

Content::~Content( void )
{ }


IpPortData::IpPortData( std::string const & ip, std::string const & port ) : ipString(ip), portString(port)
{ }

IpPortData::~IpPortData( void )
{ }


HttpServerData::HttpServerData( struct sockaddr_in const & addr, ConfigParser::ServerContext& serverConf ) : serverConfig(serverConf)
{
    setIpPort(addr, this->ipString, this->portString);
}

HttpServerData::~HttpServerData( void )
{ }


HttpClientData::HttpClientData( struct sockaddr_in const & addr, HttpServerData& servData ) : serverData(servData), toSend(false)
{
    setIpPort(addr, this->ipString, this->portString);
}

HttpClientData::~HttpClientData( void )
{ }

