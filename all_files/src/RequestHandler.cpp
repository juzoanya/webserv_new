/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:40 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/18 21:05:00 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "headers.hpp"

// RequestHandler::RequestHandler()
// {}

// RequestHandler::~RequestHandler()
// {}


// std::vector<char>	RequestHandler::handleRequest(const std::string& request, ConfigParser parser)
// {
// 	ConfigHandler	handler;
// 	std::string		host = getHost(request);
// 	std::string		requestUri = getRequestUri(request);

// 	std::cout << host << " >>>>>>>>\n";

// 	ConfigParser::ServerContext	handlerServerConfig = parser.getHandlerServer(host);

// 	// ws_config_t::iterator it;
// 	// it = handlerServerConfig.serverConfig.find("server_name");
// 	// if (it != handlerServerConfig.serverConfig.end())
// 	// {
// 	// 	std::vector<std::string> value = it->second;
// 	// 	std::vector<std::string>::iterator itv;
// 	// 	for (itv = value.begin(); itv != value.end(); ++itv)
// 	// 		std::cout << *itv << " | ";
// 	// }



// 	// handler.addServerConfig(&handlerServerConfig);

// 	//get method
// 		//if method not found update status code
	
// 	//get header uri
	
// 	//Get the serverconfig and location config for the request
// 		//check against the sever_name and port
// 		//chec if header uri is present in confi location
// 			//if not update status code

// 	//Using the server and location config, process the response
// 	std::cout << "1-------------------------------" << std::endl;

// 	HttpConfig	httpConfig;// = handler.getHttpConfig(path, serverName, "");

// 	httpConfig.setHandlerServerConfig(handlerServerConfig, requestUri);

// 	std::cout << "1.1-------------------------------" << std::endl;
// 	// if (requestUri == "/")
// 	// 	httpConfig
	
// 	std::cout << "1.2------------------------------" << std::endl;
// 	// if (!httpConfig.checkAllowedMethod(getRequestMethod(request)))
// 	// 	httpConfig.getErrorPage("405");
	

// 	// std::vector<std::string>	indexes = httpConfig.getIndexFile();
// 	// std::vector<std::string>::iterator itt;
// 	// for (itt = indexes.begin(); itt != indexes.end(); ++itt)
// 	// 	std::cout << *itt << std::endl;

// 	std::cout << "2-------------------------------" << std::endl;
// 	HttpStatic	responsePage;
// 	//responsePage.setContentByPath(path, getRequestPath(request), httpConfig.getIndexFile(), httpConfig.hasDirectoryListing());

// 	std::cout << "3-------------------------------" << std::endl;
// 	return (responsePage.fileData);
// }

// std::string	RequestHandler::getRequestMethod(const std::string& request)
// {
// 	size_t pos = request.find(' ');
// 	if (pos != std::string::npos)
// 		return (request.substr(0, pos));
// 	return (NULL);
// }

// std::string	RequestHandler::getRequestUri(const std::string& request)
// {
// 	size_t start = request.find(' ') + 1;
// 	size_t end = request.find(' ', start);
// 	if (end != std::string::npos)
// 		return (request.substr(start, end - start));
// 	return (NULL);
// }

// std::string	RequestHandler::getHost (const std::string& request)
// {
// 	size_t end;
// 	size_t pos = request.find("Host:");
// 	if (pos != std::string::npos)
// 	{
// 		std::string	partString = request.substr(pos + 5);
// 		end = partString.find("\r\n");
// 		if (end != std::string::npos)
// 			return (partString.substr(0, end));
// 	}
// 	return (NULL);
// }






#include "headers.hpp"

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}


std::string	RequestHandler::handleRequest(const std::string& request)
{
	std::string	method = getRequestMethod(request);
	std::string	path = getRequestPath(request);
	std::cout << method << " | " << path << std::endl;

	return ("");
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

std::string	RequestHandler::handleGetRequest(const std::string& path)
{
	std::vector<std::string>	indexes;
	std::vector<std::string>	root;
	ResponseHandler				response;

	indexes.push_back("index.html");
	indexes.push_back("index.htm");
	indexes.push_back("index.php");
	// TODO: check for the correct dir for the server to handle the request using header and parse it to root
	root.push_back("/home/jakes/webserv_new/");

	if (path == "/")
	{
		std::vector<std::string>::iterator it;
		for (it = indexes.begin(); it != indexes.end(); ++it)
		{
			std::string	fullPath = root[0] + *it;
			std::cout << fullPath << std::endl;
			return (response.getStaticPage(fullPath));
		}
	}
	else if (isDirectory(path.substr(1)))
		std::cout << "This is a directory" << std::endl; //TODO: handle directory listing
	else
		return (response.getStaticPage(path.substr(1)));
	return ("");
}

bool	RequestHandler::fileExists(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	return (file.good());
}

bool	RequestHandler::isDirectory(const std::string& path)
{
	struct stat	fileStat;
	const char* constPath = path.c_str();

	if (stat(constPath, &fileStat) != 0)
		return (false);
	return (S_ISDIR(fileStat.st_mode));
}