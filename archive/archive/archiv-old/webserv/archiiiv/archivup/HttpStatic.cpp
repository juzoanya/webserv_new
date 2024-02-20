/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 12:12:22 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 16:47:00 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpStatic.hpp"

HttpStatic::HttpStatic( void )
{ }

HttpStatic::~HttpStatic( void )
{ }

void    HttpStatic::setContentByPath( std::string const & currPath, ConfigParser::ServerContext& serverConfig )
{
    std::string rootPath;
    std::string filePath = rootPath + currPath;
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0 && access(filePath.c_str(), R_OK) == 0) {
        if (S_ISREG(fileStat.st_mode))
            this->setFile(filePath);
        else if (S_ISDIR(fileStat.st_mode))
            this->setDirectory();
        else
            this->setErrorPage(ws_http::STATUS_404_NOT_FOUND);
    } else if (errno == EACCES) {
        this->setErrorPage(ws_http::STATUS_403_FORBIDDEN);
    } else {
        this->setErrorPage(ws_http::STATUS_404_NOT_FOUND);
    }
}

void    HttpStatic::setFile( std::string const & filePath )
{
    std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        this->setErrorPage(ws_http::STATUS_404_NOT_FOUND);
    } else {
        std::size_t filesize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        this->_fileData.clear();
        this->_fileData.resize(filesize);
        ifs.read(this->_fileData.data(), filesize);
        ifs.close();
    }
}

void    HttpStatic::setErrorPage( ws_http::statuscodes_t statusCode )
{
    std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
    + ws_http::statuscodes.at(statusCode) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
    + ws_http::statuscodes.at(statusCode) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
    this->_fileData.clear();
    std::copy(errPage.begin(), errPage.end(), std::back_inserter(this->_fileData));
}


void    setDirListingHeader(std::stringstream& ss)
{
    ss << "<!DOCTYPE html>";
    ss << "<html lang=\"en\">";
    ss << "<head>";
    ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    ss << "<title>Directory Listing</title>";
    ss << "<style>"
            "* {font-family: sans-serif;}\
            table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
            tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
            thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
            th, td {padding: 1em 2em;}\
            tbody tr:hover {background-color: #d9d7d7;}\
            tbody tr {border-bottom: 1px solid #dddddd;}\
            tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
            tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
        "</style>";
    ss << "</head>";
    ss << "<body>";
}

void    setDirListingNavigation(std::stringstream& ss, std::string const & currPath)
{
    ss << "<h1>";
    std::string dirstrs, rootPart;
    std::stringstream dirstream(currPath);
    while (std::getline(dirstream, dirstrs, '/')) {
        rootPart += dirstrs + "/";
        std::cout << "rootpart: " << rootPart << std::endl;
        if (dirstrs.empty()) dirstrs = "~";
        std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
        ss << a;
    }
    ss << "</h1>";
}

// int    setCheckDirElementStats(std::string const & filePath)
// {
//     DIR* currDir = opendir(filePath.c_str());
//     if (currDir == NULL)
//         return (-1);
//     struct dirent *dirElem = NULL;
//     while ((dirElem = readdir(currDir))) {
//         struct stat fileStat;
//         std::string filename = std::string(dirElem->d_name, dirElem->d_namlen);
//         ws_file_type_t currType = fileInfo.readStat(this->_filePath + "/", filename);
//         if (filename == "." || filename == ".." || currType == WS_FILE_PERMISSION_DENIED || currType == WS_FILE_NOT_FOUND)
//             continue;
//         std::vector<std::string>::const_iterator it = std::find(indexFiles.begin(), indexFiles.end(), filename);
//         if (it != indexFiles.end()) {
//             this->readStat(this->_rootPath, this->_currPath + "/" + filename);
//             return (this->getFile());
//         }
//         ss << "<tr><td><a href=\"" << this->_currPath << filename << "\">" << filename << "</a></td><td>";
//         setSizeFormatted(fileInfo._fileStat.st_size, ss);
//         ss <<  "</td><td>";
//         setDateFormatted(fileInfo._fileStat.st_mtimespec.tv_sec, ss);
//         ss <<  "</td></tr>";
//     }
//     closedir(currDir);   
// }

void    setDirListingEntry()
{
    
}

std::vector<std::string>*   getConfigEntry(ConfigParser::ServerContext& serverConfig, std::string const & key)
{
    ws_config_t::iterator locit = serverConfig.locationConfig.;
    if (locit != this->_locationConfig->end())
        return (locit->second);
    ws_config_t::iterator servit = this->_serverConfig->find(key);
    if (servit != this->_serverConfig->end())
        return (locit->second);
    return (this->_dummyVec);
}

void    HttpStatic::setDirectory( std::string const & rootPath, std::string const & currPath )
{
    std::string filePath = rootPath + currPath;
    DIR* currDir = opendir(filePath.c_str());
    if (currDir == NULL)
        return ;
    struct dirent *dirElem = NULL;
    while ((dirElem = readdir(currDir))) {
        struct stat fileStat;
        std::string filename = std::string(dirElem->d_name, dirElem->d_namlen);
        ws_file_type_t currType = fileInfo.readStat(this->_filePath + "/", filename);
        if (filename == "." || filename == ".." || currType == WS_FILE_PERMISSION_DENIED || currType == WS_FILE_NOT_FOUND)
            continue;
        std::vector<std::string>::const_iterator it = std::find(indexFiles.begin(), indexFiles.end(), filename);
        if (it != indexFiles.end()) {
            this->readStat(this->_rootPath, this->_currPath + "/" + filename);
            return (this->getFile());
        }
        ss << "<tr><td><a href=\"" << this->_currPath << filename << "\">" << filename << "</a></td><td>";
        setSizeFormatted(fileInfo._fileStat.st_size, ss);
        ss <<  "</td><td>";
        setDateFormatted(fileInfo._fileStat.st_mtimespec.tv_sec, ss);
        ss <<  "</td></tr>";
    }
    closedir(currDir);


    

    
    std::stringstream ss;
    setDirListingHeader(ss);
    setDirListingNavigation(ss, currPath);
    ss << "<table>";
    ss << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
    DIR* currDir = opendir(filePath.c_str());
    if (currDir == NULL)
        return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
    struct dirent *dirElem = NULL;
    while ((dirElem = readdir(currDir))) {
        struct stat fileStat;
        std::string filename = std::string(dirElem->d_name, dirElem->d_namlen);
        ws_file_type_t currType = fileInfo.readStat(this->_filePath + "/", filename);
        if (filename == "." || filename == ".." || currType == WS_FILE_PERMISSION_DENIED || currType == WS_FILE_NOT_FOUND)
            continue;
        std::vector<std::string>::const_iterator it = std::find(indexFiles.begin(), indexFiles.end(), filename);
        if (it != indexFiles.end()) {
            this->readStat(this->_rootPath, this->_currPath + "/" + filename);
            return (this->getFile());
        }
        ss << "<tr><td><a href=\"" << this->_currPath << filename << "\">" << filename << "</a></td><td>";
        setSizeFormatted(fileInfo._fileStat.st_size, ss);
        ss <<  "</td><td>";
        setDateFormatted(fileInfo._fileStat.st_mtimespec.tv_sec, ss);
        ss <<  "</td></tr>";
    }
    closedir(currDir);
    ss << "</table></body></html>";
    this->_fileStrData = ss.str();
    // return (Content(this->_currType, this->_fileStrData.data(), this->_fileStrData.size()));
    return (Content(this->_currType, this->_fileStrData.begin(), this->_fileStrData.end()));
}

HttpClient::HttpClient( ConfigParser::ServerContext& serverConfig ) : _serverConfig(&serverConfig.serverConfig), _locationConfig(NULL)
{
    // std::vector<std::pair<std::string, ws_config_t> >&   locConfigs = serverConfig.locationConfig;
    // int index = -1;
    // for (std::size_t i = 0; i != locConfigs.size(); ++i) {
    //     std::string const & path = request.getRequestLine().getPathDecoded();
    //     if (locConfigs[i].first.find("=" == 0) && locConfigs[i].first.substr(1, std::string::npos) == path) {
    //         this->_locationConfig = &serverConfig.locationConfig[i].second;
    //         break ;
    //     } else if (locConfigs[i].first.find(path)) {
    //         if (index == -1 || locConfigs[i].first.size() > serverConfig.locationConfig[index].first.size()) {
    //             index = i;
    //         }
    //     }
    // }
    // if (index != -1) {
    //     this->_locationConfig = &serverConfig.locationConfig[index].second;
    // }
}

std::vector<std::string>*  HttpClient::findDirective( std::string const & key )
{
    if (this->_locationConfig) {
        ws_config_t::iterator locit = this->_locationConfig->find(key);
        if (locit != this->_locationConfig->end())
            return (&locit->second);
    }
    ws_config_t::iterator servit = this->_serverConfig->find(key);
    if (servit != this->_serverConfig->end())
        return (&servit->second);
    return (NULL);
}

void    HttpClient::handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end)
{
    
}

// void    HttpStatic::setDirectory( std::string const & rootPath, std::string const & currPath )
// {
//     std::stringstream ss;
//     ss << "<!DOCTYPE html>""<html lang=\"en\">""<head>"
//         "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
//         "<title>Directory Listing</title>""<style>"
//         "* {font-family: sans-serif;}\
//         table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
//         tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
//         thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
//         th, td {padding: 1em 2em;}\
//         tbody tr:hover {background-color: #d9d7d7;}\
//         tbody tr {border-bottom: 1px solid #dddddd;}\
//         tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
//         tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
//         "</style>""</head>""<body>""<h1>";

//     std::string dirstrs, rootPart;
//     std::stringstream dirstream(currPath);
//     while (std::getline(dirstream, dirstrs, '/')) {
//         rootPart += dirstrs + "/";
//         std::cout << "rootpart: " << rootPart << std::endl;
//         if (dirstrs.empty()) dirstrs = "~";
//         std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
//         ss << a;
//     }
//     ss << "</h1><table><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
//     DIR* currDir = opendir(this->_filePath.c_str());
//     if (currDir == NULL)
//         return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
//     struct dirent *dirElem = NULL;
//     while ((dirElem = readdir(currDir))) {
//         struct stat fileStat;
//         std::string filename = std::string(dirElem->d_name, dirElem->d_namlen);
//         ws_file_type_t currType = fileInfo.readStat(this->_filePath + "/", filename);
//         if (filename == "." || filename == ".." || currType == WS_FILE_PERMISSION_DENIED || currType == WS_FILE_NOT_FOUND)
//             continue;
//         std::vector<std::string>::const_iterator it = std::find(indexFiles.begin(), indexFiles.end(), filename);
//         if (it != indexFiles.end()) {
//             this->readStat(this->_rootPath, this->_currPath + "/" + filename);
//             return (this->getFile());
//         }
//         ss << "<tr><td><a href=\"" << this->_currPath << filename << "\">" << filename << "</a></td><td>";
//         setSizeFormatted(fileInfo._fileStat.st_size, ss);
//         ss <<  "</td><td>";
//         setDateFormatted(fileInfo._fileStat.st_mtimespec.tv_sec, ss);
//         ss <<  "</td></tr>";
//     }
//     closedir(currDir);
//     ss << "</table></body></html>";
//     this->_fileStrData = ss.str();
//     // return (Content(this->_currType, this->_fileStrData.data(), this->_fileStrData.size()));
//     return (Content(this->_currType, this->_fileStrData.begin(), this->_fileStrData.end()));
// }



// void    HttpStatic::setContent( std::string const & rootPath, std::string const & currPath )
// {
//     std::string filePath = rootPath + currPath;
//     struct stat fileStat;
    

//     if (stat(filePath.c_str(), &fileStat) == 0 && access(filePath.c_str(), R_OK) == 0) {
        
//     }
//     // int retstat = stat(filePath.c_str(), &fileStat);
//     // int errstat = errno;
//     // int retaccess = access(filePath.c_str(), R_OK);
//     // int erraccess = errno;
//     // if (errstat == EACCES || erraccess == EACCES) {
//     //     this->_currType = WS_FILE_PERMISSION_DENIED;
//     // } else if (retstat == 0 && retaccess == 0 && S_ISDIR(this->_fileStat.st_mode)) {
//     //     this->_currType = WS_FILE_DIRECTORY;
//     // } else if (retstat == 0 && retaccess == 0 && S_ISREG(this->_fileStat.st_mode)) {
//     //     this->_currType = WS_FILE_REGULAR;
//     // } else {
//     //     this->_currType = WS_FILE_NOT_FOUND;
//     // }
//     // this->_rootPath = rootPath;
//     // this->_currPath = currPath;
//     // this->_filePath = rootPath + currPath;
    
//     // int retstat = stat(this->_filePath.c_str(), &this->_fileStat);
//     // int errstat = errno;
//     // int retaccess = access(this->_filePath.c_str(), R_OK);
//     // int erraccess = errno;
//     // if (errstat == EACCES || erraccess == EACCES) {
//     //     this->_currType = WS_FILE_PERMISSION_DENIED;
//     // } else if (retstat == 0 && retaccess == 0 && S_ISDIR(this->_fileStat.st_mode)) {
//     //     this->_currType = WS_FILE_DIRECTORY;
//     // } else if (retstat == 0 && retaccess == 0 && S_ISREG(this->_fileStat.st_mode)) {
//     //     this->_currType = WS_FILE_REGULAR;
//     // } else {
//     //     this->_currType = WS_FILE_NOT_FOUND;
//     // }
//     // return (this->_currType);
// }