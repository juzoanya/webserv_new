/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfiguration.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:02:26 by mberline          #+#    #+#             */
/*   Updated: 2024/01/05 07:48:49 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONFIGURATION_HPP
#define HTTP_CONFIGURATION_HPP

#include <string>
#include <vector>
#include <map>
#include "ConfigParser.hpp"
#include "HttpRequest.hpp"

class   HttpConfigData {
public:
    HttpConfigData( void );
    HttpConfigData( ws_config_t* serverDirectives, ws_config_t* locationDirectives );
    ~HttpConfigData( void );
    std::vector<std::string>*   getDirectiveValues( std::string const & key );
    std::vector<std::string>*   getServerValues( std::string const & key );
    std::vector<std::string>*   getLocationValues( std::string const & key );
private:
    ws_config_t*    _serverDirectives;
    ws_config_t*    _locationDirectives;
};

struct  HttpConfiguration {
    HttpConfiguration( ConfigParser::ServerContext* newConfig, std::string const & ip, std::string const & port );
    ~HttpConfiguration( void );
    void        addServerConfig(ConfigParser::ServerContext* newConfig);
    std::string changeServerLocation( HttpRequest& request );

    std::string     serverIp;
    std::string     serverPort;
    HttpConfigData  configs;
    std::string     filePath;
private:
    std::string     setConfiguration( int index, HttpRequest& request );
    std::vector<ConfigParser::ServerContext*>   _serverConfigs;
};


#endif