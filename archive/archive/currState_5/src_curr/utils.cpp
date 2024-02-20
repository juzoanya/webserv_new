/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:39:29 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 13:58:42 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


std::string getDateString(std::time_t time, const char* format)
{
    std::time_t t = time;
    if (t == 0)
        t = std::time(0);
    std::tm* now = std::gmtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, format, now);
    return (std::string(buff, size));
}

std::size_t getStingStreamSize( std::stringstream & ss )
{
    ss.seekg(0, std::ios::end);
    std::size_t pageSize = ss.tellg();
    ss.seekg(0, std::ios::beg);
    return (pageSize);
}

std::size_t getStreamSize( std::istream & is )
{
    is.seekg(0, std::ios::end);
    std::size_t pageSize = is.tellg();
    is.seekg(0, std::ios::beg);
    return (pageSize);
}

void    addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize )
{
    std::size_t currBufferSize = buffer.size();
    buffer.resize(currBufferSize + streamSize);
    is.read(buffer.data() + currBufferSize, streamSize);
}


WsIpPort::WsIpPort( void )
{ }

WsIpPort::WsIpPort( struct sockaddr_in const & addr ) : _address(addr)
{
	this->_portStr = toStr(htons(addr.sin_port));
    // std::cout << "htons port int: " << htons(addr.sin_port) << std::endl;
    // std::cout << "port int: " << (addr.sin_port) << std::endl;
	std::stringstream ss;
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
	this->_ipStr = ss.str();
    // std::cout << "wsIpPort ip: " << this->_ipStr << " | port: " << this->_portStr << std::endl;
}

// WsIpPort::WsIpPort( std::string const & ip, std::string const & port ) : _ipStr(ip), _portStr(port)
// {
// 	memset(&this->_address, 0, sizeof(struct sockaddr_in));
// 	uint32_t ipInt = 0;
// 	unsigned int ipPartNbr;
//     // std::size_t pos_start = 0, pos = 0;
//     const char* ipPtr = ip.data();
//     char* rest;
// 	for (int i = 0; i != 4; ++i) {
//         ipPartNbr = strtol(ipPtr, &rest, 10);
//         if ((i == 3 && *rest != 0) || (i < 3 && *rest != '.') || (i < 3 && !std::isdigit(*(rest + 1))))
//             throw::std::runtime_error("invalid ip address");
//         ipPtr = rest + 1;
// 		ipInt |= (ipPartNbr << (i * 8));
// 	}
//     unsigned int portInt = strtol(port.c_str(), &rest, 10);
//     if (portInt == 0 || *rest != 0)
//         throw::std::runtime_error("invalid port");
//     this->_address.sin_family = AF_INET;
//     this->_address.sin_addr.s_addr = ipInt;
//     this->_address.sin_port = htons(portInt);
// }

WsIpPort::WsIpPort( std::string const & ipStr, std::string const & portStr ) : _ipStr(ipStr), _portStr(portStr)
{
    // std::cout << "wsIpPort ip: " << ipStr << " | port: " << portStr << std::endl;
    memset(&this->_address, 0, sizeof(struct sockaddr_in));
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
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = ip;
    // this->_address.sin_addr.s_addr = htonl(ip);
    this->_address.sin_port = htons(port);
    // std::cout << "htons port: " << htons(port) << std::endl;
    // std::cout << "port: " << (port) << std::endl;
    // std::cout << "htons ip: " << htons(ip) << std::endl;
    // std::cout << "ip: " << (ip) << std::endl;
}

WsIpPort::~WsIpPort( void )
{ }

bool WsIpPort::operator==( WsIpPort const & rhs ) const { return (rhs._ipStr == this->_ipStr && rhs._portStr == this->_portStr); }
bool WsIpPort::operator()( HttpServ const & rhs ) const { return (rhs.serverConfigs.ipPortData.getIpStr() == this->_ipStr && rhs.serverConfigs.ipPortData.getPortStr() == this->_portStr); }
std::string const & WsIpPort::getIpStr( void ) const { return (this->_ipStr); }
std::string const & WsIpPort::getPortStr( void ) const { return (this->_portStr); }
struct sockaddr_in const & WsIpPort::getSockaddrIn( void ) const { return (this->_address); }


