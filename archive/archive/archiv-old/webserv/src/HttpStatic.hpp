/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/07 11:15:27 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATIC_HPP
#define HTTP_STATIC_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "ConfigParser.hpp"
#include "HttpConstants.hpp"

class HttpStatic {
    public:
        HttpStatic( void );
        ~HttpStatic( void );

        void    setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing );
        void    setError(ws_http::statuscodes_t errorStatus, std::string const & filePath);
        ws_http::statuscodes_t      status;
        std::vector<char>           fileData;
        bool                        isDirectory;
    private:
        bool    setFile( std::string const & filePath );
        void    checkDirectory( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing);
        void    setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries );
};

#endif