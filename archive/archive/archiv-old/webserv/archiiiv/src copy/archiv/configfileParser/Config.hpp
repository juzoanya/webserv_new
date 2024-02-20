/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 13:11:25 by mberline          #+#    #+#             */
/*   Updated: 2023/12/11 07:34:39 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "Configs.hpp"
#include "LexPars.hpp"

class Config {
    public:
        Config( void );
        ~Config();
        
        int readConfig( const char *filepath );
    private:
        int parseConfig( const std::vector<LexToken>& token );
        std::vector<LocationConfig> locationConfigs;
        LexPars                     lexer;
        
};

struct Directive {
    webserv_lexical_t   nameAlias;
    std::string         name;
    int                 argNbr;
    int                 frequency;
};

#endif
