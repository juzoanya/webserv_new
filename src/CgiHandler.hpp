/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 18:41:56 by mberline         ###   ########.fr       */
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
		bool    createChildProcess( std::string const & binPath, std::string const & chToDir );
		void    handleEvent( struct pollfd & pollfd );
		void    addArgument( std::string const & argument );
		void    addEnvVariable( std::string const & envVar );
	private:

		static int childsock;
		static int parentsock;

		HttpHandler&            	_parent;
		std::vector<std::string>    _cgiArgs;
		std::vector<std::string>    _cgiEnv;
};


#endif
