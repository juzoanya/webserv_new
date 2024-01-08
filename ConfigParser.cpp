
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
			return (true);
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
	if (line.find(';') != std::string::npos || line.find('}') != std::string::npos)
		return (true);
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
			braces.push('{');
		else if ((*it).find('}') == 0)
		{
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


// void	ConfigParser::parseConfig(std::vector<std::string>& allConfig)
// {
	
// 	std::stack<std::string>	contextControl;
// 	std::map<std::string, std::vector<std::string> >	currentConfig;
// 	std::map<std::string, std::vector<std::string> >	currentLocConfig;
// 	int	count = -1;

// 	std::vector<std::string>::iterator	it;
// 	for (it = allConfig.begin(); it != allConfig.end(); ++it)
// 	{
// 		std::cout << "\nfor -\n";
// 		if (contextControl.size() > 0)
// 			std::cout << "contextControl: " << contextControl.top() << std::endl;
// 		std::cout << "stack elem: " << *it << std::endl;


// 		std::string	line = *it;
// 		if (line.empty() || line.find('#') == 0)
// 			continue;
// 		else if (line == "http {" || line == "server {" || line.find("location ") == 0) {
// 			contextControl.push(line);
// 			if (line == "server {")
// 				count++;
// 		} else if (line == "}") {
// 			if (contextControl.top() == "server {") {
// 				serverConfigs[count].serverConfig.swap(currentConfig);
// 				currentConfig = std::map<std::string, std::vector<std::string> >();
// 			} else if (contextControl.top().find("location ") == 0) {
// 				std::string locationRoute = contextControl.top().substr(8, contextControl.top().size() - 9);
// 				locationRoute.erase(remove_if(locationRoute.begin(), locationRoute.end(), isspace), locationRoute.end());
// 				serverConfigs[count].locationConfig.push_back(std::make_pair(locationRoute, currentLocConfig));
// 				currentLocConfig = std::map<std::string, std::vector<std::string> >();
// 			}
// 			contextControl.pop();
// 		} else {
// 			if (contextControl.top() == "http {") {
// 				std::cout << "parse Directive - http" << std::endl;
// 				parseDirective(line, httpConfig);
// 			} else if (contextControl.top() == "server {") {
// 				std::cout << "parse Directive - server" << std::endl;
// 				parseDirective(line, currentConfig);
// 			} else if (contextControl.top().find("location ") == 0) {
// 				std::cout << "parse Directive - location" << std::endl;
// 				parseDirective(line, currentLocConfig);
// 			}
// 		}
// 	}
// }


void	ConfigParser::parseDirective(std::string& directive, std::map<std::string, std::vector<std::string> >& contextMap)
{
	std::string	key, word;
	std::vector<std::string>	value;

	if (directive.find(';') != std::string::npos)
		directive.erase(directive.end() - 1);
	std::istringstream	line(directive);
	line >> key;
	if (key == "error_page" || key == "return" || key == "location") {
		line >> word;
		if (!(key == "location" && word == "="))
			key = key + word;
	}
	while (line >> word) {
		value.push_back(word);
	}
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
		std::cout << "\nfor -\n";
		if (contextControl.size() > 0)
			std::cout << "contextControl: " << contextControl.top() << std::endl;
		std::cout << "stack elem: " << *it << std::endl;


		std::string	line = *it;
		if (line.empty() || line.find('#') == 0)
			continue;
		else if (line == "http {" || line == "server {" || line.find("location ") == 0) {
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
				std::cout << "parse Directive - http" << std::endl;
				parseDirective(line, httpConfig);
			} else if (contextControl.top() == "server {") {
				std::cout << "parse Directive - server" << std::endl;
				parseDirective(line, currentConfig);
			} else if (contextControl.top().find("location ") == 0) {
				std::cout << "parse Directive - location" << std::endl;
				parseDirective(line, currentLocConfig);
			}
		}
	}
}


void	printConfigMap( std::string const & msg, std::map<std::string, std::vector<std::string> > const & config )
{
	std::cout << std::endl;
	std::cout << msg << std::endl;
	std::map<std::string, std::vector<std::string> >::const_iterator it;
	for (it = config.begin(); it != config.end(); ++it) {
		std::cout << "Key: " << std::setw(20) << it->first;
		std::cout << " |\tValues: ";
		for (std::size_t i = 0; i != it->second.size(); ++i) {
			std::cout << (it->second)[i] << ", ";
		}
		std::cout << std::endl;
	}
}

void	ConfigParser::configParser(char *file)
{
	std::vector<std::string>	allConfig;

	allConfig = processFile(file);

	for (std::size_t i = 0; i != allConfig.size(); ++i) {
		std::cout << "callConfig string: " << allConfig[i] << std::endl;
	}

	setServerContext();

	parseConfig(allConfig);

	for (int i = 0; i != this->getServerCount(); ++i) {
		ServerContext currServerConfig = this->serverConfigs[i];
		printConfigMap("Server Config: ", currServerConfig.serverConfig);
		for (std::size_t j = 0; j != currServerConfig.locationConfig.size(); ++j) {
			printConfigMap("\tLocation Config: " + currServerConfig.locationConfig[j].at("location").at(0) + ": ", currServerConfig.locationConfig[j]);
		}
	}
}




// int	main(int argc, char** argv)
// {
// 	if (argc != 2) {
// 		std::cerr << "invalid number of arguments";
// 		return (1);
// 	}
// 	ConfigParser parser;
// 	parser.configParser(argv[1]);

// 	std::cout << "server number: " << parser.getServerCount() << std::endl;



// 	return (0);
// }
