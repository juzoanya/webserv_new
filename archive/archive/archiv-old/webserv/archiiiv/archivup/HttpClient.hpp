/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 16:49:50 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 16:57:45 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include "ConfigParser.hpp"

struct Content {
    Content( void );
    Content( const char* dataPtr, int dataSize );
    ~Content( void );
    const char* data;
    int         size;
};

class   HttpClient {
    public:
        HttpClient( ConfigParser::ServerContext& serverConfig );
        ~HttpClient( void );
        
        Content*    getHttpContent( void );
        void        handleData(std::vector<char>::iterator it_start, std::vector<char>::iterator it_end);

    private:
        std::vector<std::string>*  findDirective( std::string const & key );
        ws_config_t*	_serverConfig;
        ws_config_t*	_locationConfig;
        std::string method;
        std::string path;
        std::string pathDecoded;
        std::string query;
        std::map<std::string, std::string>  headers;
};

#endif