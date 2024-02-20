/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 07:41:47 by mberline          #+#    #+#             */
/*   Updated: 2023/09/27 08:26:47 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configs.hpp"

ServerConfig::ServerConfig( void ) : client_max_body_size(0)
{

}

ServerConfig::~ServerConfig( void )
{
    
}

ServerConfig::ServerConfig( ServerConfig & src )
 :  hostname(src.hostname), ports(src.ports), server_names(src.server_names),
    error_pages(src.error_pages), client_max_body_size(src.client_max_body_size)
{
    
}
