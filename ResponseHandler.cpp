/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 15:22:48 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/04 17:25:32 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

ResponseHandler::ResponseHandler()
{
}

ResponseHandler::~ResponseHandler()
{
}

std::string	ResponseHandler::getStaticPage(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file)
	{
		return (generateErrorResponse(404, "Not Found"));
	}

	std::ostringstream content;
	content << file.rdbuf();
	file.close();

	std::string	contentType = getContentType(filename);
	return (generateSuccessResponse(content.str(), contentType));
}

std::string ResponseHandler::generateErrorResponse(int errorCode, const std::string errormessage)
{
	std::ostringstream	errorResponse;
	errorResponse << "HTTP/1.1 " << errorCode << " " << errormessage << "\r\n";
	errorResponse << "Content-Length: 0\r\n";
	errorResponse << "\r\n";
	return (errorResponse.str());
}

std::string ResponseHandler::getContentType(const std::string& filename)
{
	size_t	dotPos = filename.find_last_of('.');
	if (dotPos != std::string::npos)
	{
		std::string	extension = filename.substr(dotPos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (extension == "html")
			return ("text/html");
		
	}
	return ("application/octet-stream");
}

std::string ResponseHandler::generateSuccessResponse(const std::string& content, const std::string& contentType)
{
	std::ostringstream	response;
	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: " << contentType << "\r\n";
	response << "Content-Length: " << content.size() << "\r\n";
	response << "\r\n";
	response << content;
	return (response.str());
}
