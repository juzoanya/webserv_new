/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:24:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 20:03:10 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HANDLER_HPP
#define HTTP_HANDLER_HPP

#include "headers.hpp"

class HttpHandler {
    public:
        class CgiHandler;

        HttpHandler( ConfigHandler& configHandler, WsIpPort const & ipPort );
        ~HttpHandler( void );

        const WsIpPort      ipPortData;
        const char*         dataPtr;
        int                 dataSize;
        ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
        int                     createCgiProcess( void );
        ws_http::statuscodes_t  setError( ws_http::statuscodes_t errorStatusCode, bool internal );
        ws_http::statuscodes_t  setHeader( ws_http::statuscodes_t statusCode, std::istream* httpBodyStream, std::string const & mimeType, std::string const & redirectLocation="" );
        ws_http::statuscodes_t  processResponse( ws_http::statuscodes_t statusCode, std::string const & method, std::string const & requestPathDecoded, bool internal );
        HttpRequest         _request;
        buff_t              _fileData;
        HttpConfig          _httpConfig;
        ConfigHandler*      _configHandler;
        int                 _intRedirectNbr;

};

// class HttpHandler::CgiHandler : public AHttpHandler {
//     public:
//         CgiHandler( HttpHandler & httpHandler, HttpRequest & request, buff_t & resultBodyBuffer, buff_t & resultHeaderBuffer );
//         ~CgiHandler( void );
//         int createChildProcess( void );
//         ws_http::statuscodes_t  handleData( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
//     private:
//         static int parentsock;
//         static int childsock;

//         HttpRequest                 _requestHelper;
//         HttpHandler &               _httpHandler;
//         buff_t &                    _resultHeaderBuffer;
//         buff_t &                    _resultBodyBuffer;
//         std::vector<std::string>    _cgiArgs;
//         std::vector<std::string>    _cgiEnv;
//         std::vector<char*>          _arguments;
//         std::vector<char*>          _enviroment;
// };


#endif