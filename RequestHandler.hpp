/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 11:11:51 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/04 15:40:36 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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