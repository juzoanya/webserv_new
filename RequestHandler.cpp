


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
			return ("Error");//generateErrorResponse());
	}
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
	root.push_back("~/");

	if (path == "/")
	{
		std::vector<std::string>::iterator it;
		for (it = indexes.begin(); it != indexes.end(); ++it)
		{
			std::string	absolutePath = root[0] + *it;
			std::cout << absolutePath << std::endl;
			if (fileExists(absolutePath))
				return (response.getStaticPage(absolutePath));
		}
	}
	else
		return (response.getStaticPage(path.substr(1)));
	return (NULL);
}

bool	RequestHandler::fileExists(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	return (file.good());
}