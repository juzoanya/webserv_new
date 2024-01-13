/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 12:12:22 by mberline          #+#    #+#             */
/*   Updated: 2024/01/13 15:21:13 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


HttpStatic::HttpStatic( void ) : status(ws_http::STATUS_404_NOT_FOUND), isDirectory(false)
{ }

HttpStatic::~HttpStatic( void )
{ }

/**
 * @brief Set the HttpStatic object content based on the specified file path.
 *
 * This function determines the type of content to be set in the HttpStatic object
 * based on the provided file path. It checks whether the path corresponds to a regular
 * file, a directory, or if access is forbidden. The appropriate action is taken,
 * including setting the HTTP status and content accordingly.
 *
 * @param filePath The path to the file or directory.
 * @param requestUri The requested URI associated with the file or directory.
 * @param indexFiles A vector of index file names to check for in directories.
 * @param dirListing A boolean flag indicating whether directory listing is enabled.
 */
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

/**
 * @brief Set the contents of the HttpStatic object by reading a file.
 *
 * This function opens the specified file in binary mode, reads its contents,
 * and stores the data in the internal fileData member. It also sets the HTTP
 * status to "200 OK" if the file is successfully read.
 *
 * @param filePath The path to the file to be read.
 * @return true if the file is successfully read and data is set, false otherwise.
 */
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

/**
 * @brief Check the specified directory for files and handle accordingly.
 *
 * This function opens the specified directory, iterates through its entries,
 * and performs actions based on the contents. If index files are found, it sets
 * the HttpStatic object to the content of the first index file. If directory listing
 * is enabled, it displays the directory entries. If directory listing is disabled,
 * it sets the HTTP status to "403 Forbidden".
 *
 * @param filePath The path to the directory to be checked.
 * @param requestUri The requested URI associated with the directory.
 * @param indexFiles A vector of index file names to check for.
 * @param dirListing A boolean flag indicating whether directory listing is enabled.
 */
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

/**
 * @brief Set the HttpStatic object to represent an error response.
 *
 * This function populates the HttpStatic object with the content of an error page
 * based on the provided HTTP status code. If a custom error page file is specified
 * and is valid, the content of that file is used. Otherwise, a default error page
 * is generated and set in the HttpStatic object.
 *
 * @param errorStatus The HTTP status code representing the error.
 * @param filePath Optional path to a custom error page file. If empty or invalid,
 *                 a default error page is generated.
 */
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

/**
 * @brief Generate and set the HttpStatic object to represent a directory listing.
 *
 * This function generates an HTML representation of a directory listing, including
 * links to files and additional information such as file sizes and last modified
 * dates. The generated content is then set in the HttpStatic object.
 *
 * @param requestUri The requested URI associated with the directory.
 * @param dirEntries A vector of directory entries, each containing a filename and
 *                   file information (struct stat).
 */
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
        dateBuff[dateStrSize = 0];
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
