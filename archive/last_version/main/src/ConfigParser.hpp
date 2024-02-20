/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 15:58:32 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/19 11:16:41 by mberline         ###   ########.fr       */
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
			// std::map<std::string, std::vector<std::string> >	serverConfig;
			// std::vector<std::map<std::string, std::vector<std::string> > >	locationConfig;
			
			
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



		class FileOpeningException : public std::exception
		{
			public:
				const char* what() const throw(){
					return ("Unable to open file.");
				}
		};

		class InvalidContextOrDirective : public std::exception
		{
			public:
				const char* what() const throw(){
					return ("Invaline Context or Directive.");
				}
		};

		class MissingBracesException : public std::exception
		{
			public:
				const char* what() const throw(){
					return ("Missing braces in configuration file.");
				}
		};
};

#endif
