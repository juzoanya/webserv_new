/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfos.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:57:08 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 13:58:09 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

FileInfos::FileInfos( std::string const & filePath, std::string const & requestPathDecoded )
 : _fileType(0), _fileError(0), _requestPathDecoded(requestPathDecoded)
//  : _fileType(0), _fileError(0), _filePath(filePath), _requestPathDecoded(requestPathDecoded)
{
    std::cout << "---- FileInfo -- constructor" << std::endl;
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0 && access(filePath.c_str(), R_OK) == 0) {
        this->_fileType = fileStat.st_mode;
    } else {
        this->_fileError = errno;
        return ;
    }
    DIR* currDir = opendir(filePath.c_str());
    if (currDir != NULL) {
        struct dirent *dirElem = NULL;
        while ((dirElem = readdir(currDir))) {
            struct stat fileStat;
            std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
            if (filename == "." || filename == "..")
                continue;
            std::string newFilePath = filePath + "/" + filename;
            if (stat(newFilePath.c_str(), &fileStat) == 0 && access(newFilePath.c_str(), R_OK) == 0
                && (S_ISDIR(fileStat.st_mode) || S_ISREG(fileStat.st_mode))) {
                this->_dirListingInfos.push_back(DirListingInfos(filename, fileStat));
            }
        }
        closedir(currDir);
    }
}

FileInfos::~FileInfos( void )
{ }

mode_t FileInfos::getMode( void ) const { return (this->_fileType); }

int FileInfos::getError( void ) const { return (this->_fileError); }

std::string const & FileInfos::checkContainedFile( std::vector<std::string> const & files )
{
    for (std::size_t i = 0; i != this->_dirListingInfos.size(); ++i) {
        std::vector<std::string>::const_iterator it = std::find(files.begin(), files.end(), this->_dirListingInfos[i].fileName);
        if (it != files.end())
            return (*it);
    }
    return (dummyStr);
}

int     FileInfos::setDirListing( std::stringstream & ss )
{
    if (this->_dirListingInfos.empty())
        return (-1);
    if (this->_requestPathDecoded[this->_requestPathDecoded.size() - 1] != '/')
        this->_requestPathDecoded.push_back('/');
    ss << "<!DOCTYPE html>";
    ss << "<html lang=\"en\">";
    ss << "<head>";
    ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    ss << "<title>Directory Listing</title>";
    ss << "<style>"
            "* { font-family: sans-serif; }\
            table { border-collapse: collapse; margin: 1em 0; font-size: 1em; min-width: 400px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.15); }\
            tr td:first-child { width: 20px; white-space: nowrap;overflow: hidden; text-overflow: ellipsis; }\
            thead tr { background-color: #009879; color: #ffffff; text-align: left; }\
            th, td { padding: 1em 2em; }\
            tbody tr:hover { background-color: #d9d7d7; }\
            tbody tr { border-bottom: 1px solid #dddddd; }\
            tbody tr:nth-of-type(even) { background-color: #f3f3f3; }\
            tr:nth-of-type(even):hover { background-color: #d9d7d7; }"
        "</style>";
    ss << "</head>";
    ss << "<body>";
    ss << "<h1>";
    std::string dirstrs, rootPart;
    std::stringstream dirstream(this->_requestPathDecoded);
    while (std::getline(dirstream, dirstrs, '/')) {
        rootPart += dirstrs + "/";
        if (dirstrs.empty()) dirstrs = "~";
        std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
        ss << a;
    }
    ss << "</h1>";
    ss << "<table>";
    ss << "<thead><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr></thead>";
    ss << "<tbody>";
    for (std::size_t i = 0; i != this->_dirListingInfos.size(); ++i) {
        ss << "<tr>";
        ss << "<td><a href=\"" << this->_requestPathDecoded << this->_dirListingInfos[i].fileName << "\">" << this->_dirListingInfos[i].fileName << "</a></td>";
        ss << "<td>" << this->_dirListingInfos[i].fileSize << "</td>";
        ss << "<td>" << this->_dirListingInfos[i].fileModDate << "</td>";
        ss << "</tr>";
    }
    ss << "</tbody>";
    ss << "</table>" "</body>" "</html>";
    return (0);
}



FileInfos::DirListingInfos::DirListingInfos( std::string const & name, struct stat & fileStat )
 : fileName(name), statFileType(fileStat.st_mode)
{
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    double dSize = static_cast<double>(fileStat.st_size);
    std::size_t k = 0;
    for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << dSize << " " << suffixes[k];
    this->fileSize = ss.str();
    this->fileModDate = getDateString(fileStat.st_mtime, "%d.%m.%Y %H:%M:%S");
}

FileInfos::DirListingInfos::~DirListingInfos( void )
{ }

