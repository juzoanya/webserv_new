/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/05 17:15:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"



class HttpContent {
    public:
        HttpContent( void );
        HttpContent( const buff_t* headerBuffer, const buff_t* bodyBuffer );
        HttpContent( ws_http::statuscodes_t statusCode );
        ~HttpContent( void );
        int     sendDataToSocket( int socketfd, int flags );
        // ws_http::statuscodes_t  setContent( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
        // ws_http::statuscodes_t  setDefaultError( ws_http::statuscodes_t status );
        // ws_http::statuscodes_t  setRedirection( ws_http::statuscodes_t status, std::string const & location );
        bool    empty( void ) const;
    private:
        const char* headerPtr;
        int         headerSize;
        const char* bodyPtr;
        int         bodySize;
        const char* dataPtr;
        int         dataSize;
        buff_t      _bodyBuffer;
        buff_t      _headerBuffer;
};

struct AHttpWrapper {
    AHttpWrapper( bool isCgi );
    AHttpWrapper( const buff_t* headerBuffer, const buff_t* bodyBuffer, bool isCgiInstance, int index );
    ~AHttpWrapper( void );
    int     sendDataToSocket( int socketfd, int flags );
    bool    empty( void ) const;
    int         indexNbr;
    int         socketfd;
    const bool  isCgi;
    const char* headerPtr;
    int         headerSize;
    const char* bodyPtr;
    int         bodySize;
};

struct HttpWrapper : public AHttpWrapper {
    HttpWrapper( ConfigHandler& configHandler, WsIpPort ipPortData );
    ~HttpWrapper( void );
    ws_http::statuscodes_t  setContent( ws_http::statuscodes_t status, std::istream& httpBodyStream, std::string const & mimeType );
    ws_http::statuscodes_t  setDefaultError( ws_http::statuscodes_t status );
    ws_http::statuscodes_t  setRedirection( ws_http::statuscodes_t status, std::string const & location );
    const WsIpPort    ipPort;
    HttpRequest request;
    buff_t      _bodyBuffer;
    buff_t      _headerBuffer;
};

struct HttpCgi : public AHttpWrapper {
    HttpRequest                 _requestHelper;
    HttpHandler &               _httpHandler;
    buff_t &                    _resultHeaderBuffer;
    buff_t &                    _resultBodyBuffer;
    std::vector<std::string>    _cgiArgs;
    std::vector<std::string>    _cgiEnv;
};

class AHttpHandler {
    public:
        AHttpHandler( bool isCgi );
        AHttpHandler( HttpContent const & data, bool isCgi );
        bool    isCgi( void ) const;
        virtual ~AHttpHandler( void );
        HttpContent dataContent;
        virtual ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end ) = 0;
    private:
        bool _isCgi;
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