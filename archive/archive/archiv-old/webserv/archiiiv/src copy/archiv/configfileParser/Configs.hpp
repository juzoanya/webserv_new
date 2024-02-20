/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/26 11:33:05 by mberline          #+#    #+#             */
/*   Updated: 2023/12/19 12:00:46 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <string>
#include <vector>
#include <map>

typedef enum {
    WEBSERV_CONFIG_LOCATION_UNINITIALIZED = 0,
    WEBSERV_CONFIG_LOCATION_ABSOLUTE,
    WEBSERV_CONFIG_LOCATION_PREFIX
    
}   ws_config_location_type_t;

struct ServerConfig {
        ServerConfig( void );
        ~ServerConfig( void );
        ServerConfig( ServerConfig & src );

        std::string                 hostname;
        std::string                 ip_address;
        std::vector<std::string>    ports;
        std::vector<std::string>    server_names;

        std::map<int, std::string>  error_pages;
        int                         client_max_body_size;
        std::vector<std::string>    indexFiles;
        bool                        directoryListing;
        std::string                 rootDir;
        std::vector<std::string>    allowed_methods;
};

struct LocationConfig : public ServerConfig {
        LocationConfig( void );
        ~LocationConfig( void );
        LocationConfig( LocationConfig & src );

        ws_config_location_type_t   locationType;
        std::string                 locationStr;
};


#endif
