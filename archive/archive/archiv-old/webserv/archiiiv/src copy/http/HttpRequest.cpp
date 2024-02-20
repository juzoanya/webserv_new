/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:27:12 by mberline          #+#    #+#             */
/*   Updated: 2023/12/18 15:34:19 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include "HttpRequest.hpp"

// HttpRequest::HttpRequest( void )
// { }

HttpRequest::HttpRequest(std::size_t maxHeaderSize, std::size_t maxBodySize )
 :  _status(WS_HTTP_PARSE_PARSING), _debHeaderSize(0),
    _remainCapacity(maxHeaderSize), _maxBodySize(maxBodySize), _headerDone(false)
{
    this->_headerBuffer.reserve(maxHeaderSize);
    this->_prevHeaderPos = this->_headerBuffer.begin();
}

HttpRequest::~HttpRequest( void )
{ }

ws_http_parser_status_t HttpRequest::getStatus( void ) const
{
    return (this->_status);
}

void HttpRequest::printRequest( void ) const
{
    std::cout << " --- REQUEST HEADER Size: " << this->_debHeaderSize << " --- " << std::endl;
    std::cout << " --- Request-Line --- " << std::endl;
    std::cout << "Request Method: " << ws_http::methods_rev.at(this->_requestLine.getMethod()) << std::endl;
    std::cout << "Request Http Version: " << ws_http::versions_rev.at(this->_requestLine.getVersion()) << std::endl;
    std::cout << "Request Target PathEncoded: " << this->_requestLine.getPath() << std::endl;
    std::cout << "Request Target PathDecoded: " << this->_requestLine.getPathDecoded() << std::endl;
    std::cout << "Request Target Query: " << this->_requestLine.getQuery() << std::endl;
    std::cout << "Request Target FullUri: " << this->_requestLine.getFullUri() << std::endl;
    std::cout << " --- Header-Fields --- " << std::endl;
    this->_header.printHeaders();
    std::cout << " --- REQUEST BODY Size: " << this->getBody().size() << "--- " << std::endl;
    if (this->getBody().size() < 1000)
        std::cout << std::string(this->getBody().begin(), this->getBody().end()) << std::endl;
}

buff_t const &  HttpRequest::getBody( void ) const
{
    return (this->_body.getBody());
}

HttpHeaderParser const & HttpRequest::getHeaders( void ) const
{
    return (this->_header);
}

HttpRequestLineParser const & HttpRequest::getRequestLine( void ) const
{
    return (this->_requestLine);
}

bool HttpRequest::parseBuffer( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    buff_t::const_iterator  it_pos;

    it_pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    this->_status = this->_requestLine.parseRequestLine(it_start, it_pos);
    if (this->_status != WS_HTTP_PARSE_PARSING)
        return (false);
    this->_status = this->_header.parseHeadersInBuffer(it_pos + ws_http::crlf.size(), it_end);
    if (this->_status != WS_HTTP_PARSE_PARSING)
        return (false);
    this->_status = WS_HTTP_PARSE_HEADER_DONE;
    this->_headerDone = true;
    return (true);
}


ws_http_parser_status_t HttpRequest::parseRequest( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    if (this->_status != WS_HTTP_PARSE_PARSING)
        return (this->_status);
    if (this->_headerDone) {
        this->_status = this->_body.parseBody(it_start, it_end);
        if (this->_status == WS_HTTP_PARSE_BODY_DONE)
            this->_status = WS_HTTP_PARSING_DONE;
        return (this->_status);
    }
    buff_t::const_iterator it_pos = it_end;
    if (it_end - it_start > this->_remainCapacity)
        it_pos = it_start + this->_remainCapacity;
    this->_remainCapacity -= it_pos - it_start;
    std::copy(it_start, it_pos, std::back_inserter(this->_headerBuffer));
    buff_t::const_iterator itEndPos = std::search(this->_prevHeaderPos, this->_headerBuffer.end(),
                                    ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
    if (itEndPos == this->_headerBuffer.end() && this->_remainCapacity == 0) {
        this->_status = WS_HTTP_PARSE_ERROR_HEADER_OVERSIZE;
        buff_t().swap(this->_headerBuffer);
    } else if (itEndPos == this->_headerBuffer.end()) {
        this->_prevHeaderPos = this->_headerBuffer.end()
                        - std::min(ws_http::httpHeaderEnd.size(), this->_headerBuffer.size());
    } else {
        if (this->parseBuffer(this->_headerBuffer.begin(), itEndPos)) {
            std::string const & contLenStr = this->_header.getHeader(ws_http::HEADER_CONTENT_LENGTH);
            long contentLen = strtol(contLenStr.c_str(), NULL, 10);
            if (contentLen == 0 && this->_header.getHeader(ws_http::HEADER_TRANSFER_ENCODING).empty()) {
                this->_status = WS_HTTP_PARSING_DONE;
            } else {
                this->_body = HttpBody(contentLen, this->_maxBodySize);
                this->_status = this->_body.parseBody(itEndPos + ws_http::httpHeaderEnd.size(), this->_headerBuffer.end());
                if (this->_status == WS_HTTP_PARSE_BODY_DONE)
                    this->_status = WS_HTTP_PARSING_DONE;
            }
        }
        buff_t().swap(this->_headerBuffer);
    }
    return (this->_status);
}

// char* const*    HttpRequest::getCgiEnviroment( std::string const & docRoot, std::string const & serverName )
// {
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_QUERY_STRING, this->_requestLine.getQuery());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_REQUEST_METHOD, ws_http::methods_rev.at(this->_requestLine.getMethod()));
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_CONTENT_TYPE, this->_header.getHeader(ws_http::HEADER_CONTENT_TYPE));
    
//     std::stringstream ss;
//     ss << this->getBody().size();
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_CONTENT_LENGTH, ss.str());

//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SCRIPT_FILE_NAME, docRoot + this->_requestLine.getPath());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SCRIPT_NAME, this->_requestLine.getPath());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_PATH_INFO, this->_requestLine.getPath());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_PATH_TRANSLATED, docRoot + this->_requestLine.getPath());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_REQUEST_URI, this->_requestLine.getFullUri());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SERVER_PROTOCOL, ws_http::versions_rev.at(this->_requestLine.getVersion()));
    
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_GATEWAY_INTERFACE, "CGI/1.1");
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SERVER_SOFTWARE, WEBSERV_VERSION);

//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_REMOTE_ADDR, this->_client->getRemoteAdress());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_REMOTE_PORT, this->_client->getRemotePort());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SERVER_ADDR, this->_client->getServSock().getServerAdress());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SERVER_PORT, this->_client->getServSock().getServerPort());
//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_SERVER_NAME, serverName);

//     this->_cgi.addEnvVariable(ws_cgi::WS_CGI_REDIRECT_STATUS, "200");
    
//     std::map<ws_http::header_field_t, std::string> const & headers = this->_header.getHeaderMap();
//     std::map<ws_http::header_field_t, std::string>::const_iterator  ith;
//     for (ith = headers.begin(); ith != headers.end(); ++ith) {
//         this->_cgi.addEnvVariable(ws_cgi::headerToCgiEnv(ws_http::headers_rev.at(ith->first)), ith->second);
//     }
    
//     std::map<std::string, std::string, ws_http::CaInCmp> const & custHeaders = this->_header.getCustomHeaderMap();
//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator  itch;
//     for (itch = custHeaders.begin(); itch != custHeaders.end(); ++itch) {
//         this->_cgi.addEnvVariable(ws_cgi::headerToCgiEnv(itch->first), itch->second);
//     }
    
//     return (this->_cgi.getCgiEnviroment());
// }

// int HttpRequest::spawnCgi( std::string const & cgiBinPath, char** args, std::string const & rootPath, std::string const & serverName, HttpResponse& response )
// {
//     this->_cgiHandler = CgiHandler(response);
//     return (this->_client->getEventLoop().childProcessManager.spawn(cgiBinPath, args, this->getCgiEnviroment(rootPath, serverName),
//         this->_cgiHandler, this->_cgiHandler._cgiData, this->getBody().begin(), this->getBody().end()));
// }

// CgiHandler::CgiHandler( void ) : _response(NULL)
// { }

// CgiHandler::CgiHandler(HttpResponse& response) : _response(&response)
// { }

// void CgiHandler::operator()(ChildProcessStatus status)
// {
//     std::cout << "child exited - status: " << status.status << " | " << status.error << std::endl;
//     if (!this->_response)
//         return ;
//     if (status.status != WS_CHILDPROCESS_EXITED_OK || std::distance(this->_cgiData.begin(), this->_cgiData.end()) == 0) {
//         this->_response->sendError(ws_http::STATUS_500_INTERNAL_SERVER_ERROR);
//     } else {
//         buff_t::iterator it;
//         std::cout << std::string(this->_cgiData.begin(), this->_cgiData.end()) << std::endl;
//         std::cout << "cgi Buffer distance: " << std::distance(this->_cgiData.begin(), this->_cgiData.end()) << std::endl;
//         it = std::search(this->_cgiData.begin(), this->_cgiData.end(), ws_http::httpHeaderEnd.begin(),  ws_http::httpHeaderEnd.end());
//         this->_response->getHeaders().parseHeadersInBuffer(this->_cgiData.begin(), it);
//         std::string const & statusCode = this->_response->getHeaders().getHeader("Status");
//         std::map<const std::string, ws_http::statuscodes_t>::const_iterator statusEnum;
//         statusEnum = ws_http::statuscodes_rev.lower_bound(statusCode);
//         this->_response->statuscode = statusEnum->second;
//         this->_response->sendBufferRange(it + ws_http::httpHeaderEnd.size(), this->_cgiData.end());
//     }
// }
