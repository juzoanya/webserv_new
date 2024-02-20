/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/21 21:37:38 by mberline         ###   ########.fr       */
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

class Content {
    public:
        Content( const char* dataPtr, int dataSize );
        Content( void );
        ~Content( void );
        const char* data;
        int         size;
    private:
        bool        isHeader;
};

class HttpStatic {
    public:

        HttpStatic( ConfigHandler& configHandler );
        ~HttpStatic( void );

        typedef enum {  WS_NO_ACCESS, WS_NOT_FOUND, WS_REGULAR, WS_DIR } file_t;
        static  HttpStatic::file_t  checkFileStat( std::string const & filePath, struct stat* custFileStat );
        const char* dataPtr;
        int         dataSize;

        ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
        ws_http::statuscodes_t  setHeader( ws_http::statuscodes_t statusCode, std::string const & mimeType, std::size_t fileSize, std::string const & location="" );
        ws_http::statuscodes_t  makeRedirect( Redirect& redirect );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestUri );
        ws_http::statuscodes_t  setError( ws_http::statuscodes_t errorStatusCode );
        bool                    setFile( ws_http::statuscodes_t statusCode );
        ws_http::statuscodes_t  checkDirectory( std::string const & requestUri );
        void                    setDirListing( std::string requestUri, std::vector<DirListingInfos> & dirEntries );
        // HttpRequestBody             _requestBody;
        HttpRequest                 _request;
        std::vector<char>           _fileData; //holds data to be written to the client
        HttpConfig                  _httpConfig;
        ConfigHandler*              _configHandler;
};

#endif