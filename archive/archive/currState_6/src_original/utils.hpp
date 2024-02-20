/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:40:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/03 22:24:50 by mberline         ###   ########.fr       */
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

class FileInfos {
    public:
        FileInfos( std::string const & filePath, std::string const & requestPathDecoded );
        ~FileInfos( void );
        std::string const & checkContainedFile( std::vector<std::string> const & files );
        int                 setDirListing( std::stringstream & ss );
        mode_t  getMode( void ) const;
        int     getError( void ) const;
    private:
        struct DirListingInfos {
            DirListingInfos( std::string const & name, struct stat & fileStat );
            ~DirListingInfos( void );
            std::string fileName;
            std::string fileSize;
            std::string fileModDate;
            mode_t      statFileType;
        };
        mode_t  _fileType;
        int     _fileError;
        // std::string const & _filePath;
        std::string         _requestPathDecoded;
        std::string dummyStr;
        std::vector<DirListingInfos>    _dirListingInfos;
};


std::string getDateString(std::time_t time, const char* format);
std::size_t getStingStreamSize(std::stringstream & ss);
std::size_t getStreamSize(std::istream & is );
void        addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize );

#endif