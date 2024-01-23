/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 12:12:22 by mberline          #+#    #+#             */
/*   Updated: 2024/01/18 21:30:56 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpStatic::HttpStatic( void ) : status(ws_http::STATUS_404_NOT_FOUND), isDirectory(false)
{ }

HttpStatic::~HttpStatic( void )
{ }

void    HttpStatic::handleData( std::vector<char>::iterator it_start, std::vector<char>::iterator it_end )
{
    (void)it_start;
    (void)it_end;
}

void    HttpStatic::setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing )
{
    this->isDirectory = false;
    this->status = ws_http::STATUS_404_NOT_FOUND;
    struct stat fileStat;
    if ((stat(filePath.c_str(), &fileStat) == -1 || access(filePath.c_str(), R_OK) == -1) && errno == EACCES) {
        this->status = ws_http::STATUS_403_FORBIDDEN;
    } else if (S_ISREG(fileStat.st_mode)) {
        this->setFile(filePath);
    } else if (S_ISDIR(fileStat.st_mode)) {
        this->checkDirectory(filePath, requestUri, indexFiles, dirListing);
    }
}

bool    HttpStatic::setFile( std::string const & filePath )
{
    std::ifstream   ifs(filePath.c_str(), std::ios::binary | std::ios::ate);
    if (ifs.is_open()) {
        std::size_t filesize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        this->fileData.clear();
        this->fileData.resize(filesize);
        ifs.read(this->fileData.data(), filesize);
        ifs.close();
        this->status = ws_http::STATUS_200_OK;
        return (true);
    }
    return (false);
}

void    HttpStatic::checkDirectory( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing)
{
    DIR* currDir = opendir(filePath.c_str());
    if (currDir != NULL) {
        std::vector< std::pair<std::string, struct stat> > dirEntries;
        struct dirent *dirElem = NULL;
        while ((dirElem = readdir(currDir))) {
            struct stat fileStat;
            std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
            if (filename == "." || filename == "..")
                continue;
            std::string filePathFull = filePath + "/" + filename;
            if (stat(filePathFull.c_str(), &fileStat) == -1 || access(filePathFull.c_str(), R_OK) == -1)
                continue;
            if (S_ISREG(fileStat.st_mode)
            && std::find(indexFiles.begin(), indexFiles.end(), filename) != indexFiles.end()) {
                this->setFile(filePathFull);
                return ;
            }
            dirEntries.push_back(std::make_pair(filename, fileStat));
        }
        closedir(currDir);
        if (dirListing) {
            this->setDirListing(requestUri, dirEntries);
        } else {
            this->status = ws_http::STATUS_403_FORBIDDEN;
        }
    }
}

void    HttpStatic::setError(ws_http::statuscodes_t errorStatus, std::string const & filePath)
{
    struct stat fileStat;
    if (!filePath.empty() && stat(filePath.c_str(), &fileStat) == 0 && access(filePath.c_str(), R_OK) == 0
            && S_ISREG(fileStat.st_mode) && this->setFile(filePath)) {
            return ;
    }
    std::string errPage = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
    + ws_http::statuscodes.at(errorStatus) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
    + ws_http::statuscodes.at(errorStatus) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
    this->fileData = std::vector<char>(errPage.begin(), errPage.end());
}






void    HttpStatic::setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries )
{
    std::stringstream ss;
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

    ss << "<h1>";
    std::string dirstrs, rootPart;
    std::stringstream dirstream(requestUri);
    while (std::getline(dirstream, dirstrs, '/')) {
        rootPart += dirstrs + "/";
        std::cout << "rootpart: " << rootPart << std::endl;
        if (dirstrs.empty()) dirstrs = "~";
        std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
        ss << a;
    }
    ss << "</h1>";

    ss << "<table>";
    ss << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    for (std::size_t i = 0; i != dirEntries.size(); ++i) {
        ss << "<tr>";
        ss << "<td><a href=\"" << requestUri << dirEntries[i].first << "\">" << dirEntries[i].first << "</a></td>";
        double dSize = static_cast<double>(dirEntries[i].second.st_size);
        std::size_t k = 0;
        for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
        ss << "<td>" << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i] << "</td>";
        std::tm* now = std::localtime(&dirEntries[i].second.st_mtime);
        char dateBuff[100];
        std::size_t dateStrSize = strftime(dateBuff, 1000, "%d.%m.%Y %H:%M:%S", now);
        dateBuff[dateStrSize] = 0;
        ss << "<td>" << dateBuff << "</td>";
        ss << "</tr>";
    }
    ss << "</table>";

    ss << "</body>";
    ss << "</html>";
    this->isDirectory = true;
    this->status = ws_http::STATUS_200_OK;
    std::string dirstr = ss.str();
    this->fileData = std::vector<char>(dirstr.begin(), dirstr.end());
}
