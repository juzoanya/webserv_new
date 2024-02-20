/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpfunc.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 08:12:45 by mberline          #+#    #+#             */
/*   Updated: 2023/12/22 11:44:02 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpfunc.hpp"
#include "FileInfo.hpp"

ClientData::ClientData( ConfigParser::ServerContext& serverConf ) : serverConfig(serverConf)
{ }

ConfigHandler::ConfigHandler( ConfigParser::ServerContext& serverConfig, HttpRequest& request )
 : _serverConfig(&serverConfig.serverConfig), _locationConfig(NULL)
{
    std::vector<std::pair<std::string, ws_config_t> >&   locConfigs = serverConfig.locationConfig;
    int index = -1;
    for (std::size_t i = 0; i != locConfigs.size(); ++i) {
        std::string const & path = request.getRequestLine().getPathDecoded();
        if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == path) {
            this->_locationConfig = &serverConfig.locationConfig[i].second;
            break ;
        } else if (locConfigs[i].first.find(path)) {
            if (index == -1 || locConfigs[i].first.size() > serverConfig.locationConfig[index].first.size()) {
                index = i;
            }
        }
    }
    if (index != -1) {
        this->_locationConfig = &serverConfig.locationConfig[index].second;
    }
}

ConfigHandler::~ConfigHandler( void )
{
    
}

std::vector<std::string> const & ConfigHandler::getVal( std::string const & key ) const
{
    ws_config_t::iterator locit = this->_locationConfig->find(key);
    if (locit != this->_locationConfig->end())
        return (locit->second);
    ws_config_t::iterator servit = this->_serverConfig->find(key);
    if (servit != this->_serverConfig->end())
        return (locit->second);
    return (this->_dummyVec);
}

std::string const & ConfigHandler::getDocRoot( void ) const
{
    std::vector<std::string> const & value = this->getVal("root");
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);
}

// std::string const & ConfigHandler::getServerName( void ) const
// {
    
// }

long ConfigHandler::getMaxBodySize( void ) const
{
    std::vector<std::string> const & value = this->getVal("client_max_body_size");
    if (value.size() == 1) {
        char* rest;
        long size = std::strtol(value[0].c_str(), &rest, 10);
        std::string unit(rest);
        return (unit == "M" ? size * 1000000 : unit == "K" ? size * 1000 : size);
    }
    return (4096);
}

bool    ConfigHandler::hasDirectoryListing( void ) const
{
    std::vector<std::string> const & value = this->getVal("directory_listing");
    if (value.size() == 1 && value[0] == "on")
        return (true);
    return (false);
}

bool    ConfigHandler::checkAllowedMethod( std::string const & method ) const
{
    std::vector<std::string> const & value = this->getVal("accepted_methods");
    for (std::size_t i = 0; i != value.size(); ++i) {
        if (value[i] == method)
            return (true);
    }
    return (false);
}

std::string const & ConfigHandler::getRedirection( void ) const
{
    std::vector<std::string> const & value = this->getVal("return");
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);   
}

std::string const & ConfigHandler::getErrorPage( std::string const & statusCode ) const
{
    std::vector<std::string> const & value = this->getVal("return" + statusCode);
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);   
}

std::vector<std::string> const & ConfigHandler::getIndexFile( void ) const
{
    return (this->getVal("index"));
}
