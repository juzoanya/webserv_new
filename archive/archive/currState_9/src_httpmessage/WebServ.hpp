/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 09:40:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 13:35:07 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEB_SERV_HPP
#define WEB_SERV_HPP

#include "headers.hpp"
#include <queue>

struct NextHttpHandler {
    NextHttpHandler( HttpHandler & handler );
    ~NextHttpHandler( void );
    int index;
    HttpHandler & httpHandler;
};

class WebServ {
    public:
        WebServ( void );
        ~WebServ( void );

        void    setConfig(ConfigParser & config);
        void    startServer( void );
    private:
        WsIpPort    getServerIpPort( ws_config_t const & serverDirectives );
        int	    createHttpServersByConfig( void );
        ConfigParser*               _config;
        Polling                     _polling;
};

#endif