/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 22:57:57 by mberline          #+#    #+#             */
/*   Updated: 2023/12/21 20:00:27 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileInfo.hpp"
#include <fstream>

void    setSizeFormatted(std::size_t size, std::stringstream& ss) {
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    std::size_t i = 0;
    double dSize = static_cast<double>(size);
    while (dSize >= 1000.0 && i < (sizeof(suffixes) / sizeof(suffixes[0]) - 1)) {
        dSize /= 1000.0;
        i++;
    }
    ss << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i];
}

void setDateFormatted(std::time_t t, std::stringstream& ss) {
    std::tm* now = std::localtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
    buff[size] = 0;
    ss << buff;
}

// Content::Content( ws_file_type_t contentType, void const* contentData, int contentSize )
//  : fileType(contentType), data(contentData), size(contentSize)
// { }

Content::Content( void )
{ }

Content::Content( ws_file_type_t contentType, buff_t::const_iterator itstart, buff_t::const_iterator itend )
 : fileType(contentType), it_start(itstart), it_end(itend)
{ }

Content::~Content( void )
{ }

FileInfo::FileInfo( void ) : _currType(WS_FILE_UNSET)
{ }

FileInfo::~FileInfo( void )
{ }

ws_file_type_t  FileInfo::readStat( std::string const & rootPath, std::string const & currPath )
{
    this->_rootPath = rootPath;
    this->_currPath = currPath;
    this->_filePath = rootPath + currPath;
    
    int retstat = stat(this->_filePath.c_str(), &this->_fileStat);
    int errstat = errno;
    int retaccess = access(this->_filePath.c_str(), R_OK);
    int erraccess = errno;
    if (errstat == EACCES || erraccess == EACCES) {
        this->_currType = WS_FILE_PERMISSION_DENIED;
    } else if (retstat == 0 && retaccess == 0 && S_ISDIR(this->_fileStat.st_mode)) {
        this->_currType = WS_FILE_DIRECTORY;
    } else if (retstat == 0 && retaccess == 0 && S_ISREG(this->_fileStat.st_mode)) {
        this->_currType = WS_FILE_REGULAR;
    } else {
        this->_currType = WS_FILE_NOT_FOUND;
    }
    return (this->_currType);
}

ws_file_type_t  FileInfo::getFileType( void ) const { return (this->_currType); }

Content FileInfo::getDirectory( std::vector<std::string> const & indexFiles )
{
    std::stringstream ss;
    ss << "<!DOCTYPE html>""<html lang=\"en\">""<head>"
        "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
        "<title>Directory Listing</title>""<style>"
        "* {font-family: sans-serif;}\
        table {border-collapse: collapse;margin: 1em 0;font-size: 1em;min-width: 400px;box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}\
        tr td:first-child {width: 20px;white-space: nowrap;overflow: hidden;text-overflow: ellipsis;}\
        thead tr {background-color: #009879;color: #ffffff;text-align: left;}\
        th, td {padding: 1em 2em;}\
        tbody tr:hover {background-color: #d9d7d7;}\
        tbody tr {border-bottom: 1px solid #dddddd;}\
        tbody tr:nth-of-type(even) {background-color: #f3f3f3;}\
        tr:nth-of-type(even):hover {background-color: #d9d7d7;}"
        "</style>""</head>""<body>""<h1>";

    std::string dirstrs, rootPart;
    std::cout << "currpath: " << this->_currPath << std::endl;
    std::stringstream dirstream(this->_currPath);
    while (std::getline(dirstream, dirstrs, '/')) {
        rootPart += dirstrs + "/";
        std::cout << "rootpart: " << rootPart << std::endl;
        if (dirstrs.empty()) dirstrs = "~";
        std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
        ss << a;
    }
    ss << "</h1><table><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>";
    DIR* currDir = opendir(this->_filePath.c_str());
    if (currDir == NULL)
        return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
    struct dirent *dirElem = NULL;
    while ((dirElem = readdir(currDir))) {
        FileInfo fileInfo;
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

Content FileInfo::getFile( void )
{
    std::ifstream   ifs(this->_filePath.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open())
        return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
    std::size_t filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    this->_fileData.clear();
    this->_fileData.resize(filesize);
    ifs.read(this->_fileData.data(), filesize);
    ifs.close();
    return (Content(this->_currType, this->_fileData.begin(), this->_fileData.end()));
}


Content FileInfo::getContent( std::vector<std::string> const & indexFiles )
{
    if (this->_currType == WS_FILE_PERMISSION_DENIED) {
        return (this->getErrorPage(ws_http::STATUS_403_FORBIDDEN));
    } else if (this->_currType == WS_FILE_PERMISSION_DENIED) {
        return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
    } else if (this->_currType == WS_FILE_REGULAR) {
        return (this->getFile());
    } else if (this->_currType == WS_FILE_DIRECTORY) {
        return (this->getDirectory(indexFiles));
    }
    return (this->getErrorPage(ws_http::STATUS_404_NOT_FOUND));
}

Content FileInfo::getErrorPage( ws_http::statuscodes_t statusCode, std::string const & rootPath, std::string const & currPath )
{
    if (!rootPath.empty() && !currPath.empty()) {
        ws_file_type_t type = this->readStat(rootPath, currPath);
        if (type == WS_FILE_REGULAR) {
            this->dirEntries.clear();
            return (this->getContent(this->dirEntries));
        }
    }
    this->_fileStrData = "<!doctype html>" CRLF "<html lang=\"en\">" CRLF "<head>" CRLF "<meta charset=\"utf-8\">" CRLF "<title>"
    + ws_http::statuscodes.at(statusCode) + "</title>" CRLF "</head>" CRLF "<body>" CRLF "<div style=\"text-align: center;\">" CRLF "<h1>"
    + ws_http::statuscodes.at(statusCode) + "</h1>" CRLF "<hr>" CRLF "<p>" WEBSERV_VERSION "</p>" CRLF "</div>" CRLF "</body>" CRLF "</html>" CRLF;
    return (Content(this->_currType, this->_fileStrData.begin(), this->_fileStrData.end()));
}

// int main(int argc, char**argv)
// {
//     if (argc != 3)
//         return (1);
//     FileInfo info;
    
//     ws_file_type_t type = info.readStat(argv[1], argv[2]);
//     std::cout << "filetype:" << type << std::endl;
    
//     Content  content = info.getContent();
//     std::cout << "content: " << std::string(content.it_start, content.it_end) << std::endl;

//     return (0);
// }





// std::string FileInfo::getSizeFormatted(std::size_t size) {
//     const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
//     std::size_t i = 0;
//     double dSize = static_cast<double>(size);
//     while (dSize >= 1000.0 && i < (sizeof(suffixes) / sizeof(suffixes[0]) - 1)) {
//         dSize /= 1000.0;
//         i++;
//     }
//     std::stringstream ss;
//     ss << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i];
//     return (ss.str());
// }

// std::string FileInfo::getDateFormatted(std::time_t t) {
//     std::tm* now = std::localtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
//     return (std::string(buff, size));
// }

// FileInfo::FileInfo( std::string const & filePath ) : _errorErrno(-1)
// {
//     this->_filePath = filePath;
//     std::size_t pos = filePath.find_last_of('/');
//     if (pos != std::string::npos)
//         this->_name = filePath.substr(pos + 1, std::string::npos);
//     else
//         this->_name = filePath;
// }

// FileInfo::FileInfo( std::string const & rootPath, struct dirent const & dirElem ) : _errorErrno(-1)
// {
//     this->_name = std::string(dirElem.d_name, dirElem.d_namlen);
//     this->_filePath = rootPath + this->_name;
// }

// FileInfo::~FileInfo( void ) { }

// std::string const & FileInfo::getName() const       { return (this->_name); }
// std::string const & FileInfo::getFilePath() const   { return (this->_filePath); }

// std::string FileInfo::getSizeStr() const            { return ( this->_errorErrno == 0 ? FileInfo::getSizeFormatted(this->_fileStat.st_size) : "" ); }
// std::string FileInfo::getModDate() const            { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_mtimespec.tv_sec) : "" ); }
// std::string FileInfo::getAccessDate() const         { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_atimespec.tv_sec) : "" ); }
// std::string FileInfo::getStatusChangeDate() const   { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_ctimespec.tv_sec) : "" ); }

// bool    FileInfo::isDir() const         { return ( this->_errorErrno == 0 ? S_ISDIR(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isBlock() const       { return ( this->_errorErrno == 0 ? S_ISBLK(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isCharDevice() const  { return ( this->_errorErrno == 0 ? S_ISCHR(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isRegular() const      { return ( this->_errorErrno == 0 ? S_ISREG(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isSocket() const      { return ( this->_errorErrno == 0 ? S_ISSOCK(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isLink() const        { return ( this->_errorErrno == 0 ? S_ISLNK(this->_fileStat.st_mode) : false ); }
// bool    FileInfo::isFifo() const        { return ( this->_errorErrno == 0 ? S_ISFIFO(this->_fileStat.st_mode) : false ); }

// int FileInfo::getError() const { return (this->_errorErrno); }

// bool    FileInfo::checkRealPermissions( int permissionFlags ) const { return (access(this->_filePath.c_str(), permissionFlags) == 0 ? true : false); }


// bool    FileInfo::readStat( void )
// {
//     int ret = stat(this->_filePath.c_str(), &this->_fileStat);
//     this->_errorErrno = ret == 0 ? 0 : errno;
//     return (ret == 0 ? true : false);
// }

// bool FileInfo::readDirectory( void )
// {
//     if (this->_errorErrno == -1 && !this->readStat()) {
//         return (false);
//     }
//     if (!this->isDir() || !this->checkRealPermissions(R_OK))
//         return (false);
//     DIR* currDir = opendir(this->_filePath.c_str());
//     if (currDir == NULL)
//         return (false);
//     struct dirent *dirElem = NULL;
//     while ((dirElem = readdir(currDir))) {
//         this->_fileInfos.push_back(FileInfo(this->_filePath, *dirElem));
//         if (!this->_fileInfos.back().readStat()) {
//             this->_fileInfos.pop_back();
//         }
//     }
//     closedir(currDir);
//     return (true);
// }

// FileInfo const & FileInfo::operator[]( std::size_t n)
// {
//     if (n >= this->_fileInfos.size())
//         throw std::out_of_range("FileInfo: out of range");
//     return(this->_fileInfos[n]);
// }

// std::size_t FileInfo::getDirectoryElemCount( void ) const
// {
//     return (this->_fileInfos.size());
// }
