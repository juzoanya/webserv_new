/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:51 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/18 21:04:22 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// #pragma once
// #ifndef REQUESTHANDLER_HPP
// #define REQUESTHANDLER_HPP

// #include "headers.hpp"

// class ResponseHandler;

// class RequestHandler : /*public ConfigParser,*/ public HttpConfig, public ConfigHandler
// {
// 	private:
	
// 	public:
// 		RequestHandler();
// 		~RequestHandler();

// 		std::vector<char>	handleRequest(const std::string& request, ConfigParser parser);
// 		static std::string	getRequestMethod(const std::string& request);
// 		static std::string	getRequestUri(const std::string& request);
// 		static std::string	getHost(const std::string& request);
// 		// static std::string	handleGetRequest(const std::string& path);
// 		// static std::string	generateErrorResponse();

// };


// #endif

#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "headers.hpp"

class ResponseHandler;

class RequestHandler
{
	private:
		/* data */
	public:
		RequestHandler();
		~RequestHandler();

		static std::string	handleRequest(const std::string& request);
		static std::string	getRequestMethod(const std::string& request);
		static std::string	getRequestPath(const std::string& request);
		static std::string	handleGetRequest(const std::string& path);
		static std::string	generateErrorResponse();
		static std::string	servePage(const std::string& path);
		static bool	isDirectory(const std::string& path);
		static bool	fileExists(const std::string& filename);
};


#endif