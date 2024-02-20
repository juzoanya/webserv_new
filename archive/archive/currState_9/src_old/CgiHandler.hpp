/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:01 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 22:19:35 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "headers.hpp"

class HttpHandler;
class HttpContent;


class IParentProcess {
    public:
        IParentProcess( void ) {};
        virtual ~IParentProcess( void ) {};
        virtual void    closeChildProcess( int bytesSend ) = 0;
        virtual void    readFromChildProcess( int socketfd ) = 0;
    private:
};

class ChildProcessHandler : public APollEventHandler {
    public:
        ChildProcessHandler( Polling & polling, const void * dataPtr, std::size_t dataSize );
        ~ChildProcessHandler( void );
        int     createChildProcess( std::string const & binPath );
        void    handleEvent( struct pollfd & pollfd );
        void    addArgument( std::string const & argument );
        void    addEnvVariable( std::string const & envVar );
    private:
        static int parentsock;
        static int childsock;
        const unsigned char*        _dataToSendPtr;
        std::size_t                 _dataToSendSize;
        std::vector<std::string>    _cgiArgs;
        std::vector<std::string>    _cgiEnv;
        std::vector<char*>          _arguments;
        std::vector<char*>          _enviroment;
};









// class ChildProcessHandler : public APollEventHandler {
//     public:
//         ChildProcessHandler( Polling & polling );
//         // CgiHandler( Polling & polling, WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, HttpContent & clientResponseBuffer );
//         ~ChildProcessHandler( void );
//         int     createChildProcess( void );
//         void    handleEvent( struct pollfd & pollfd );
//         void    setCgiBinaryAndScriptPath( std::string const & cgiExecutablePath, std::string const & cgiScriptPath );
//         void    setScriptInfos( std::string const & scriptName, std::string const & scriptFileName,
//                                         std::string const & pathInfo, std::string const & pathTranslated );
//         void    setProtocolInfos( std::string const & httpProtocol, std::string const & cgiProtocol );
//         void    setRequestPathInfos( std::string const & requestQuery, std::string const & requestFullUriEncoded );
//         void    setRequestMethodAndContentInfos( std::string const & method, std::string const & contentType, std::string const & contentLength);
//         void    setServerInfos( std::string const & serverIp, std::string const & serverPort,
//                                 std::string const & serverName, std::string const & serverSoftware );
//         void    setClientInfos( std::string const & clientIp, std::string const & clientPort );

//     private:
//         static int parentsock;
//         static int childsock;

//         // HttpRequest                 _requestHelper;
//         // // HttpHandler&                _httpHandler;
//         // HttpContent &               _clientResponseBuffer;
//         std::vector<std::string>    _cgiArgs;
//         std::vector<std::string>    _cgiEnv;
//         std::vector<char*>          _arguments;
//         std::vector<char*>          _enviroment;
// };

// class CgiHandler : public APollEventHandler {
//     public:
//         CgiHandler( Polling & polling, WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, HttpContent & clientResponseBuffer );
//         ~CgiHandler( void );
//         int     createChildProcess( void );
//         void    handleEvent( struct pollfd & pollfd );
//         void    setScriptInfos( std::string const & scriptName, std::string const & scriptFileName,
//                                         std::string const & pathInfo, std::string const & pathTranslated );
//         void    setProtocolInfos( std::string const & httpProtocol, std::string const & cgiProtocol );
//         void    setRequestPathInfos( std::string const & requestQuery, std::string const & requestFullUriEncoded );
//         void    setRequestMethodAndContentInfos( std::string const & method, std::string const & contentType, std::string const & contentLength);
//         void    setServerInfos( std::string const & serverIp, std::string const & serverPort,
//                                 std::string const & serverName, std::string const & serverSoftware );
//         void    setClientInfos( std::string const & clientIp, std::string const & clientPort );

//     private:
//         static int parentsock;
//         static int childsock;

//         // HttpRequest                 _requestHelper;
//         // // HttpHandler&                _httpHandler;
//         // HttpContent &               _clientResponseBuffer;
//         std::vector<std::string>    _cgiArgs;
//         std::vector<std::string>    _cgiEnv;
//         std::vector<char*>          _arguments;
//         std::vector<char*>          _enviroment;
// };

#endif