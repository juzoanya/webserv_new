/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfo_old.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 22:57:57 by mberline          #+#    #+#             */
/*   Updated: 2023/12/21 14:21:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileInfo_old.hpp"

std::string FileInfo::getSizeFormatted(std::size_t size) {
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    std::size_t i = 0;
    double dSize = static_cast<double>(size);
    while (dSize >= 1000.0 && i < (sizeof(suffixes) / sizeof(suffixes[0]) - 1)) {
        dSize /= 1000.0;
        i++;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << dSize << " " << suffixes[i];
    return (ss.str());
}

std::string FileInfo::getDateFormatted(std::time_t t) {
    std::tm* now = std::localtime(&t);
    char buff[1000];
    std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
    return (std::string(buff, size));
}

FileInfo::FileInfo( std::string const & filePath ) : _errorErrno(-1)
{
    this->_filePath = filePath;
    std::size_t pos = filePath.find_last_of('/');
    if (pos != std::string::npos)
        this->_name = filePath.substr(pos + 1, std::string::npos);
    else
        this->_name = filePath;
}

FileInfo::FileInfo( std::string const & rootPath, struct dirent const & dirElem ) : _errorErrno(-1)
{
    this->_name = std::string(dirElem.d_name, dirElem.d_namlen);
    this->_filePath = rootPath + this->_name;
}

FileInfo::~FileInfo( void ) { }

std::string const & FileInfo::getName() const       { return (this->_name); }
std::string const & FileInfo::getFilePath() const   { return (this->_filePath); }

std::string FileInfo::getSizeStr() const            { return ( this->_errorErrno == 0 ? FileInfo::getSizeFormatted(this->_fileStat.st_size) : "" ); }
std::string FileInfo::getModDate() const            { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_mtimespec.tv_sec) : "" ); }
std::string FileInfo::getAccessDate() const         { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_atimespec.tv_sec) : "" ); }
std::string FileInfo::getStatusChangeDate() const   { return ( this->_errorErrno == 0 ? FileInfo::getDateFormatted(this->_fileStat.st_ctimespec.tv_sec) : "" ); }

bool    FileInfo::isDir() const         { return ( this->_errorErrno == 0 ? S_ISDIR(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isBlock() const       { return ( this->_errorErrno == 0 ? S_ISBLK(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isCharDevice() const  { return ( this->_errorErrno == 0 ? S_ISCHR(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isRegular() const      { return ( this->_errorErrno == 0 ? S_ISREG(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isSocket() const      { return ( this->_errorErrno == 0 ? S_ISSOCK(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isLink() const        { return ( this->_errorErrno == 0 ? S_ISLNK(this->_fileStat.st_mode) : false ); }
bool    FileInfo::isFifo() const        { return ( this->_errorErrno == 0 ? S_ISFIFO(this->_fileStat.st_mode) : false ); }

int FileInfo::getError() const { return (this->_errorErrno); }

bool    FileInfo::checkRealPermissions( int permissionFlags ) const { return (access(this->_filePath.c_str(), permissionFlags) == 0 ? true : false); }


bool    FileInfo::readStat( void )
{
    int ret = stat(this->_filePath.c_str(), &this->_fileStat);
    this->_errorErrno = ret == 0 ? 0 : errno;
    return (ret == 0 ? true : false);
}

bool FileInfo::readDirectory( void )
{
    if (this->_errorErrno == -1 && !this->readStat()) {
        return (false);
    }
    if (!this->isDir() || !this->checkRealPermissions(R_OK))
        return (false);
    DIR* currDir = opendir(this->_filePath.c_str());
    if (currDir == NULL)
        return (false);
    struct dirent *dirElem = NULL;
    while ((dirElem = readdir(currDir))) {
        this->_fileInfos.push_back(FileInfo(this->_filePath, *dirElem));
        if (!this->_fileInfos.back().readStat()) {
            this->_fileInfos.pop_back();
        }
    }
    closedir(currDir);
    return (true);
}

FileInfo const & FileInfo::operator[]( std::size_t n)
{
    if (n >= this->_fileInfos.size())
        throw std::out_of_range("FileInfo: out of range");
    return(this->_fileInfos[n]);
}

std::size_t FileInfo::getDirectoryElemCount( void ) const
{
    return (this->_fileInfos.size());
}
