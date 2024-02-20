/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 22:57:17 by mberline          #+#    #+#             */
/*   Updated: 2023/12/21 21:02:35 by mberline         ###   ########.fr       */
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

#include "http/HttpHeaderDef.hpp"

typedef enum {
    WS_FILE_UNSET = 0,
    WS_FILE_REGULAR = 1,
    WS_FILE_DIRECTORY = 2,
    WS_FILE_NOT_FOUND = 3,
    WS_FILE_PERMISSION_DENIED = 4
} ws_file_type_t;

// struct Content {
//     Content( ws_file_type_t contentType, void const * contentData, int contentSize );
//     ~Content( void );
//     ws_file_type_t    fileType;
//     void const*       data;
//     int         size;
// };

struct Content {
    Content( void );
    Content( ws_file_type_t contentType, buff_t::const_iterator itstart, buff_t::const_iterator itend );
    ~Content( void );
    ws_file_type_t    fileType;
    buff_t::const_iterator it_start;
    buff_t::const_iterator it_end;
    // void const* data;
    // int         size;
};

class FileInfo {
    public:
        FileInfo( void );
        ~FileInfo( void );

        ws_file_type_t  readStat( std::string const & rootPath, std::string const & currPath );
        ws_file_type_t  getFileType( void ) const;
        Content         getContent( std::vector<std::string> const & indexFiles );
        Content         getErrorPage( ws_http::statuscodes_t statusCode, std::string const & rootPath = "", std::string const & currPath = "" );
        std::vector<std::string> const &    getDirEntryNames( void ) const;
    protected:
        struct stat             _fileStat;
    
    private:
        Content         getDirectory( std::vector<std::string> const & indexFiles );
        Content         getFile( void );
        ws_file_type_t      _currType;
        std::string         _rootPath;
        std::string         _currPath;
        std::string         _filePath;
        std::vector<char>   _fileData;
        std::string         _fileStrData;
        std::vector<std::string> dirEntries;
};

#endif
