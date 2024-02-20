/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 12:00:51 by mberline          #+#    #+#             */
/*   Updated: 2023/12/19 12:31:42 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
#define CONF_HPP

#include <vector>
#include <string>
#include <utility>
#include <map>

typedef std::pair<std::string, std::string> ip_port_t;

struct Configuration {
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

class Config {

    public:
        Config( void );
        ~Config( void );

        int parseConfig( const char *filepath );
    private:
        std::vector< ip_port_t >   ip_port_pairs;
    
};


#endif