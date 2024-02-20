/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 16:50:38 by mberline          #+#    #+#             */
/*   Updated: 2024/01/22 22:17:26 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpClient.hpp"

HttpClient::HttpClient( ConfigHandler& httpConfig, std::string const & ipStr, std::string const & portStr)
 : _httpConfigHandler(httpConfig), _ipStr(ipStr), _portStr(portStr)
{ }

HttpClient::~HttpClient( void )
{ }

int HttpClient::setHeader( ws_http::statuscodes_t status, HttpConfig& config )
{
    std::stringstream headerstream;
    headerstream << "HTTP/1.1" << ws_http::statuscodes.at(status) << CRLF;
    headerstream << "connection: close" << CRLF;
    headerstream << "content-type: " << config.getMimeType() << CRLF;
    headerstream << "content-length: " << this->_staticContent.fileData.size() << CRLF;
    std::time_t t = std::time(0);
    std::tm* now = std::gmtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
    buff[size] = 0;
    headerstream << "date: " << buff << CRLF << CRLF;
    headerstream << CRLF;
    this->_httpHeader = headerstream.str();
    this->_headerContent = Content(this->_httpHeader.data(), this->_httpHeader.size());
    this->_bodyContent = Content(this->_staticContent.fileData.data(), this->_staticContent.fileData.size());
    return (0);
}

int    HttpClient::handleCGI( HttpConfig& config )
{
    
}

// int    HttpClient::handleError( ws_http::statuscodes_t status, HttpConfig& config )
// {
//     std::stringstream ss;
//     ss << status;
//     std::string const & errorPagePath = config.getErrorPage(ss.str());
//     if (!errorPagePath.empty()) {
//         HttpConfig errorConfig = this->_httpConfigHandler.getHttpConfig(errorPagePath, this->_request.headerMap.at("host"), "");
//         this->_staticContent.setError(status, errorConfig.getFilePath());
//         this->setHeader(status, config);
//     } else {
//         this->_staticContent.setError(status, "");
//         this->setHeader(status, config);
//     }
// }

int    HttpClient::handleError( ws_http::statuscodes_t status, HttpConfig& config )
{
    std::stringstream ss;
    ss << status;
    std::string const & errorPagePath = config.getErrorPage(ss.str());
    if (!errorPagePath.empty()) {
        HttpConfig errorConfig = this->_httpConfigHandler.getHttpConfig(errorPagePath, this->_request.headerMap.at("host"), "");
        this->_staticContent.setContentByPath(errorConfig.getFilePath(), errorPagePath, errorConfig.getIndexFile(), errorConfig.hasDirectoryListing());
        if (this->_staticContent.status == ws_http::STATUS_200_OK && errorConfig.getMimeType() == "text/html")
            this->setHeader(status, config);
    } else {
        this->_staticContent.setError(status, "");
        this->setHeader(status, config);
    }
}

int HttpClient::handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end)
{
    this->_request.parseRequest(it_start, it_end);
    if (this->_request.status == ws_http::STATUS_UNDEFINED)
        return (0);
    HttpConfig config = this->_httpConfigHandler.getHttpConfig(this->_request.pathDecoded, this->_request.headerMap.at("host"), "");
    if (this->_request.status != ws_http::STATUS_200_OK)
        return (this->handleError(this->_request.status, config));
    if (!config.checkAllowedMethod(this->_request.method))
        return (this->handleError(ws_http::STATUS_405_METHOD_NOT_ALLOWED, config));
    std::string const cgiExecutable = config.getCgiExecutable();
    if (!cgiExecutable.empty())
        return (this->handleCGI(config));
    this->_staticContent.setContentByPath(config.getFilePath(), this->_request.pathDecoded, config.getIndexFile(), config.hasDirectoryListing());
    this->_request = HttpRequest();
}


Content*    HttpClient::getHttpContent( void )
{
    
}




// // HttpConfiguration::HttpConfiguration( ConfigParser& config, std::string const & ip, std::string const & port )
// // {
// //     for (int i = 0; i != config.getServerCount(); ++i) {
// //         ws_config_t::iterator it = config.serverConfigs[i].serverConfig.find("listen");
// //         if (it->second.size() == 1 && it->second[0] == )
// //     }
// // }

// // HttpConfiguration::~HttpConfiguration( void )
// // { }

// // void    HttpConfiguration::changeServerLocation( HttpRequest& request )
// // { }





// HttpClient::HttpClient( HttpConfiguration& httpConfig, std::string const & ipStr, std::string const & portStr)
//  : _httpConfig(httpConfig), _ipStr(ipStr), _portStr(portStr)
// {
//     // std::vector<std::string>* maxSizeDirective = this->findDirective("client_max_body_size");
//     // if (maxSizeDirective && maxSizeDirective->size() == 1) {
//     //     char* rest;
//     //     long size = std::strtol(maxSizeDirective->at(0).c_str(), &rest, 10);
//     //     std::string unit(rest);
//     //     this->_maxBodySize = unit == "M" ? size * 1000000 : unit == "K" ? size * 1000 : size;
//     // }
// }

// HttpClient::~HttpClient( void )
// { }

// void    HttpClient::setLocationConfig( void )
// {
//     std::vector<std::pair<std::string, ws_config_t> >&   locConfigs = this->_serverConfig.locationConfig;
//     int index = -1;
//     for (std::size_t i = 0; i != locConfigs.size(); ++i) {
//         std::string const & path = this->_request.pathDecoded;
//         if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == this->_request.pathDecoded) {
//             this->_locationConfig = &this->_serverConfig.locationConfig[i];
//             break ;
//         }
//         if (locConfigs[i].first.find(this->_request.pathDecoded)
//         && (!this->_locationConfig || locConfigs[i].first.size() > this->_locationConfig->first.size())) {
//             this->_locationConfig = &locConfigs[i];
//         }
//     }
//      ws_config_t::iterator it;
//     if (this->_locationConfig) {
//         it = this->_locationConfig->second.find("root");
//         if (it != this->_locationConfig->second.end() && it->second.size() == 1) {
//             this->_filePath = it->second.at(0) + this->_request.pathDecoded.substr(it->first.size(), std::string::npos);
//             return ;
//         }
//     }
//     it = this->_serverConfig.serverConfig.find("root");
//     if (it != this->_locationConfig->second.end() && it->second.size() == 1)
//         this->_filePath = it->second.at(0) + this->_request.pathDecoded;
// }

// void    HttpClient::setFilePath( void )
// {
//     ws_config_t::iterator it;
//     ws_config_t* loc = this->_locationConfig ? &this->_locationConfig->second : NULL;
//     ws_config_t* serv = &this->_serverConfig.serverConfig;
//     if (loc && (it = loc->find("root")) != loc->end() && it->second.size() == 1) {
//         this->_filePath = it->second.at(0) + this->_request.pathDecoded.substr(it->first.size(), std::string::npos);
//     } else if ((it = serv->find("root")) != serv->end() && it->second.size() == 1) {
//         this->_filePath = it->second.at(0) + this->_request.pathDecoded;
//     }
//     // if (this->_locationConfig && (it = this->_locationConfig->second.find("root")) != this->_locationConfig->second.end() && it->second.size() == 1) {
//     //     this->_filePath = it->second.at(0) + this->_request.pathDecoded.substr(it->first.size(), std::string::npos);
//     // } else if ((it = this->_serverConfig.serverConfig.find("root")) != this->_serverConfig.serverConfig.end() && it->second.size() == 1) {
//     //     this->_filePath = it->second.at(0) + this->_request.pathDecoded;
//     // }
// }

// std::vector<std::string>*  HttpClient::findDirective( std::string const & key )
// {
//     ws_config_t::iterator it;
//     if (this->_locationConfig) {
//         it = this->_locationConfig->second.find(key);
//         if (it != this->_locationConfig->second.end())
//             return (&it->second);
//     }
//     it  = this->_serverConfig.serverConfig.find(key);
//     if (it != this->_serverConfig.serverConfig.end())
//         return (&it->second);
//     return (NULL);
// }

// void    HttpClient::handleGetRequest( void )
// {
//     // ws_http_parser_status_t parserStatus = clientData.request.parseRequest(it_start, it_end);
//     // if (parserStatus == WS_HTTP_PARSE_PARSING)
//     //     return (0);
//     // ConfigHandler handler(clientData.serverConfig, clientData.request);
//     // if (parserStatus >= WS_HTTP_PARSE_ERROR_DEFAULT) {
//     //     setHeadersAndError(clientData, handler, ws_http::STATUS_400_BAD_REQUEST, "text/html");
//     //     return (-1);
//     // }
//     // if (!handler.checkAllowedMethod(ws_http::methods_rev.at(clientData.request.getRequestLine().getMethod()))) {
//     //     setHeadersAndError(clientData, handler, ws_http::STATUS_405_METHOD_NOT_ALLOWED, "text/html");
//     //     return (-1);
//     // }
//     // ws_file_type_t type = clientData.fileInfo.readStat(handler.getDocRoot(), clientData.request.getRequestLine().getPathDecoded());
//     // std::map<const std::string, const std::string>::const_iterator  mimeTypeIt;
//     // mimeTypeIt = ws_http::mimetypes.find(ws_http::getFileExtension(clientData.request.getRequestLine().getPathDecoded()));
//     // if (type == WS_FILE_PERMISSION_DENIED) {
//     //     setHeadersAndError(clientData, handler, ws_http::STATUS_403_FORBIDDEN, "text/html");
//     // } else if (type == WS_FILE_NOT_FOUND) {
//     //     setHeadersAndError(clientData, handler, ws_http::STATUS_404_NOT_FOUND, "text/html");
//     // } else if (type == WS_FILE_REGULAR) {
//     //     clientData.bodyIterator = clientData.fileInfo.getContent(handler.getIndexFile());
//     // } else if (type == WS_FILE_DIRECTORY) {

//     // }
//     // return (0);
// }

// void    HttpClient::handleCGI( void )
// {
    
// }

// void    HttpClient::handleError( ws_http::statuscodes_t status )
// {
//     std::stringstream ss;
//     ss << status;
//     std::vector<std::string>* vecPtr = this->findDirective("error_page" + ss.str());
//     if (vecPtr && vecPtr->size() == 1) {
//         this->_staticContent.setContentByPath()
//     }
// // void    HttpStatic::setErrorPage( ws_http::statuscodes_t statusCode )
// // {
// //     std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
// //     + ws_http::statuscodes.at(statusCode) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
// //     + ws_http::statuscodes.at(statusCode) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
// //     this->_fileData.clear();
// //     std::copy(errPage.begin(), errPage.end(), std::back_inserter(this->_fileData));
// // }


// }

// void        HttpClient::handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end)
// {
//     this->_request.parseRequest(it_start, it_end);
//     if (this->_request.status == ws_http::STATUS_UNDEFINED)
//         return ;
//     this->setLocationConfig();
//     std::vector<std::string>* rootDirective = this->findDirective("root");
//     if (this->_request.status == ws_http::STATUS_200_OK) {
//         if (this->_request.method == "GET")
//             this->handleGetRequest();
//         else if (this->_request.method == "POST" || this->_request.method == "DELETE")
//             this->handleCGI();
//         else
//             this->handleError(ws_http::STATUS_405_METHOD_NOT_ALLOWED);
//     } else {
//         this->handleError(this->_request.status);
//     }
//     this->_request = HttpRequest();
// }


// Content*    HttpClient::getHttpContent( void )
// {
    
// }
