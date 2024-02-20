/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:03:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 16:27:02 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::pair<const std::string, std::vector<std::string> > HttpConfig::_dummyMapVal;

HttpConfig::HttpConfig( void )
 : _serverDirectives(NULL), _locationDirectives(NULL)
{ }

HttpConfig::HttpConfig( ws_config_t const * serverDirectives, ws_config_t const * locationDirectives, std::string const & pathDecoded, std::string const & serverName )
 : _serverDirectives(serverDirectives), _locationDirectives(locationDirectives), _serverName(serverName)
{
    std::cout << "\n------- HttpConfig constructor -------" << std::endl;
    if (locationDirectives) {
        std::cout << "------- check locationConfig -------" << std::endl;
        ws_config_t::const_iterator itLoc = locationDirectives->find("location");
        std::string const &  location = (itLoc->second.size() == 2 && itLoc->second.at(0) == "=") ? itLoc->second.at(1) : itLoc->second.at(0);
        std::cout << "location in constructor: " << location << std::endl;
        ws_config_t::const_iterator it = locationDirectives->find("root");
        if (it != locationDirectives->end() && it->second.size() == 1) {
            this->_rootPath = it->second.at(0);
            this->_filePath = it->second.at(0) + pathDecoded.substr(location.size(), std::string::npos);
            std::cout << "----> found root in locationConfig:" << std::endl;
            std::cout << "- selected rootPath: " << this->_rootPath << std::endl;
            std::cout << "- selected filePath: " << this->_filePath << std::endl;
            return ;
        }
    }
    if (serverDirectives) {
        std::cout << "------- check serverConfig -------" << std::endl;
        ws_config_t::const_iterator it = serverDirectives->find("root");
        if (it != serverDirectives->end()) {
            this->_rootPath = it->second.at(0);
            this->_filePath = it->second.at(0) + pathDecoded;
            std::cout << "----> found root in serverConfig:" << std::endl;
            std::cout << "- selected rootPath: " << this->_rootPath << std::endl;
            std::cout << "- selected filePath: " << this->_filePath << std::endl;
            return ;
        }
    }
    this->_rootPath = DEFAULT_ROOT_PATH;
    this->_filePath = DEFAULT_ROOT_PATH + pathDecoded;
}

HttpConfig::~HttpConfig( void )
{ }

HttpConfig &    HttpConfig::HttpConfig::operator=(const HttpConfig & src)
{
    _serverDirectives = src._serverDirectives;
    _locationDirectives = src._locationDirectives;
    _filePath = src._filePath;
    _serverName = src._serverName;
    return (*this);
}

std::pair<const std::string, std::vector<std::string> > const &   HttpConfig::getMapValue( std::string const & key, bool exact ) const
{
    // std::cout << "getMapVal\n";
    ws_config_t::const_iterator it;
    if (this->_locationDirectives) {
        // std::cout << "getMapVal - loc\n";
        if (exact) {
            // std::cout << "find\n";
            it = this->_locationDirectives->find(key);
        } else {
            // std::cout << "lower bound\n";
            it = this->_locationDirectives->lower_bound(key);
        }
        if (it != this->_locationDirectives->end()) {
            // std::cout << "found\n";
            return (*it);
        }
    }
    if (this->_serverDirectives) {
        // std::cout << "getMapVal - server\n";
        if (exact) {
            // std::cout << "find\n";
            it = this->_serverDirectives->find(key);
        } else {
            // std::cout << "lower bound\n";
            it = this->_serverDirectives->lower_bound(key);
        }
        if (it != this->_serverDirectives->end()) {
            // std::cout << "found\n";
            return (*it);
        }
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

std::string const & HttpConfig::getRootPath( void ) const
{
    return (this->_rootPath);
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
    // std::cout << "get Redirection" << std::endl;
    // std::cout << "debred1\n";
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("return", false);
    // std::cout << "debred2\n";
    std::size_t i = value.first.find("return");
    // std::cout << "debred3\n";
    if (i == 0 && value.second.size() == 1) {
        std::string rest = value.first.substr(i, 6);
        if (rest == "301")
            return (Redirect(ws_http::STATUS_301_MOVED_PERMANENTLY, value.second[0]));
        else if (rest == "302")
            return (Redirect(ws_http::STATUS_302_FOUND, value.second[0]));
    }
    return (Redirect(ws_http::STATUS_UNDEFINED, value.second[0]));
}

std::string const & HttpConfig::getErrorPage( ws_http::statuscodes_t statusCode ) const
{
    // std::stringstream ss;
    // ss << statusCode;
    // std::string statusCodeS = ss.str();

    std::ostringstream oss;
    oss << statusCode;
    std::string statusCodeS = oss.str();
    std::cout << "errorPageStatusCode: " << statusCodeS << std::endl;
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("error_page" + statusCodeS, true);
    if (value.first == "error_page" + statusCodeS && value.second.size() == 1)
        return (value.second[0]);
    return (this->_dummyMapVal.first);   
}

std::string const & HttpConfig::getErrorPage( std::string const & statusCode ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("error_page" + statusCode, true);
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

std::string const & HttpConfig::getUploadDir( void ) const
{
    std::pair< const std::string, std::vector<std::string> > const & value = this->getMapValue("upload_root", false);
    if (value.second.size() == 1)
        return (value.second[0]);
    return (this->_dummyMapVal.first); 
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




// ConfigHandler::ConfigHandler( WsIpPort const & ipPort )
//  : ipPortData(ipPort)
// { }

// ConfigHandler::~ConfigHandler( void )
// { }

// HttpConfig ConfigHandler::getHttpConfig( std::string const & pathDecoded, std::string const & hostHeader )
// {
//     std::cout << "\n---- getHttpConfig ----" << std::endl;
//     if (this->_serverConfigs.size() == 0)
//         return (HttpConfig(NULL, NULL, pathDecoded, ""));
//     std::string hostname = hostHeader.substr(0, hostHeader.find(':'));
//     std::cout << "hostname: " << hostname << std::endl;
//     std::cout << "hostheader: " << hostHeader << std::endl;
//     std::cout << "pathDecoded: " << pathDecoded << std::endl;
//     ConfigParser::ServerContext* currConfig = this->_serverConfigs[0];
//     std::cout << "\n------- select server by hostname <-> server_name -------" << std::endl;
//     for (std::size_t i = 0; i != this->_serverConfigs.size(); ++i) {

//         debPrintServerContext("curr context at pos: " + toStr(i) + ": ", *this->_serverConfigs[i], false);
//         debPrintConfigSelectedDirective("server_names of serverContext at index: " + toStr(i) + ": ", "server_name",  this->_serverConfigs[i]->serverConfig, 0);
        
//         ws_config_t::iterator it = this->_serverConfigs[i]->serverConfig.find("server_name");
//         if (it == this->_serverConfigs[i]->serverConfig.end())
//             continue ;
//         std::vector<std::string>::iterator itServerName = std::find(it->second.begin(), it->second.end(), hostname);
//         if (itServerName != it->second.end()) {
//             std::cout << "---> servername found: " << *itServerName << std::endl;
//             currConfig = this->_serverConfigs[i];
//             break ;
//         }
//     }
//     std::cout << "\n------- select location by requestPath -------" << std::endl;
//     ws_config_t* currLoc = NULL;
//     std::size_t selectedLocSize = 0;
//     for (std::size_t i = 0 ; i != currConfig->locationConfig.size(); ++i) {
//         ws_config_t::iterator it = currConfig->locationConfig[i].find("location");
//         if (it == currConfig->locationConfig[i].end() || it->second.size() == 0)
//             continue;
//         std::string const &  location = (it->second.size() == 2 && it->second.at(0) == "=") ? it->second.at(1) : it->second.at(0);
//         bool                 isAbs    = (it->second.size() == 2 && it->second.at(0) == "=") ? true : false;
//         debPrintStrVector("loction: ", it->second, 0);
//         if (isAbs && location == pathDecoded) {
//             std::cout << "-> location is EQUAL pathDecoded : " << location << std::endl;
//             return (HttpConfig(&currConfig->serverConfig, &currConfig->locationConfig[i], pathDecoded,  hostname));
//         } else if (pathDecoded.find(location) == 0 && (!currLoc || location.size() > selectedLocSize)) {
//             currLoc = &currConfig->locationConfig[i];
//             selectedLocSize = location.size();
//             std::cout << "-> selected location: " << location << std::endl;
//         }
//     }
//     std::cout << "-----------------------------------" << std::endl;
//     return (HttpConfig(&currConfig->serverConfig, currLoc, pathDecoded, hostname));
// }

// long    ConfigHandler::operator()( std::string const & pathDecoded, std::string const & hostHeader )
// {
//     std::cout << "confighandler operator() ->return maxBodySize\n";
//     return (this->getHttpConfig(pathDecoded, hostHeader).getMaxBodySize());
// }

// void    ConfigHandler::addServerConfig(ConfigParser::ServerContext* newConfig)
// {
//     this->_serverConfigs.push_back(newConfig);
// }
