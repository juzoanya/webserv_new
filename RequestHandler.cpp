/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:40 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/05 09:21:38 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

	std::vector<std::string>	methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	std::vector<std::string>::iterator	it;
	for (it = methods.begin(); it != methods.end(); ++it)
	{
		if (*it == method)
		{
			if (method == "GET")
				return (handleGetRequest(path));
			else if (method == "POST")
				return ("POST");
			else if (method == "DELETE")
				return ("DELETE");
		}
		else
			return ("Error");//TODO: generate error response for no available method
	}
	std::cout << "HR, Done" << std::endl;
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