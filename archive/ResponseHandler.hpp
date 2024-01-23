/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 15:22:54 by juzoanya          #+#    #+#             */
/*   Updated: 2023/12/28 15:27:20 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "headers.hpp"

class ResponseHandler
{
	private:
		/* data */
	public:
		ResponseHandler();
		~ResponseHandler();

		std::string	getStaticPage(const std::string& filename);
		std::string	generateErrorResponse(int errorCode, const std::string errormessage);
		std::string	getContentType(const std::string& filename);
		std::string	generateSuccessResponse(const std::string& content, const std::string& contentType);
};

#endif
