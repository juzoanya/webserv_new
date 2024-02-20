/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileInfos.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:57:00 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 13:57:45 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_INFOS_HPP
#define FILE_INFOS_HPP

#include "headers.hpp"

class FileInfos {
    public:
        FileInfos( std::string const & filePath, std::string const & requestPathDecoded );
        ~FileInfos( void );
        std::string const & checkContainedFile( std::vector<std::string> const & files );
        int                 setDirListing( std::stringstream & ss );
        mode_t  getMode( void ) const;
        int     getError( void ) const;
    private:
        struct DirListingInfos {
            DirListingInfos( std::string const & name, struct stat & fileStat );
            ~DirListingInfos( void );
            std::string fileName;
            std::string fileSize;
            std::string fileModDate;
            mode_t      statFileType;
        };
        mode_t  _fileType;
        int     _fileError;
        // std::string const & _filePath;
        std::string         _requestPathDecoded;
        std::string dummyStr;
        std::vector<DirListingInfos>    _dirListingInfos;
};

#endif
