/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 13:02:26 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 10:28:17 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIG_HANDLER_HPP
#define CONFIG_HANDLER_HPP

#include "headers.hpp"

class ConfigParser;

struct Redirect {
	Redirect( ws_http::statuscodes_t  status, std::string const & loc );
	~Redirect( void );
	ws_http::statuscodes_t  redirectStatus;
	std::string const & location;
};

class   HttpConfig {
public:
	HttpConfig( void );
	HttpConfig( ws_config_t const * serverDirectives, ws_config_t const * locationDirectives, std::string const & pathDecoded, std::string const & serverName );
	~HttpConfig( void );
	HttpConfig  &operator=(const HttpConfig &);
	std::string const & getFilePath( void ) const;
	std::string const & getRootPath( void ) const;
	std::string const & getServerName( void ) const;
	long                getMaxBodySize( void ) const;
	bool                hasDirectoryListing( void ) const;
	bool                checkAllowedMethod( std::string const & method ) const;
	Redirect            getRedirection( void ) const;
	std::string const & getErrorPage( ws_http::statuscodes_t statusCode ) const;
	std::string const & getErrorPage( std::string const & statusCode ) const;
	std::string const & getCgiExecutable( void ) const;
	std::string const & getUploadDir( void ) const;
	std::string const & getMimeType( void ) const;
	std::vector<std::string> const & getIndexFile( void ) const;
private:
	std::pair<const std::string, std::vector<std::string> > const &   getMapValue( std::string const & key, bool exact ) const;
	ws_config_t const*    _serverDirectives;
	ws_config_t const*    _locationDirectives;
	std::string     _filePath;
	std::string     _rootPath;
	std::string     _serverName;
	static std::pair<const std::string, std::vector<std::string> > _dummyMapVal;
};

#endif
