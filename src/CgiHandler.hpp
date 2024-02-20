/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/19 21:38:44 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "headers.hpp"

class HttpHandler;
class HttpContent;


// class IParentProcess {
//     public:
//         IParentProcess( void ) {};
//         virtual ~IParentProcess( void ) {};
//         virtual void    handleChildEvent( struct pollfd & pollfd ) = 0;
//     private:
// };

class ChildProcessHandler : public APollEventHandler {
    public:
        ChildProcessHandler( Polling & polling, HttpHandler& parent );
        ~ChildProcessHandler( void );
        int     createChildProcess( std::string const & binPath );
        // int     closeWriteSide( void );
        // int     closeReadSide( void );
        void    handleEvent( struct pollfd & pollfd );
        void    addArgument( std::string const & argument );
        void    addEnvVariable( std::string const & envVar );
    private:
        static int parentsock;
        static int childsock;
        HttpHandler&            _parent;
        std::vector<std::string>    _cgiArgs;
        std::vector<std::string>    _cgiEnv;
        std::vector<char*>          _arguments;
        std::vector<char*>          _enviroment;
};


#endif
