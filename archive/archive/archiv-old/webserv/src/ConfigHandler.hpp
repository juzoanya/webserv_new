/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:02:26 by mberline          #+#    #+#             */
/*   Updated: 2024/01/07 12:33:03 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HANDLER_HPP
#define CONFIG_HANDLER_HPP

#include "headers.hpp"

struct Redirect {
    Redirect( ws_http::statuscodes_t  status, std::string const & loc );
    ~Redirect( void );
    ws_http::statuscodes_t  redirectStatus;
    std::string const & location;
};

class   HttpConfig {
public:
    HttpConfig( ws_config_t* serverDirectives, ws_config_t* locationDirectives, std::string const & filePath, std::string const & serverName );
    ~HttpConfig( void );
    std::string const & getFilePath( void ) const;
    std::string const & getServerName( void ) const;
    long                getMaxBodySize( void ) const;
    bool                hasDirectoryListing( void ) const;
    bool                checkAllowedMethod( std::string const & method ) const;
    Redirect            getRedirection( void ) const;         
    std::string const & getErrorPage( std::string const & statusCode ) const;
    std::string const & getCgiExecutable( void ) const;
    std::string const & getMimeType( void ) const;
    std::vector<std::string> const & getIndexFile( void ) const;
private:
    std::pair<std::string, std::vector<std::string> > const &   getMapValue( std::string const & key, bool exact ) const;
    ws_config_t*    _serverDirectives;
    ws_config_t*    _locationDirectives;
    std::string     _filePath;
    std::string     _serverName;
    std::pair<std::string, std::vector<std::string> > _dummyMapVal;
};

struct  ConfigHandler {
    ConfigHandler( ConfigParser::ServerContext* newConfig, std::string const & ip, std::string const & port );
    ~ConfigHandler( void );
    void        addServerConfig(ConfigParser::ServerContext* newConfig);
    HttpConfig      getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader, std::string const & defaultRoot );

    std::string     serverIp;
    std::string     serverPort;
private:
    HttpConfig     setConfiguration( int index, HttpRequest& request );
    std::vector<ConfigParser::ServerContext*>   _serverConfigs;
};


#endif