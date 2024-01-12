/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:40 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/11 19:41:03 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}


std::string	RequestHandler::handleRequest(const std::string& request)
{
	std::cout << "**************************\n" << request << "**************************\n" << std::endl;
	std::string	serverName = getServerName(request);
	std::cout << serverName << "\n<------------------>" << std::endl;

	size_t pos = serverName.find(':');
	if (pos != std::string::npos)
	{
		this->_host = serverName.substr(0, pos);
		this->_port = serverName.substr(pos + 1);
	}
	else
	{
		this->_host = serverName;
		this->_port = "";
	}
	std::cout << this->_host << " | " << this->_port << "\n====================" << std::endl;


	std::string	path = getRequestPath(request);


	//get method
		//if method not found update status code
	
	//get header uri
	
	//Get the serverconfig and location config for the request
		//check against the sever_name and port
		//chec if header uri is present in confi location
			//if not update status code

	//Using the server and location config, process the response


	HttpConfig	httpConfig = ConfigHandler::getHttpConfig(path, serverName, "/var/www");

	this->_handlerConfigIndex = ConfigParser::getHandlerConfigIndex(this->_host, this->_port);
	std::cout << this->_handlerConfigIndex << std::endl;

	// this->_handlerServConfig = ConfigParser::serverConfigs[this->_handlerConfigIndex].serverConfig;
	// this->_handlerLocConfig = ConfigParser::serverConfigs[this->_handlerConfigIndex].locationConfig;



	// std::vector<ws_config_t>::iterator	it;
	// for (it = this->_handlerLocConfig.begin(); it != this->_handlerLocConfig.end(); ++it)
	// {
		
	// }
	

	// if (HttpConfig::checkAllowedMethod(getRequestMethod(request)))
	// {
	// 	std::cout << "Method Found" << std::endl;
	// }
	


	// std::string	method = getRequestMethod(request);
	// std::string	path = getRequestPath(request);

	// std::vector<std::string>	methods;
	// methods.push_back("GET");
	// methods.push_back("POST");
	// methods.push_back("DELETE");

	// std::vector<std::string>::iterator	it;
	// for (it = methods.begin(); it != methods.end(); ++it)
	// {
	// 	if (*it == method)
	// 	{
	// 		if (method == "GET")
	// 			return (handleGetRequest(path));
	// 		else if (method == "POST")
	// 			return ("POST");
	// 		else if (method == "DELETE")
	// 			return ("DELETE");
	// 	}
	// 	else
	// 		return ("Error");//TODO: generate error response for no available method
	// }
	// std::cout << "HR, Done" << std::endl;
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