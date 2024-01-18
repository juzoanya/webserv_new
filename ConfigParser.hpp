/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 15:58:32 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/16 21:32:53 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "headers.hpp"

typedef std::map<std::string, std::vector<std::string> > ws_config_t;

class ConfigParser
{
	protected:
		int	_lineCount;
		int	_serverCount;
	
	public:
		ConfigParser();
		~ConfigParser();
		ConfigParser(const ConfigParser& src);
		ConfigParser& operator=(const ConfigParser& rhs);


		struct ServerContext
		{
			ws_config_t					serverConfig;
			std::vector<ws_config_t>	locationConfig;
		};
		ServerContext	*serverConfigs;
		std::map<std::string, std::vector<std::string> >	httpConfig;

		void	setServerContext();
		int		getServerCount();
		void	configParser(char *file);
		std::vector<std::string>	processFile(char *confFile);
		bool	isValidContextOrDirective(const std::string& line);
		bool	checkBraceBalance(std::vector<std::string> allConfig);
		void	parseConfig(std::vector<std::string>& allConfig);
		void	parseDirective(std::string& directive, std::map<std::string, std::vector<std::string> >& contextMap);

		std::vector<std::string>	getConfigValue(ServerContext *serverConfig, const std::string& key);
		int	getHandlerConfigIndex(std::string& host, std::string& port);

		void	printConfigMap( std::string const & msg, ws_config_t const & config );

		ServerContext	getHandlerServer(const std::string& serverName);//, ServerContext* configs);

		class FileOpeningException : std::exception
		{
			public:
				const char* what() const throw(){
					return ("Unable to open file.");
				}
		};

		class InvalidContextOrDirective : std::exception
		{
			public:
				const char* what() const throw(){
					return ("Invaline Context or Directive.");
				}
		};

		class MissingBracesException : std::exception
		{
			public:
				const char* what() const throw(){
					return ("Missing braces in configuration file.");
				}
		};
};

#endif
