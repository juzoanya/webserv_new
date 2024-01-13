/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:51 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/13 16:16:55 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "headers.hpp"

class ResponseHandler;

class RequestHandler : public ConfigParser, public HttpConfig, public ConfigHandler
{
	private:
	
	public:
		RequestHandler();
		~RequestHandler();

		std::vector<char>	handleRequest(const std::string& request);
		static std::string	getRequestMethod(const std::string& request);
		static std::string	getRequestPath(const std::string& request);
		static std::string	getServerName(const std::string& request);
		// static std::string	handleGetRequest(const std::string& path);
		// static std::string	generateErrorResponse();

};


#endif