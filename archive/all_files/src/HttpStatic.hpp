/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/11 08:21:30 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTP_STATIC_HPP
#define HTTP_STATIC_HPP

#include "headers.hpp"

class HttpStatic {
    public:
        HttpStatic( void );
        ~HttpStatic( void );

        void    handleData( std::vector<char>::iterator it_start, std::vector<char>::iterator it_end );
        void    setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing );
        void    setError(ws_http::statuscodes_t errorStatus, std::string const & filePath);
        ws_http::statuscodes_t      status;
        std::string                 httpHeader;
        std::vector<char>           fileData; //holds data to be written to the client
        bool                        isDirectory;
    private:
        bool    setFile( std::string const & filePath );
        void    checkDirectory( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing);
        void    setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries );
};

#endif