/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpfunc.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 08:13:02 by mberline          #+#    #+#             */
/*   Updated: 2023/12/21 21:16:13 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPFUNC_HPP
#define HTTPFUNC_HPP

#include <vector>
#include <string>
#include "http/HttpRequest.hpp"
#include "ConfigParser.hpp"
#include "FileInfo.hpp"

// struct ClientData {
//     ClientData( ConfigParser::ServerContext& serverConf ) : serverConfig(serverConf) { }
//     // HttpMsg             requestData;
//     HttpRequest             request;
//     ConfigParser::ServerContext& serverConfig;
//     bool                toSend;
//     std::vector<char>   responseHeader;
//     buff_t::iterator    currResponseHeaderIt;
//     std::vector<char>   responseBody;
//     buff_t::iterator    currResponseBodyIt;
// };

struct ClientData {
    ClientData( ConfigParser::ServerContext& serverConf );
    // HttpRequest             request;
    ConfigParser::ServerContext& serverConfig;
    bool                toSend;
    // std::vector<char>   responseHeader;
    std::string         headerData;
    FileInfo            fileInfo;
    Content             headerIterator;
    Content             bodyIterator;
};

class ConfigHandler {
    public:
        ConfigHandler( ConfigParser::ServerContext& serverConfig, HttpRequest& request );
        ~ConfigHandler( void );

        std::string const & getDocRoot( void ) const;
        std::string const & getServerName( void ) const;
        long                getMaxBodySize( void ) const;
        bool                hasDirectoryListing( void ) const;
        bool                checkAllowedMethod( std::string const & method ) const;
        std::string const & getRedirection( void ) const;         
        std::string const & getErrorPage( std::string const & statusCode ) const;
        std::vector<std::string> const & getIndexFile( void ) const;

    private:
        std::vector<std::string> const & getVal( std::string const & key ) const;

        ws_config_t*	_serverConfig;
        ws_config_t*	_locationConfig;
        std::string     _dummyValue;
        std::vector<std::string>    _dummyVec;
};


#endif
