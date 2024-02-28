/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 20:18:18 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "headers.hpp"

class HttpHandler;
class HttpContent;

class ChildProcessHandler : public APollEventHandler {
	public:
		ChildProcessHandler( Polling & polling, HttpHandler& parent );
		~ChildProcessHandler( void );
		void    createChildProcess( std::string const & binPath, std::string const & chToDir );
		void    handleEvent( struct pollfd pollfd );
		void    addArgument( std::string const & argument );
		void    addEnvVariable( std::string const & envVar );
		void	terminateChildProcess( void );
	private:

		static int childsock;
		static int parentsock;
		int	_childPid;

		HttpHandler&            	_parent;
		std::vector<std::string>    _cgiArgs;
		std::vector<std::string>    _cgiEnv;
};


#endif
