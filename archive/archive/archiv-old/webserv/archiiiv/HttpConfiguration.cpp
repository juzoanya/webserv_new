/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfiguration.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:03:01 by mberline          #+#    #+#             */
/*   Updated: 2024/01/05 20:42:55 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfiguration.hpp"

HttpConfigData::HttpConfigData( void ) : _serverDirectives(NULL), _locationDirectives(NULL)
{ }

HttpConfigData::HttpConfigData( ws_config_t* serverDirectives, ws_config_t* locationDirectives )
 : _serverDirectives(serverDirectives), _locationDirectives(locationDirectives)
{ }

HttpConfigData::~HttpConfigData( void )
{ }

std::vector<std::string>*   HttpConfigData::getDirectiveValues( std::string const & key )
{
    std::vector<std::string>* locationValues = this->getLocationValues(key);
    return (locationValues ? locationValues : this->getServerValues(key));
}

std::vector<std::string>*   HttpConfigData::getServerValues( std::string const & key )
{
    if (this->_serverDirectives) {
        ws_config_t::iterator it = this->_serverDirectives->find(key);
        if (it != this->_serverDirectives->end())
            return (&it->second);
    }
    return (NULL);
}

std::vector<std::string>*   HttpConfigData::getLocationValues( std::string const & key )
{
    if (this->_locationDirectives) {
        ws_config_t::iterator it = this->_locationDirectives->find(key);
        if (it != this->_locationDirectives->end())
            return (&it->second);
    }
    return (NULL);
}


HttpConfiguration::HttpConfiguration( ConfigParser::ServerContext* newConfig, std::string const & ip, std::string const & port )
 : serverIp(ip), serverPort(port)
{
    this->addServerConfig(newConfig);
}

HttpConfiguration::~HttpConfiguration( void )
{ }

std::string HttpConfiguration::setConfiguration( int index, HttpRequest& request )
{
    if (index >= this->_serverConfigs.size()) {
        this->configs = HttpConfigData();
        return ("/var/www" + request.pathDecoded);
    }
    ConfigParser::ServerContext* currConfig = this->_serverConfigs[index];
    std::pair<std::string, ws_config_t>* selected = NULL;
    std::vector< std::pair<std::string, ws_config_t> >::iterator locIt = currConfig->locationConfig.begin();
    for (; locIt != currConfig->locationConfig.end(); ++locIt) {
        if (locIt->first.find("=" == 0) && locIt->first.substr(1, std::string::npos) == request.pathDecoded) {
            selected = &(*locIt);
            break ;
        }
        if (locIt->first.find(request.pathDecoded) && (!selected || locIt->first.size() > selected->first.size())) {
            selected = &(*locIt);
        }
    }
    this->configs = HttpConfigData(&currConfig->serverConfig, selected ? &selected->second : NULL);
    std::vector<std::string>* locationRoot = this->configs.getLocationValues("root");
    if (locationRoot && locationRoot->size() == 1)
        return(locationRoot->at(0) + request.pathDecoded.substr(it->first.size(), std::string::npos));
    std::vector<std::string>* serverRoot = this->configs.getServerValues("root");
    if (serverRoot && serverRoot->size() == 1)
        return(serverRoot->at(0) + request.pathDecoded);
    return ("/var/www" + request.pathDecoded);
}

std::string HttpConfiguration::changeServerLocation( HttpRequest& request )
{
    std::map<std::string, std::string>::iterator itHostHeader = request.headerMap.find("host");
    if (itHostHeader == request.headerMap.end()) {
        this->setConfiguration(0, request);
        return ;
    }
    std::string hostname = itHostHeader->second.substr(0, itHostHeader->second.find(':'));
    for (std::size_t i = 0; i != this->_serverConfigs.size(); ++i) {
        ws_config_t::iterator itServerName = this->_serverConfigs[i]->serverConfig.find("server_name");
        if (itServerName == this->_serverConfigs[i]->serverConfig.end())
            continue;
        std::vector<std::string>::iterator foundServer;
        foundServer = std::find(itServerName->second.begin(), itServerName->second.begin(), hostname);
        if (foundServer != itServerName->second.end()) {
            this->setConfiguration(i, request);
            return ;
        }
    }
    this->setConfiguration(0, request);
}

void    HttpConfiguration::addServerConfig(ConfigParser::ServerContext* newConfig)
{
    this->_serverConfigs.push_back(newConfig);
}
