/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 07:41:43 by mberline          #+#    #+#             */
/*   Updated: 2023/09/27 20:20:15 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configs.hpp"

LocationConfig::LocationConfig( void ) : locationType(WEBSERV_CONFIG_LOCATION_UNINITIALIZED)
{
    (void)locationType;
}

LocationConfig::~LocationConfig( void )
{
    
}

LocationConfig::LocationConfig( LocationConfig & src )
 :  ServerConfig(src)
{
    
}
