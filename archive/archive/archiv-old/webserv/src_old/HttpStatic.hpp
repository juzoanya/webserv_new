/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 16:46:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATIC_HPP
#define HTTP_STATIC_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "http/HttpHeaderDef.hpp"
#include "ConfigParser.hpp"

struct Content {
    Content( void );
    Content( const char* dataPtr, int dataSize );
    ~Content( void );
    const char* data;
    int         size;
};

class FileStat {
    public:
        FileStat( void );
        ~FileStat( void );
        int     getErrno( void );
        void    fillData( std::vector<char>& buffer);
    private:
};

class HttpStatic {
    public:
        HttpStatic( void );
        ~HttpStatic( void );

        // void    setContent( std::string const & rootPath, std::string const & currPath );
        void    setHeader( std::string const & key, std::string const & value );
        void    setContentByPath( std::string const & currPath, ConfigParser::ServerContext& serverConfig );
        void    setContentByError( std::string const & currPath, ConfigParser::ServerContext& serverConfig );
        void    setContentByRedirect( std::string const & currPath, ConfigParser::ServerContext& serverConfig );
    private:
        void    setFile( std::string const & filePath );
        void    setErrorPage( ws_http::statuscodes_t statusCode );
        void    setDirectory( std::string const & rootPath, std::string const & currPath );

        ConfigParser::ServerContext& serverConfig;
        std::string         _fileStrData;
        std::vector<char>   _fileData;
};

struct   HttpRequestData {
    std::string                         method;
    std::string                         path;
    std::string                         pathDecoded;
    std::string                         query;
    std::map<std::string, std::string>  headers; 
    std::vector<char>                   bodyData;
};

class   HttpClient {
    public:
        HttpClient( ConfigParser::ServerContext& serverConfig );
        ~HttpClient( void );
        
        Content*    getHttpContent( void );
        void        handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end);

    private:
        std::vector<std::string>*  findDirective( std::string const & key );
        ws_config_t*	_serverConfig;
        ws_config_t*	_locationConfig;
        std::string method;
        std::string path;
        std::string pathDecoded;
        std::string query;
        std::map<std::string, std::string>  headers;
};

#endif