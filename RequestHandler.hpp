/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:51 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/11 16:10:39 by juzoanya         ###   ########.fr       */
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
		std::string	_host;
		std::string	_port;
		int	_handlerConfigIndex;
		ws_config_t	_handlerServConfig;
		std::vector<ws_config_t>	_handlerLocConfig;
		
	
	public:
		RequestHandler();
		~RequestHandler();

		std::string	handleRequest(const std::string& request);
		static std::string	getRequestMethod(const std::string& request);
		static std::string	getRequestPath(const std::string& request);
		static std::string	getServerName(const std::string& request);
		static std::string	handleGetRequest(const std::string& path);
		static std::string	generateErrorResponse();
		static std::string	servePage(const std::string& path);
		static bool	isDirectory(const std::string& path);
		static bool	fileExists(const std::string& filename);
};


#endif