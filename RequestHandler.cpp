/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:40 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/16 07:40:01 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}


std::vector<char>	RequestHandler::handleRequest(const std::string& request)
{
	ConfigHandler	handler;
	int servCount = ConfigParser::getServerCount();
	std::string	serverName = getServerName(request);
	std::string	path = getRequestPath(request);

	for (int i = 0; i < servCount; ++i)
		handler.addServerConfig(&ConfigParser::serverConfigs[i]);
	

	// for (int i = 0; i != this->getServerCount(); ++i) {
	// 	ServerContext currServerConfig = this->serverConfigs[i];
	// 	printConfigMap("Server Config: ", currServerConfig.serverConfig);
	// 	std::vector<ws_config_t>::iterator	it;
	// 	for (it = currServerConfig.locationConfig.begin(); it != currServerConfig.locationConfig.end(); ++it)
	// 		printConfigMap("Location Config: ", *it);
	// }

	HttpConfig::printDirective();

	//get method
		//if method not found update status code
	
	//get header uri
	
	//Get the serverconfig and location config for the request
		//check against the sever_name and port
		//chec if header uri is present in confi location
			//if not update status code

	//Using the server and location config, process the response
	std::cout << "1-------------------------------" << std::endl;

	HttpConfig	httpConfig = handler.getHttpConfig(path, serverName, "");
	if (!httpConfig.checkAllowedMethod(getRequestMethod(request)))
		httpConfig.getErrorPage("405");
	

	std::vector<std::string>	indexes = httpConfig.getIndexFile();
	std::vector<std::string>::iterator it;
	for (it = indexes.begin(); it != indexes.end(); ++it)
		std::cout << *it << std::endl;

	std::cout << "2-------------------------------" << std::endl;
	HttpStatic	responsePage;
	responsePage.setContentByPath(path, getRequestPath(request), httpConfig.getIndexFile(), httpConfig.hasDirectoryListing());

	std::cout << "3-------------------------------" << std::endl;
	return (responsePage.fileData);
}

std::string	RequestHandler::getRequestMethod(const std::string& request)
{
	size_t pos = request.find(' ');
	if (pos != std::string::npos)
		return (request.substr(0, pos));
	return (NULL);
}

std::string	RequestHandler::getRequestPath(const std::string& request)
{
	size_t start = request.find(' ') + 1;
	size_t end = request.find(' ', start);
	if (end != std::string::npos)
		return (request.substr(start, end - start));
	return (NULL);
}

std::string	RequestHandler::getServerName (const std::string& request)
{
	size_t end;
	size_t pos = request.find("Host:");
	if (pos != std::string::npos)
	{
		std::string	partString = request.substr(pos + 5);
		end = partString.find("\r\n");
		if (end != std::string::npos)
			return (partString.substr(0, end));
	}
	return (NULL);
}

// std::string	RequestHandler::handleGetRequest(const std::string& path)
// {
// 	std::vector<std::string>	indexes;
// 	std::vector<std::string>	root;
// 	ResponseHandler				response;

// 	indexes.push_back("index.html");
// 	indexes.push_back("index.htm");
// 	indexes.push_back("index.php");
// 	// TODO: check for the correct dir for the server to handle the request using header and parse it to root
// 	root.push_back("/home/jakes/webserv_new/");

// 	if (path == "/")
// 	{
// 		std::vector<std::string>::iterator it;
// 		for (it = indexes.begin(); it != indexes.end(); ++it)
// 		{
// 			std::string	fullPath = root[0] + *it;
// 			std::cout << fullPath << std::endl;
// 			return (response.getStaticPage(fullPath));
// 		}
// 	}
// 	else if (isDirectory(path.substr(1)))
// 		std::cout << "This is a directory" << std::endl; //TODO: handle directory listing
// 	else
// 		return (response.getStaticPage(path.substr(1)));
// 	return ("");
// }

