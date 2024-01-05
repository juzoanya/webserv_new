/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatic.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 11:59:32 by mberline          #+#    #+#             */
/*   Updated: 2024/01/05 09:41:00 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STATIC_HPP
#define HTTP_STATIC_HPP

#include "headers.hpp"


#include "ConfigParser.hpp"
#include "HttpConstants.hpp"

class HttpStatic {
    public:
        HttpStatic( void );
        ~HttpStatic( void );

        void    setContentByPath( std::string const & filePath, std::string const & requestUri, std::vector<std::string>* indexFiles, std::vector<std::string>* dirListing );
        ws_http::statuscodes_t      status;
        std::vector<char>           fileData;
        bool                        isDirectory;
    private:
        void    setFile( std::string const & filePath );
        void    checkDirectory( std::string const & filePath, std::string const & requestUri);
        void    setDirListing( std::string requestUri, std::vector< std::pair<std::string, struct stat> >& dirEntries );

        std::vector<std::string>*   _indexFiles;
        std::vector<std::string>*   _dirListing;
};

#endif