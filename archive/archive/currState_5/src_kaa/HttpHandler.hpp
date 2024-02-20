/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/05 19:27:00 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"

class HttpContent {
    public:
        HttpContent( void );
        HttpContent( const buff_t* headerBuffer, const buff_t* bodyBuffer );
        int     sendDataToSocket( int socketfd, int flags );
        bool    empty( void ) const;
        ws_http::statuscodes_t  setContent( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
        ws_http::statuscodes_t  setDefaultError( ws_http::statuscodes_t status );
        ws_http::statuscodes_t  setRedirection( ws_http::statuscodes_t status, std::string const & location );
        ~HttpContent( void );
    private:
        const char* headerPtr;
        int         headerSize;
        const char* bodyPtr;
        int         bodySize;
        buff_t      _bodyBuffer;
        buff_t      _headerBuffer;
};

class AHttpHandler {
    public:
        AHttpHandler( void );
        AHttpHandler( HttpContent const & data );
        HttpContent httpContent;
        // int     sendDataToSocket( int socketfd, int flags );
        // bool    empty( void ) const;
        // virtual ~AHttpHandler( void );
        // const char* headerPtr;
        // int         headerSize;
        // const char* dataPtr;
        // int         dataSize;

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
        ws_http::statuscodes_t  setError( ws_http::statuscodes_t errorStatusCode, bool internal );
    protected:
        ws_http::statuscodes_t  setHeader( ws_http::statuscodes_t statusCode, std::istream* httpBodyStream, std::string const & mimeType, std::string const & redirectLocation="" );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, bool internal );
        HttpRequest         _request;
        buff_t              _headerBuffer;
        buff_t              _fileData;
        HttpConfig          _httpConfig;
        ConfigHandler*      _configHandler;
        int                 _intRedirectNbr;

};

class HttpHandler::CgiHandler : public AHttpHandler {
    public:
        CgiHandler( HttpHandler & httpHandler, HttpRequest & request, buff_t & resultBodyBuffer, buff_t & resultHeaderBuffer );
        ~CgiHandler( void );
        int createChildProcess( void );
        ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
        static int parentsock;
        static int childsock;

        HttpRequest                 _requestHelper;
        HttpHandler &               _httpHandler;
        buff_t &                    _resultHeaderBuffer;
        buff_t &                    _resultBodyBuffer;
        std::vector<std::string>    _cgiArgs;
        std::vector<std::string>    _cgiEnv;
        // std::vector<char*>          _arguments;
        // std::vector<char*>          _enviroment;
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