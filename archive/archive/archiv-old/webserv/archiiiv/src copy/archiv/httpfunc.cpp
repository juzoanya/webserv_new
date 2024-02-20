/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpfunc.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 08:12:45 by mberline          #+#    #+#             */
/*   Updated: 2023/12/22 11:43:49 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "httpfunc.hpp"
#include "FileInfo.hpp"

ClientData::ClientData( ConfigParser::ServerContext& serverConf ) : serverConfig(serverConf)
{ }

ConfigHandler::ConfigHandler( ConfigParser::ServerContext& serverConfig, HttpRequest& request )
 : _serverConfig(&serverConfig.serverConfig), _locationConfig(NULL)
{
    std::vector<std::pair<std::string, ws_config_t> >&   locConfigs = serverConfig.locationConfig;
    int index = -1;
    for (std::size_t i = 0; i != locConfigs.size(); ++i) {
        std::string const & path = request.getRequestLine().getPathDecoded();
        if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == path) {
            this->_locationConfig = &serverConfig.locationConfig[i].second;
            break ;
        } else if (locConfigs[i].first.find(path)) {
            if (index == -1 || locConfigs[i].first.size() > serverConfig.locationConfig[index].first.size()) {
                index = i;
            }
        }
    }
    if (index != -1) {
        this->_locationConfig = &serverConfig.locationConfig[index].second;
    }
}

ConfigHandler::~ConfigHandler( void )
{
    
}

std::vector<std::string> const & ConfigHandler::getVal( std::string const & key ) const
{
    ws_config_t::iterator locit = this->_locationConfig->find(key);
    if (locit != this->_locationConfig->end())
        return (locit->second);
    ws_config_t::iterator servit = this->_serverConfig->find(key);
    if (servit != this->_serverConfig->end())
        return (locit->second);
    return (this->_dummyVec);
}

std::string const & ConfigHandler::getDocRoot( void ) const
{
    std::vector<std::string> const & value = this->getVal("root");
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);
}

// std::string const & ConfigHandler::getServerName( void ) const
// {
    
// }

long ConfigHandler::getMaxBodySize( void ) const
{
    std::vector<std::string> const & value = this->getVal("client_max_body_size");
    if (value.size() == 1) {
        char* rest;
        long size = std::strtol(value[0].c_str(), &rest, 10);
        std::string unit(rest);
        return (unit == "M" ? size * 1000000 : unit == "K" ? size * 1000 : size);
    }
    return (4096);
}

bool    ConfigHandler::hasDirectoryListing( void ) const
{
    std::vector<std::string> const & value = this->getVal("directory_listing");
    if (value.size() == 1 && value[0] == "on")
        return (true);
    return (false);
}

bool    ConfigHandler::checkAllowedMethod( std::string const & method ) const
{
    std::vector<std::string> const & value = this->getVal("accepted_methods");
    for (std::size_t i = 0; i != value.size(); ++i) {
        if (value[i] == method)
            return (true);
    }
    return (false);
}

std::string const & ConfigHandler::getRedirection( void ) const
{
    std::vector<std::string> const & value = this->getVal("return");
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);   
}

std::string const & ConfigHandler::getErrorPage( std::string const & statusCode ) const
{
    std::vector<std::string> const & value = this->getVal("return" + statusCode);
    if (value.size() == 1)
        return (value[0]);
    return (this->_dummyValue);   
}

std::vector<std::string> const & ConfigHandler::getIndexFile( void ) const
{
    return (this->getVal("index"));
}
































// std::string getDateString( void )
// {
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now);
//     return (std::string(buff, size));
// }

// int writeDir(std::string const & currDir, FileInfo& dir, ClientData *clientData)
// {
//     std::string dirhtml =
//     "<!DOCTYPE html>""<html lang=\"en\">""<head>"
//     "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
//     "<title>404 Error - Page Not Found</title>""<style>"
//     "* {font-family: sans-serif;}\
//     table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
//     tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
//     thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
//     th, td {padding: 1em 2em;}\
//     tbody tr:hover {background-color: #d9d7d7;}\
//     tbody tr {border-bottom: 1px solid #dddddd;}\
//     tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
//     tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
//     "</style>""</head>""<body>""<h1>";

//     std::string dirstrs, rootPart;
//     std::stringstream ss(currDir);
//     while (std::getline(ss, dirstrs, '/')) {
//         rootPart += dirstrs + "/";
//         if (dirstrs.empty()) dirstrs = "~";
//         std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
//         dirhtml += a;
//     }
//     dirhtml += "</h1><table><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
//     for (std::size_t i = 0; i != dir.getDirectoryElemCount(); ++i) {
//         if (dir[i].getName() == "." || dir[i].getName() == "..")
//             continue;
//         std::string tablerow = "<tr><td><a href=\"" + currDir + dir[i].getName() + "\">"
//             + dir[i].getName() + "</a></td><td>" + dir[i].getSizeStr() + "</td><td>" + dir[i].getModDate() + "</td></tr>";
//         dirhtml += tablerow;
//     }

//     dirhtml += "</table></body></html>";
//     std::copy(dirhtml.begin(), dirhtml.end(), std::back_inserter(clientData->responseHeader));
//     return (1);
// }

// int fillBuffer(std::string const & documentRoot)
// {
    
// }

// int getLocationConfig(ClientData* clientData)
// {
//     std::vector<std::pair<std::string, ws_config_t>>&   locConfigs = clientData->serverConfig.locationConfig;
//     int index = -1;
//     for (std::size_t i = 0; i != locConfigs.size(); ++i) {
//         std::string const & path = clientData->request.getRequestLine().getPathDecoded();
//         if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == path) {
//             return (i);
//         } else if (locConfigs[i].first.find(path)) {
//             if (index == -1 || locConfigs[i].first.size() > clientData->serverConfig.locationConfig[index].first.size()) {
//                 index = i;
//             }
//         }
//     }
//     return (index);
// }


// int writeFile(ws_http::statuscodes_t status, FileInfo& currFile, std::string const & filePath, ClientData *clientData)
// {
//     std::stringstream   header;
//     header << "HTTP/1.1 " << ws_http::statuscodes.at(status) << CRLF;
//     header << "date" << getDateString() << CRLF;
//     if (currFile.isRegular() && currFile.checkRealPermissions(R_OK)) {
//         std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//         if (!ifs.is_open())
//             return (errno);
//         std::size_t filesize = ifs.tellg();
//         std::map<const std::string, const std::string>::const_iterator  mimeTypeIt = ws_http::mimetypes.find(ws_http::getFileExtension(filePath));
//         header << "content-type: " << (mimeTypeIt == ws_http::mimetypes.end() ? "application/ octet-stream" : mimeTypeIt->second) << CRLF;
//         header << "content-length: " << filesize << CRLF << CRLF;
//         std::string headerstr = header.str();
//         std::copy(headerstr.begin(), headerstr.end(), std::back_inserter(clientData->responseHeader));
//         std::size_t currSize = clientData->responseHeader.size();
//         ifs.seekg(0, std::ios::beg);
//         clientData->responseHeader.resize(currSize + filesize);
//         ifs.read(clientData->responseHeader.data() + currSize, filesize);
//         ifs.close();
//     } else {
//         std::string errorhtml = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
//         + ws_http::statuscodes.at(status) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
//         + ws_http::statuscodes.at(status) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
//         header << "content-type: text/html" << CRLF;
//         header << "content-length: " << errorhtml.size() << CRLF << CRLF;
//         std::string headerstr = header.str();
//         std::copy(headerstr.begin(), headerstr.end(), std::back_inserter(clientData->responseHeader));
//         std::copy(errorhtml.begin(), errorhtml.end(), std::back_inserter(clientData->responseHeader));
//     }
//     return (-1);
// }


// int fillBuffer(ClientData *clientData, std::map<std::string, std::vector<std::string> >* locationConfig, std::string const & documentRoot)
// {
//     ws_http::statuscodes_t status = ws_http::STATUS_200_OK;
//     std::map<std::string, std::string>  headers;
//     std::string filePath = documentRoot + clientData->request.getRequestLine().getPathDecoded();
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     // headers["date"] = std::string(buff, strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now));
//     // headers["content-length"] = "0";
//     FileInfo currFile(filePath);
//     if (!currFile.readStat())
        
//     if (currFile.isDir()) {
//         currFile.readDirectory();
//         std::string dirhtml = writeDir(clientData->request.getRequestLine().getPathDecoded(), currFile);
//     }
//     else if (!currFile.isRegular() || !currFile.checkRealPermissions(R_OK))
//         return (EACCES);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open())
//         return (errno);
//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);

//     ws_http::writeStatusLine("HTTP/1.1", ws_http::statuscodes.at(ws_http::STATUS_200_OK), clientData->responseHeader);
//     ws_http::writeHeader("Date", std::string(buff, size), clientData->responseHeader);
//     ws_http::writeHeader("connection", "close", clientData->responseHeader);
//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-length", "", clientData->responseHeader);
//     ws_http::writeHeader("location", "", clientData->responseHeader);


//     this->_resBody.clear();
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();
//     this->findSetContentTypeLength(ws_http::getFileExtension(filePath), filesize);
// }


// int fillBuffer(ClientData *clientData, std::map<std::string, std::vector<std::string> >* locationConfig, std::string const & documentRoot)
// {
//     ws_http::statuscodes_t status = ws_http::STATUS_200_OK;
//     std::map<std::string, std::string>  headers;
//     std::string filePath = documentRoot + clientData->request.getRequestLine().getPathDecoded();
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     // headers["date"] = std::string(buff, strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now));
//     // headers["content-length"] = "0";
//     FileInfo currFile(filePath);
//     if (!currFile.readStat())
//         return (currFile.getError());
//     if (currFile.isDir()) {
//         currFile.readDirectory();
//         std::string dirhtml = writeDir(clientData->request.getRequestLine().getPathDecoded(), currFile);
//     }
//     else if (!currFile.isRegular() || !currFile.checkRealPermissions(R_OK))
//         return (EACCES);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open())
//         return (errno);
//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);

//     ws_http::writeStatusLine("HTTP/1.1", ws_http::statuscodes.at(ws_http::STATUS_200_OK), clientData->responseHeader);
//     ws_http::writeHeader("Date", std::string(buff, size), clientData->responseHeader);
//     ws_http::writeHeader("connection", "close", clientData->responseHeader);
//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-length", "", clientData->responseHeader);
//     ws_http::writeHeader("location", "", clientData->responseHeader);


//     this->_resBody.clear();
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();
//     this->findSetContentTypeLength(ws_http::getFileExtension(filePath), filesize);
// }


// int getLocationConfig(ClientData* clientData)
// {
//     // std::string route;
//     // std::map<std::string, std::vector<std::string> >*    locationConfig;
//     // for (std::size_t i = 0; i != clientData->serverConfig.locationConfig.size(); ++i) {
//     //     std::string const & currLocation = clientData->serverConfig.locationConfig[i].first;
//     //     if (currLocation.find("=" == 0) && currLocation.substr(1, std::string::npos) == clientData->request.getRequestLine().getPathDecoded()) {
//     //         route = currLocation.substr(1, std::string::npos);
//     //         locationConfig = &clientData->serverConfig.locationConfig[i].second;
//     //         break ;
//     //     } else if (currLocation.find(clientData->request.getRequestLine().getPathDecoded()) && currLocation.size() > route.size()) {
//     //         route = currLocation;
//     //         locationConfig = &clientData->serverConfig.locationConfig[i].second;
//     //     }
//     // }

    
//     // int index = -1;
//     // for (std::size_t i = 0; i != clientData->serverConfig.locationConfig.size(); ++i) {
//     //     std::string const & cLoc = clientData->serverConfig.locationConfig[i].first;
//     //     std::string const & path = clientData->request.getRequestLine().getPathDecoded();
//     //     if (cLoc.find("=" == 0) && cLoc.substr(1, std::string::npos) == path) {
//     //         return (i);
//     //     } else if (cLoc.find(path)) {
//     //         if (index == -1 || cLoc.size() > clientData->serverConfig.locationConfig[index].first.size()) {
//     //             index = i;
//     //         }
//     //     }
//     // }
//     // return (index);


//     std::vector<std::pair<std::string, ws_config_t>>&   locConfigs = clientData->serverConfig.locationConfig;
//     int index = -1;
//     for (std::size_t i = 0; i != locConfigs.size(); ++i) {
//         std::string const & path = clientData->request.getRequestLine().getPathDecoded();
//         if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == path) {
//             return (i);
//         } else if (locConfigs[i].first.find(path)) {
//             if (index == -1 || locConfigs[i].first.size() > clientData->serverConfig.locationConfig[index].first.size()) {
//                 index = i;
//             }
//         }
//     }
//     return (index);
// }



// int handleClientData(void* arg)
// {
//     ClientData* clientData = static_cast<ClientData*>(arg);

//     std::string route;
//     route.resize(4096);
//     std::map<std::string, std::vector<std::string> >*    locationConfig;
//     for (std::size_t i = 0; i != clientData->serverConfig.locationConfig.size(); ++i) {
//         std::string const & currLocation = clientData->serverConfig.locationConfig[i].first;
//         if (currLocation.find("=" == 0) && currLocation.substr(1, std::string::npos) == clientData->request.getRequestLine().getPathDecoded()) {
//             route = currLocation.substr(1, std::string::npos);
//             locationConfig = &clientData->serverConfig.locationConfig[i].second;
//             break ;
//         } else if (currLocation.find(clientData->request.getRequestLine().getPathDecoded()) && currLocation.size() < route.size()) {
//             route = currLocation;
//             locationConfig = &clientData->serverConfig.locationConfig[i].second;
//         }
//     }

//     std::string documentRoot;
//     std::map<std::string, std::vector<std::string> >::iterator it = locationConfig->find("root");
//     if (it != locationConfig->end() && it->second.size() == 1)
//         documentRoot = it->second[0];
    
//     ws_http::writeStatusLine("HTTP/1.1", ws_http::statuscodes.at(ws_http::STATUS_200_OK), clientData->responseHeader);
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%a, %d %b %Y %H:%M:%S GMT", now);
//     ws_http::writeHeader("Date", std::string(buff, size), clientData->responseHeader);
//     ws_http::writeHeader("connection", "close", clientData->responseHeader);

//     std::string filePath = documentRoot + clientData->request.getRequestLine().getPathDecoded();
//     FileInfo currFile(filePath);
//     if (!currFile.readStat())
//         return (currFile.getError());
//     if (currFile.isDir())
//         return (EISDIR);
//     else if (!currFile.isRegular() || !currFile.checkRealPermissions(R_OK))
//         return (EACCES);
//     std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
//     if (!ifs.is_open())
//         return (errno);
//     std::size_t filesize = ifs.tellg();
//     ifs.seekg(0, std::ios::beg);
//     this->_resBody.clear();
//     this->_resBody.resize(filesize);
//     ifs.read(this->_resBody.data(), filesize);
//     ifs.close();
//     this->findSetContentTypeLength(ws_http::getFileExtension(filePath), filesize);

//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-type", "", clientData->responseHeader);
//     ws_http::writeHeader("content-length", "", clientData->responseHeader);
//     ws_http::writeHeader("location", "", clientData->responseHeader);
    
//     return (0);


//     for (int i = 0; i != this->_ndfs; ++i) {
//         if (this->fd[i].fd == -1) {
//             this->fd[i].fd = clientsocket;
//         }
//     }
// }
