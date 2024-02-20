/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:17:56 by juzoanya          #+#    #+#             */
/*   Updated: 2024/02/12 19:35:34 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "headers.hpp"

class CgiHandler
{
	private:
		char const **_argv;
		struct pollfd	*_pollFd;
		char const **_env;
		int	_inputFd[2];
		int	_outputFd[2];

		void	setupEnv();

	public:
		CgiHandler();
		CgiHandler(struct pollfd& pollfd, const std::string& request);
		~CgiHandler();
		CgiHandler(const CgiHandler& src);
		CgiHandler&	operator=(const CgiHandler& rhs);

		void	handleCgi(int clientSocket);
		void	processRequest();
};

#endif