/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/23 11:54:24 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTP_STATIC_HPP
#define HTTP_STATIC_HPP

#include "headers.hpp"

struct DirListingInfos {
    DirListingInfos( std::string const & name, struct stat & fileStat );
    ~DirListingInfos( void );
    std::string fileName;
    std::string fileSize;
    std::string fileModDate;
};

class HttpStatic {
    public:

        HttpStatic( ConfigHandler& configHandler, std::string const & clientIpStr, std::string const & clientPortStr, int clientSockFd );
        ~HttpStatic( void );

        typedef enum {  WS_NO_ACCESS, WS_NOT_FOUND, WS_REGULAR, WS_DIR } file_t;
        static  HttpStatic::file_t  checkFileStat( std::string const & filePath, struct stat* custFileStat, int accessFlags );
        const int           clientSocket;
        const std::string   clientIp;
        const std::string   clientPort;
        const char*         dataPtr;
        int                 dataSize;

        ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
        ws_http::statuscodes_t  setHeader( ws_http::statuscodes_t statusCode, std::string const & mimeType, std::size_t fileSize, std::string const & location="" );
        ws_http::statuscodes_t  makeRedirect( Redirect& redirect );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestUri, bool internal );
        ws_http::statuscodes_t  setError( ws_http::statuscodes_t errorStatusCode, bool internal );
        bool                    setFile( ws_http::statuscodes_t statusCode );
        ws_http::statuscodes_t  handleGetHead( ws_http::statuscodes_t statusCode, std::string const & method, bool internal );
        ws_http::statuscodes_t  handlePost( void );
        ws_http::statuscodes_t  checkDirectory( std::string const & requestUri, bool internal );
        void                    setDirListing( std::string requestUri, std::vector<DirListingInfos> & dirEntries );
        HttpRequest             _request;
        std::vector<char>       _fileData; //holds data to be written to the client
        HttpConfig              _httpConfig;
        ConfigHandler*          _configHandler;
        int                     _intRedirectNbr;
};

#endif