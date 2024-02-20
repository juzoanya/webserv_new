/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:40:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 13:57:19 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include "headers.hpp"

template<typename T>
std::string toStr(T data)
{
    std::stringstream ss;
    ss << data;
    return (ss.str());
}

class HttpServ;

class WsIpPort {
    public:
        WsIpPort( void );
        WsIpPort( struct sockaddr_in const & addr );
        WsIpPort( std::string const & ip, std::string const & port );
        ~WsIpPort( void );

        bool operator==( WsIpPort const & rhs ) const;
        bool operator()( HttpServ const & rhs ) const;

        std::string const & getIpStr( void ) const;
        std::string const & getPortStr( void ) const;
        struct sockaddr_in const & getSockaddrIn( void ) const;
    private:
        std::string         _ipStr;
        std::string         _portStr;
        struct sockaddr_in  _address;
};


std::string getDateString(std::time_t time, const char* format);
std::size_t getStingStreamSize(std::stringstream & ss);
std::size_t getStreamSize(std::istream & is );
void        addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize );

#endif