/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/08 22:08:11 by juzoanya         ###   ########.fr       */
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

        void    setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing );
        void    setError(ws_http::statuscodes_t errorStatus, std::string const & filePath);
        ws_http::statuscodes_t      status;
        std::vector<char>           fileData; //holds data to be written to the client
        bool                        isDirectory;
    private:
        bool    setFile( std::string const & filePath );
        void    checkDirectory( std::string const & filePath, std::string const & requestUri, std::vector<std::string> const & indexFiles, bool dirListing);
        void    setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries );
};

#endif