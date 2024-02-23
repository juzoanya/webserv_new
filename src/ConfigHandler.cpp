/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:03:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/23 12:26:36 by juzoanya         ###   ########.fr       */
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
	logging("\n------- HttpConfig constructor -------", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	if (locationDirectives) {
		logging("------- check locationConfig -------", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		ws_config_t::const_iterator itLoc = locationDirectives->find("location");
		std::string const &  location = (itLoc->second.size() == 2 && itLoc->second.at(0) == "=") ? itLoc->second.at(1) : itLoc->second.at(0);
		logging("location in constructor: ", location, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		ws_config_t::const_iterator it = locationDirectives->find("root");
		if (it != locationDirectives->end() && it->second.size() == 1) {
			this->_rootPath = it->second.at(0);
			this->_filePath = it->second.at(0) + pathDecoded.substr(location.size(), std::string::npos);
			logging("----> found root in locationConfig:", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			logging("- selected rootPath: ", _rootPath, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			logging("- selected filePath: ", _filePath, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			return ;
		}
	}
	if (serverDirectives) {
		logging("------- check serverConfig -------", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		ws_config_t::const_iterator it = serverDirectives->find("root");
		if (it != serverDirectives->end()) {
			this->_rootPath = it->second.at(0);
			this->_filePath = it->second.at(0) + pathDecoded;
			logging("----> found root in serverConfig:", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			logging("- selected rootPath: ", _rootPath, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
			logging("- selected filePath: ", _filePath, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
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
	ws_config_t::const_iterator it;
	if (this->_locationDirectives) {
		if (exact) {
			it = this->_locationDirectives->find(key);
		} else {
			it = this->_locationDirectives->lower_bound(key);
		}
		if (it != this->_locationDirectives->end()) {
			return (*it);
		}
	}
	if (this->_serverDirectives) {
		if (exact) {
			it = this->_serverDirectives->find(key);
		} else {
			it = this->_serverDirectives->lower_bound(key);
		}
		if (it != this->_serverDirectives->end()) {
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
	//std::pair<const std::string, std::vector<std::string> > const & value = this->getMapValue("client_max_body_size", true);
	// if (value.second.size() == 1) {
	// 	char* rest;
	// 	long size = std::strtol(value.second[0].c_str(), &rest, 10);
	// 	std::string unit(rest);
	// 	return (unit == "M" ? size * 1000000 : unit == "K" ? size * 1000 : unit == "" ? size : 4096);
	// }
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
		int redirStatus;
		std::stringstream ss(value.first.substr(6, std::string::npos));
		ss >> redirStatus;
		if (redirStatus >= ws_http::STATUS_300_MULTIPLE_CHOICES && redirStatus <= ws_http::STATUS_308_PERMANENT_REDIRECT )
			return (Redirect(static_cast<ws_http::statuscodes_t>(redirStatus), value.second[0]));
	}
	return (Redirect(ws_http::STATUS_UNDEFINED, value.second[0]));
}

std::string const & HttpConfig::getErrorPage( ws_http::statuscodes_t statusCode ) const
{
	std::ostringstream oss;
	oss << statusCode;
	std::string statusCodeS = oss.str();
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
