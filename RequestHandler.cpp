


#include "headers.hpp"

RequestHandler::RequestHandler()
{}

RequestHandler::~RequestHandler()
{}


std::string	RequestHandler::handleRequest(const std::string& request)
{
	std::string	method = getRequestMethod(request);
	std::string	path = getRequestPath(request);

	if (method == "GET")
		return (handleGetRequest(path));
	else
		return (generateErrorResponse());
}