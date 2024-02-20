/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 22:57:17 by mberline          #+#    #+#             */
/*   Updated: 2023/12/21 14:20:50 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

class FileInfo {
    public:
        FileInfo( std::string const & filePath );
        FileInfo( std::string const & rootPath, struct dirent const & dirElem );
        ~FileInfo( void );

        bool                readStat( );
        bool                readDirectory( void );
        std::size_t         getDirectoryElemCount( void ) const;
        FileInfo const &    operator[]( std::size_t n);

        static std::string  getSizeFormatted(std::size_t size);
        static std::string  getDateFormatted(std::time_t t);
        std::string const & getName() const;
        std::string const & getFilePath() const;
        std::string         getSizeStr() const;
        std::string         getAccessDate() const;
        std::string         getModDate() const;
        std::string         getStatusChangeDate() const;
        bool                isDir() const;
        bool                isBlock() const;
        bool                isCharDevice() const;
        bool                isRegular() const;
        bool                isSocket() const;
        bool                isLink() const;
        bool                isFifo() const;
        bool                checkRealPermissions( int permissionFlags ) const;
        int                 getError() const;
        // std::vector<char>   getContent( void ) const;
    private:
        struct stat             _fileStat;
        int                     _errorErrno;
        std::string             _name;
        std::string             _filePath;
        std::vector<FileInfo>   _fileInfos;
};

#endif
