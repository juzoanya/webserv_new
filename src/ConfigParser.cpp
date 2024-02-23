
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 16:03:11 by juzoanya          #+#    #+#             */
/*   Updated: 2023/11/23 19:18:43 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "headers.hpp"

ConfigParser::ConfigParser() : _lineCount(0), _serverCount(0), serverConfigs(NULL)
{
}

ConfigParser::~ConfigParser()
{
	delete[] serverConfigs;
}

ConfigParser::ConfigParser(const ConfigParser& src) : _lineCount(src._lineCount), _serverCount(src._serverCount)
{
	serverConfigs = new ServerContext[_serverCount];
	for (int i = 0; i < this->_serverCount; ++i)
		serverConfigs[i] = src.serverConfigs[i];
	httpConfig = src.httpConfig;
}

ConfigParser&	ConfigParser::operator=(const ConfigParser& rhs)
{
	if (this != &rhs)
	{
		delete[] serverConfigs;
		this->_lineCount = rhs._lineCount;
		this->_serverCount = rhs._lineCount;
		serverConfigs = new ServerContext[_serverCount];
		for (int i = 0; i < this->_serverCount; ++i)
			serverConfigs[i] = rhs.serverConfigs[i];
		httpConfig = rhs.httpConfig;
	}
	return(*this);
}

std::vector<std::string>	ConfigParser::processFile(char *file)
{
	std::string					line;
	std::ifstream				confFile(file);
	std::vector<std::string>	allConfig;
	std::vector<std::string>::iterator	it;

	if (!confFile.is_open())
		throw ConfigParser::FileOpeningException();
	while (std::getline(confFile, line))
	{
		line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
		if (line.find("server {") == 0)
			this->_serverCount++;
		allConfig.push_back(line);
	}
	if (!checkBraceBalance(allConfig))
		throw ConfigParser::MissingBracesException();
	for (it = allConfig.begin(); it != allConfig.end(); ++it)
	{
		this->_lineCount++;
		(*it).erase(0, (*it).find_first_not_of(" \t\n\r\f\v"));
		if ((*it).empty() || (*it).find('#') == 0)
			continue;
		if (!isValidContextOrDirective((*it)))
			throw ConfigParser::InvalidContextOrDirective();
	}
	return(allConfig);
}


bool	ConfigParser::isValidContextOrDirective(const std::string& line)
{
	if (line.find("http {") == 0 || line.find("server {") == 0 || line.find("location ") == 0)
	{
		std::string lineSubString = line.substr(line.find('{') + 1);
		if (!lineSubString.empty())
		{
			lineSubString.erase(0, lineSubString.find_first_not_of(" \t\n\r\f\v"));
			if (!lineSubString.empty() && (lineSubString.find('#') != 0 || lineSubString.find('#') == std::string::npos))
				return (false);
		}
		return (true);
	}
	size_t	commentPos = line.find('#');
	if (commentPos != std::string::npos)
	{
		std::string	commentSubString = line.substr(commentPos);
		if (commentSubString.find('{') != std::string::npos ||
			commentSubString.find(';') != std::string::npos ||
			commentSubString.find('}') != std::string::npos ||
			commentSubString.find("server {") != std::string::npos ||
			commentSubString.find("http {") != std::string::npos ||
			commentSubString.find("location ") != std::string::npos)
			return (false);
	}
	if (line.find(';') != std::string::npos)
	{
		std::string lineSubString = line.substr(line.find(';') + 1);
		if (!lineSubString.empty())
		{
			lineSubString.erase(0, lineSubString.find_first_not_of(" \t\n\r\f\v"));
			if (!lineSubString.empty() && (lineSubString.find('#') != 0 || lineSubString.find('#') == std::string::npos))
				return (false);
		}
		return (true);
	}
	else if (line.find('}') != std::string::npos)
	{
		std::string lineSubString = line.substr(line.find('}') + 1);
		if (!lineSubString.empty())
		{
			lineSubString.erase(0, lineSubString.find_first_not_of(" \t\n\r\f\v"));
			if (!lineSubString.empty() && (lineSubString.find('#') != 0 || lineSubString.find('#') == std::string::npos))
				return (false);
		}
		return (true);
	}
	return (false);
}

bool	ConfigParser::checkBraceBalance(std::vector<std::string> allConfig)
{
	std::stack<char>	braces;
	std::vector<std::string>::iterator	it;

	for (it = allConfig.begin(); it != allConfig.end(); ++it)
	{
		if ((*it).empty() || (*it).find('#') == 0)
			continue;
		if ((*it).find('{') != std::string::npos)
		{
			if ((*it).find('#') != std::string::npos && (*it).find('{') > (*it).find('#'))
				continue;
			braces.push('{');
		}
		else if ((*it).find('}') == 0)
		{
			if ((*it).find('#') != std::string::npos && (*it).find('}') > (*it).find('#'))
				continue;
			if (braces.empty())
				return (false);
			braces.pop();
		}
	}
	return (braces.empty());
}

void	ConfigParser::setServerContext()
{
	this->serverConfigs = new ServerContext[_serverCount];
}

int	ConfigParser::getServerCount()
{
	return(this->_serverCount);
}

void	ConfigParser::parseDirective(std::string& directive, std::map<std::string, std::vector<std::string> >& contextMap)
{
	std::string	key, word;
	std::vector<std::string>	value;

	if (directive.find(';') != std::string::npos)
		directive.erase(directive.find(';'));
	std::istringstream	line(directive);
	line >> key;
	if (key == "error_page" || key == "return" || key == "cgi") {
		line >> word;
		key += word;
	}
	while (line >> word) {
		value.push_back(word);
	}
	if (key == "location")
		value.erase(value.end() - 1);
	contextMap.insert(std::make_pair(key, value));
}

void	ConfigParser::parseConfig(std::vector<std::string>& allConfig)
{
	
	std::stack<std::string>	contextControl;
	std::map<std::string, std::vector<std::string> >	currentConfig;
	std::map<std::string, std::vector<std::string> >	currentLocConfig;
	int	count = -1;

	std::vector<std::string>::iterator	it;
	for (it = allConfig.begin(); it != allConfig.end(); ++it)
	{
		std::string	line = *it;
		if (line.empty() || line.find('#') == 0)
			continue;
		else if (line.find("http {") == 0 || line.find("server {") == 0 || line.find("location ") == 0)
		{
			line = line.erase(line.find('{') + 1);
			contextControl.push(line);
			if (line == "server {")
				count++;
		} else if (line == "}") {
			if (contextControl.top() == "server {") {
				serverConfigs[count].serverConfig.swap(currentConfig);
				currentConfig = std::map<std::string, std::vector<std::string> >();
			} else if (contextControl.top().find("location ") == 0) {
				parseDirective(contextControl.top(), currentLocConfig);
				serverConfigs[count].locationConfig.push_back(currentLocConfig);
				currentLocConfig = std::map<std::string, std::vector<std::string> >();
			}
			contextControl.pop();
		} else {
			if (contextControl.top() == "http {") {
				parseDirective(line, httpConfig);
			} else if (contextControl.top() == "server {") {
				parseDirective(line, currentConfig);
			} else if (contextControl.top().find("location ") == 0) {
				parseDirective(line, currentLocConfig);
			}
		}
	}
}

void	ConfigParser::configParser(char *file)
{
	std::vector<std::string>	allConfig;

	allConfig = processFile(file);
	std::cout << "processing done\n"; 
	setServerContext();
	std::cout << "context setting done\n"; 
	parseConfig(allConfig);
	std::cout << "parsing done\n"; 
}
