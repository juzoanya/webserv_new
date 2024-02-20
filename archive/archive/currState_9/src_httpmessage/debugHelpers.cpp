/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugHelpers.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:02:06 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 16:27:41 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

void    debPrintStrVector(std::string const & msg, std::vector<std::string> const & vec, int width)
{
    std::cout << std::setw(width) << msg << ": [";
    for (std::vector<std::string>::const_iterator itt = vec.begin(); itt != vec.end(); ++itt) {
        std::cout << *itt;
        if (itt < vec.end() - 1)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void	debPrintConfigDirectives(std::string const & msg, ws_config_t const & config, int width)
{
    std::cout << std::setw(width) << "------------------ " << msg << " ------------------" << std::endl;
	for (ws_config_t::const_iterator it = config.begin(); it != config.end(); ++it) {
        debPrintStrVector(it->first, it->second, width);
    }
}

void    debPrintConfigSelectedDirective(std::string const & msg, std::string const & directive, ws_config_t const & config, int width)
{
    ws_config_t::const_iterator it = config.find(directive);
    if (it == config.end())
        std::cout << msg << directive << ": not found";
    else
        debPrintStrVector(msg + it->first, it->second, width);
}

void    debPrintIpPortData(std::string const & msg, WsIpPort const & ipPort)
{
    std::cout << msg << ": [" << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]" << std::endl;
}

void    debPrintServerContext(std::string const & msg, ConfigParser::ServerContext const & serverContext, bool printLocation)
{
    std::cout << "------------------ " << msg <<  " ------------------" << std::endl;
    debPrintConfigDirectives("serverConfig", serverContext.serverConfig, 0);
    if (printLocation) {
        for (std::size_t i = 0; i != serverContext.locationConfig.size(); ++i) {
            std::cout << "--------------------" << std::endl;
            debPrintConfigDirectives("locationConfig", serverContext.locationConfig[i], 20);
            std::cout << "--------------------" << std::endl;
        }
    }
}

void	debPrintServerData(std::vector<HttpServer>& servers, bool printLocation)
{
	for (std::size_t i = 0; i != servers.size(); ++i) {
        debPrintIpPortData(std::string("server: ") +  toStr(i), servers[i].serverIpPort);
        std::vector<ConfigParser::ServerContext const *> const & contextVec = servers[i].getServerConfigs();
        for (std::size_t j = 0; j != contextVec.size(); ++j) {
            if (contextVec[j])
                debPrintServerContext("serverContext: " + toStr(j), *contextVec[j], printLocation);
            else
                std::cout << "serverContext: " << j << " is NULL";
        }

	}
}
