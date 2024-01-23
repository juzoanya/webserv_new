/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:03:01 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 22:53:47 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

HttpConfig::HttpConfig( ws_config_t* serverDirectives, ws_config_t* locationDirectives, std::string const & filePath, std::string const & serverName)
 : _serverDirectives(serverDirectives), _locationDirectives(locationDirectives), _filePath(filePath), _serverName(serverName)
{ }

HttpConfig::~HttpConfig( void )
{ }

std::pair<const std::string, std::vector<std::string> > const &   HttpConfig::getMapValue( std::string const & key, bool exact ) const
{
    ws_config_t::iterator it;
    if (this->_locationDirectives) {
        // ws_config_t::iterator it;
        if (exact)
            it = this->_locationDirectives->find(key);
        else
            it = this->_locationDirectives->lower_bound(key);
        if (it != this->_locationDirectives->end())
            return (*it);
    }
    if (this->_serverDirectives) {
        // ws_config_t::iterator it;
        if (exact)
            it = this->_locationDirectives->find(key);
        else
            it = this->_locationDirectives->lower_bound(key);
        if (it != this->_serverDirectives->end())
            return (*it);
    }
    return (this->_dummyMapVal);
}

std::string const & HttpConfig::getServerName( void ) const
{
    return (this->_serverName);
}

std::string const & HttpConfig::getFilePath( void ) const
{
    return (this->_filePath);
}

long HttpConfig::getMaxBodySize( void ) const
{
    std::pair<const std::string, std::vector<std::string> > const & value = this->getMapValue("client_max_body_size", true);
    if (value.second.size() == 1) {
        char* rest;
        long size = std::strtol(value.second[0].c_str(), &rest, 10);
        std::string unit(rest);
        return (unit == "M" ? size * 1000000 : unit == "K" ? size * 1000 : unit == "" ? size : 4096);
    }
    return (4096);
}

bool    HttpConfig::hasDirectoryListing( void ) const
{
    std::pair<const std::string, std::vector<std::string> > const & value = this->getMapValue("directory_listing", true);
    if (value.second.size() == 1 && value.second[0] == "on")
        return (true);
    return (false);
}

bool    HttpConfig::checkAllowedMethod( std::string const & method ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("allowed_methods", true);
    if (value.first.empty() && method == "GET")
        return (true);
    for (std::size_t i = 0; i != value.second.size(); ++i) {
        if (value.second[i] == method)
            return (true);
    }
    return (false);
}

Redirect::Redirect( ws_http::statuscodes_t  status, std::string const & loc )
 : redirectStatus(status), location(loc)
{ }

Redirect::~Redirect( void )
{ }

Redirect HttpConfig::getRedirection( void ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("return", false);
    std::size_t i = value.first.find("return");
    if (i == 0 && value.second.size() == 1) {
        std::string rest = value.first.substr(i, 6);
        if (rest == "301")
            return (Redirect(ws_http::STATUS_301_MOVED_PERMANENTLY, value.second[0]));
        else if (rest == "302")
            return (Redirect(ws_http::STATUS_302_FOUND, value.second[0]));
    }
    return (Redirect(ws_http::STATUS_UNDEFINED, value.second[0]));
}

std::string const & HttpConfig::getErrorPage( std::string const & statusCode ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("error_page" + statusCode, false);
    if (value.first == "error_page" + statusCode && value.second.size() == 1)
        return (value.second[0]);
    return (this->_dummyMapVal.first);   
}

std::vector<std::string> const & HttpConfig::getIndexFile( void ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("index", true);
    return (value.second);
}

std::string getFileExtension( const std::string& filePath )
{
    size_t lastDotPosition = filePath.find_last_of(".");
    if (lastDotPosition != std::string::npos)
        return (filePath.substr(lastDotPosition + 1));
    return ("");
    }

std::string const & HttpConfig::getCgiExecutable( void ) const
{
    std::string fileExtension = getFileExtension(this->_filePath);
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("cgi" + fileExtension, false);
    if (value.first == "cgi" + fileExtension && value.second.size() == 1)
        return (value.second[0]);
    return (this->_dummyMapVal.first);
}

std::string const & HttpConfig::getMimeType( void ) const
{
    std::string fileExtension = getFileExtension(this->_filePath);
    std::map<const std::string, const std::string>::const_iterator it = ws_http::mimetypes.find(fileExtension);
    if (it != ws_http::mimetypes.end())
        return (it->second);
    it = ws_http::mimetypes.find("");
    return (it->second);
}




ConfigHandler::ConfigHandler( std::string const & ip, std::string const & port )
 : serverIp(ip), serverPort(port)
{ }

ConfigHandler::ConfigHandler( ConfigParser::ServerContext* newConfig, std::string const & ip, std::string const & port )
 : serverIp(ip), serverPort(port)
{
    this->addServerConfig(newConfig);
}

ConfigHandler::~ConfigHandler( void )
{ }

// takes
std::string getFilePath(ws_config_t* serverDirectives, ws_config_t* locationDirectives, std::string const & pathDecoded, std::string const & location, std::string const & defaultRoot)
{
    if (locationDirectives) {
        ws_config_t::iterator it = locationDirectives->find("root");
        if (it != locationDirectives->end() && it->second.size() == 1)
            return (it->second.at(0) + pathDecoded.substr(location.size(), std::string::npos));
    }
    if (serverDirectives) {
        ws_config_t::iterator it = serverDirectives->find("root");
        if (it != serverDirectives->end())
            return(it->second.at(0) + pathDecoded);
    }
    return (defaultRoot + pathDecoded);
}

HttpConfig ConfigHandler::getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader, std::string const & defaultRoot )
{
    if (this->_serverConfigs.size() == 0)
        return (HttpConfig(NULL, NULL, defaultRoot + pathDecoded, ""));
    std::string hostname = hostHeader.substr(0, hostHeader.find(':'));
    ConfigParser::ServerContext& currConfig = *this->_serverConfigs[0];
    for (std::size_t i = 0; i != this->_serverConfigs.size(); ++i) {
        ws_config_t::iterator it = this->_serverConfigs[i]->serverConfig.find("server_name");
        if (it == this->_serverConfigs[i]->serverConfig.end())
            continue ;
        std::vector<std::string>::iterator itServerName = std::find(it->second.begin(), it->second.end(), hostname);
        if (itServerName != it->second.end()) {
            currConfig = *this->_serverConfigs[i];
            break ;
        }
    }
    ws_config_t* currLoc = NULL;
    std::string selectedLocation = "";
    for (std::size_t i = 0 ; i != currConfig.locationConfig.size(); ++i) {
        ws_config_t::iterator it = currConfig.locationConfig[i].find("location");
        if (it == currConfig.locationConfig[i].end()) {
            std::cout << "no location --\n";
            continue;
        }
        std::string location = (it->second.size() == 1) ? it->second.at(0) : (it->second.size() == 2) ? it->second.at(1) : NULL;
        bool isAbs = (it->second.size() == 2 && it->second.at(0) == "=") ? true : false;
        std::cout << "location: " << location << std::endl;
        std::cout << "absloute?: " << isAbs << std::endl;
        if (isAbs && location == pathDecoded) {
            currLoc = &currConfig.locationConfig[i];
            selectedLocation = location;
            std::cout << "absolute path found" << std::endl;
            break ;
        } else if (pathDecoded.find(location) == 0 && (!currLoc || location.size() > selectedLocation.size())) {
            currLoc = &currConfig.locationConfig[i];
            selectedLocation = location;
            std::cout << "new path found" << std::endl;
        }
    }
    std::cout << "selected location: " << selectedLocation << std::endl;
    std::string filePath = getFilePath(&currConfig.serverConfig, currLoc, pathDecoded, selectedLocation, defaultRoot);
    return (HttpConfig(&currConfig.serverConfig, currLoc, filePath, hostname));
}

void    ConfigHandler::addServerConfig(ConfigParser::ServerContext* newConfig)
{
    this->_serverConfigs.push_back(newConfig);
}
