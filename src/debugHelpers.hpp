/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugHelpers.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:02:23 by mberline          #+#    #+#             */
/*   Updated: 2024/02/15 08:48:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HELPERS_HPP
#define DEBUG_HELPERS_HPP

#include "headers.hpp"

void    debPrintStrVector(std::string const & msg, std::vector<std::string> const & vec, int width);
void	debPrintConfigDirectives(std::string const & msg, ws_config_t const & config, int width);
void    debPrintConfigSelectedDirective(std::string const & msg, std::string const & directive, ws_config_t const & config, int width);
void    debPrintServerContext(std::string const & msg, ConfigParser::ServerContext const & serverContext, bool printLocation);
void	debPrintServerData(std::vector<HttpServer>& servers, bool printLocation);


class WsLogger {
    public:

    private:
};

#endif
