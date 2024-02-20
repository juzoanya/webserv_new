/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 16:49:50 by mberline          #+#    #+#             */
/*   Updated: 2024/01/07 12:42:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include "ConfigParser.hpp"
#include "HttpRequest.hpp"
#include "HttpStatic.hpp"
#include "ConfigHandler.hpp"

struct Content {
    Content( void );
    Content( const char* dataPtr, int dataSize );
    ~Content( void );
    const char* data;
    int         size;
};

class   HttpClient {
    public:
        HttpClient( ConfigHandler& httpConfig, std::string const & ipStr, std::string const & portStr );
        ~HttpClient( void );
        
        Content*    getHttpContent( void );
        int        handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end);

    private:
        int    setHeader( ws_http::statuscodes_t status, HttpConfig& config );
        int    handleCGI( HttpConfig& config );
        int    handleError( ws_http::statuscodes_t status, HttpConfig& config );
        ConfigHandler       _httpConfigHandler;
        std::string         _ipStr;
        std::string         _portStr;
        HttpRequest         _request;
        HttpStatic          _staticContent;
        std::string         _httpHeader;
        Content             _headerContent;
        Content             _bodyContent;
};

// class   HttpClient {
//     public:
//         HttpClient( HttpConfiguration& httpConfig, std::string const & ipStr, std::string const & portStr );
//         ~HttpClient( void );
        
//         Content*    getHttpContent( void );
//         void        handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end);

//     private:
//         // void    setLocationConfig( void );
//         void    setFilePath( void );
//         std::vector<std::string>*  findDirective( std::string const & key );
//         void    handleGetRequest( void );
//         void    handleCGI( void );
//         void    handleError( ws_http::statuscodes_t status );
//         HttpConfiguration&  _httpConfig;
//         std::string         _ipStr;
//         std::string         _portStr;
//         HttpRequest         _request;
//         HttpStatic          _staticContent;
//         std::string         _httpHeader;
//         Content*            _headerContent;
//         Content*            _bodyContent;
// };

#endif