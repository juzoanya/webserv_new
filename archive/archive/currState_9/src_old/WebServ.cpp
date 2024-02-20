/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 09:43:06 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 18:02:47 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

WebServ::WebServ( void ) : _config(NULL)
{ }

WebServ::~WebServ( void )
{ }


void    WebServ::setConfig(ConfigParser & config)
{
    this->_config = &config;
}

WsIpPort    WebServ::getServerIpPort( ws_config_t const & serverDirectives )
{
    ws_config_t::const_iterator it = serverDirectives.find("listen");
    if (it == serverDirectives.end() || it->second.size() == 0)
        return (WsIpPort("0.0.0.0", "8080"));
    std::size_t pos = it->second[0].find(':');
    if (pos == std::string::npos)
        return (WsIpPort("0.0.0.0", it->second[0]));
    return (WsIpPort(it->second[0].substr(0, pos), it->second[0].substr(pos + 1, std::string::npos)));
}

int	WebServ::createHttpServersByConfig( void )
{
    std::vector<HttpServer*> servers;
	for (int i = 0; i != _config->getServerCount(); ++i) {
        ConfigParser::ServerContext const & currContext = _config->serverConfigs[i];
		try {
			WsIpPort ipPort = getServerIpPort(currContext.serverConfig);
			std::cout << "create server: " << i << ": [" << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
			std::vector<HttpServer*>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
			if (servIt != servers.end()) {
                (*servIt)->addServerConfig(currContext);
				std::cout << "added server config to existing HttpServer" << std::endl; 
			} else {
                servers.push_back(new HttpServer(ipPort, _polling));
                servers.back()->addServerConfig(currContext);
				std::cout << "create instance of HttpServer" << std::endl; 
			}
		} catch(const std::exception& e) {
			std::cout << "unable to create server: " << e.what() << '\n';
		}
	}
    return (servers.size());
}





void    WebServ::startServer( void )
{
    if (_config)
        createHttpServersByConfig();
	// if (_servers.empty())
	// 	_servers.push_back(HttpServer(WsIpPort("0.0.0.0", "8080")));
    
    while (true) {
        // std::cout << "polling...\n";
        NextPollEvent nextPoll = _polling.getNextEvent();
        nextPoll.handler.handleEvent(nextPoll.pollstruct);
    }

    // while (true) {

    //     int ret = poll(this->_pollFds.data(), this->_pollFds.size(), -1);

    //     std::size_t currSize = this->_pollFds.size();
    //     for (std::size_t i = 0; i != currSize; ++i) {
    //         if (this->_pollFds[i].fd == -1 || this->_pollFds[i].revents == 0)
    //             continue;
    //     }
    // }
}
