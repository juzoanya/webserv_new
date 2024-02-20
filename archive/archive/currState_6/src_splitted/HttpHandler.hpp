/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 19:29:11 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"

class HttpContent {
    public:
        HttpContent( void );
        ~HttpContent( void );
        int     sendDataToSocket( int socketfd, int flags );
        ws_http::statuscodes_t  setResponseDefaultError( ws_http::statuscodes_t errorStatusCode );
        ws_http::statuscodes_t  setResponseRedirection( ws_http::statuscodes_t status, std::string const & location );
        ws_http::statuscodes_t  setResponseStatic( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
        ws_http::statuscodes_t  appendHeaderByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
        ws_http::statuscodes_t  appendContentByBuffer( buff_t::const_iterator itStart, buff_t::const_iterator itEnd, bool flush );
        void                    setContentExternalBuffer( buff_t const & buffer );
        void                    setContent( std::istream& httpBodyStream, std::string const & mimeType );
        void                    setStatus( ws_http::statuscodes_t status );
        void                    setHeader( std::string const & key, std::string const & value );
        void                    flushResponse( void );
        bool    empty( void ) const;
    private:
        const char* headerPtr;
        int         headerSize;
        const char* bodyPtr;
        int         bodySize;
        buff_t      _headerBuffer;
        buff_t      _bodyBuffer;
};

class AHttpHandler {
    public:
        AHttpHandler( void );
        AHttpHandler( HttpContent const & data );
        virtual ~AHttpHandler( void );
        HttpContent httpContent;
        virtual ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end ) = 0;
};

class HttpHandler : public AHttpHandler {
    public:
        class CgiHandler;

        HttpHandler( ConfigHandler& configHandler, WsIpPort const & ipPort );
        ~HttpHandler( void );

        const WsIpPort      ipPortData;
        CgiHandler *            getCgiProcessInstance( void );
        virtual ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
        // ws_http::statuscodes_t  setError( ws_http::statuscodes_t errorStatusCode, bool internal );
    protected:
        // ws_http::statuscodes_t  setHeader( ws_http::statuscodes_t statusCode, std::istream* httpBodyStream, std::string const & mimeType, std::string const & redirectLocation="" );
        // ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, bool internal );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        ws_http::statuscodes_t  processResponseFile( HttpConfig const & config, ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, int redirNo );
        HttpRequest         _request;
        // buff_t              _headerBuffer;
        // buff_t              _fileData;
        // HttpConfig          _httpConfig;
        ConfigHandler*      _configHandler;
        CgiHandler*         _cgiProcess;
        // int                 _intRedirectNbr;

};

class HttpHandler::CgiHandler : public AHttpHandler {
    public:
        CgiHandler( WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, HttpContent & clientResponseBuffer );
        // CgiHandler( WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, buff_t & resultBodyBuffer, buff_t & resultHeaderBuffer );
        ~CgiHandler( void );
        int createChildProcess( void );
        ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
        static int parentsock;
        static int childsock;

        HttpRequest                 _requestHelper;
        // HttpHandler &               _httpHandler;
        // buff_t &                    _resultHeaderBuffer;
        // buff_t &                    _resultBodyBuffer;
        HttpContent & _clientResponseBuffer;
        std::vector<std::string>    _cgiArgs;
        std::vector<std::string>    _cgiEnv;
        std::vector<char*>          _arguments;
        std::vector<char*>          _enviroment;
};




// class HttpCgiHandler {
//     public:
//         HttpCgiHandler( void );
//         ~HttpCgiHandler( void );
//     private:

// };

// class HttpStaticHandler {
//     public:
//         HttpStaticHandler( void );
//         ~HttpStaticHandler( void );
//     private:

// };

#endif