/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 12:06:23 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 14:10:21 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "HttpMessage.hpp"
#include "headers.hpp"

HttpMessage::HttpMessage( ConfigHandler* configHandler )
{ }

HttpMessage::~HttpMessage( void )
{ }


ws_http::statuscodes_t HttpMessage::handleDataFromSocket( buff_t::const_iterator it_start, buff_t::const_iterator itEnd )
{
    
}

int HttpMessage::writeToSocket( int socketfd )
{

}
