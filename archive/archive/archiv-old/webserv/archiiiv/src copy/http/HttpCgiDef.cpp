/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpCgiDef.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 18:26:29 by mberline          #+#    #+#             */
/*   Updated: 2023/11/15 15:06:15 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "HttpCgiDef.hpp"

namespace ws_cgi {

    namespace {
        std::map<ws_cgi_env_var_t, const std::string> initCgiEnvMap()
        {
            std::map<ws_cgi_env_var_t, const std::string> cgiEnvMap;

            cgiEnvMap.insert(std::make_pair(WS_CGI_QUERY_STRING, "QUERY_STRING="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_REQUEST_METHOD, "REQUEST_METHOD="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_CONTENT_TYPE, "CONTENT_TYPE="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_CONTENT_LENGTH, "CONTENT_LENGTH="));

            cgiEnvMap.insert(std::make_pair(WS_CGI_SCRIPT_FILE_NAME, "SCRIPT_FILE_NAME="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SCRIPT_NAME, "SCRIPT_NAME="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_PATH_INFO, "PATH_INFO="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_PATH_TRANSLATED, "PATH_TRANSLATED="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_REQUEST_URI, "REQUEST_URI="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SERVER_PROTOCOL, "SERVER_PROTOCOL="));

            cgiEnvMap.insert(std::make_pair(WS_CGI_GATEWAY_INTERFACE, "GATEWAY_INTERFACE="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SERVER_SOFTWARE, "SERVER_SOFTWARE="));

            cgiEnvMap.insert(std::make_pair(WS_CGI_REMOTE_ADDR, "REMOTE_ADDR="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_REMOTE_PORT, "REMOTE_PORT="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SERVER_ADDR, "SERVER_ADDR="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SERVER_NAME, "SERVER_NAME="));
            cgiEnvMap.insert(std::make_pair(WS_CGI_SERVER_PORT, "SERVER_PORT="));

            cgiEnvMap.insert(std::make_pair(WS_CGI_REDIRECT_STATUS, "REDIRECT_STATUS="));

            return (cgiEnvMap);
        }

        int charToupperUnderscore( int c )
        {
            if (c == '-')
                return ('_');
            return (::toupper(c));
        }
    }

    const std::map<ws_cgi_env_var_t, const std::string>  cgiEnvMap = initCgiEnvMap();

    std::string headerToCgiEnv( std::string const &headerField )
    {
        std::string cgiHeader = "HTTP_";
        std::transform(headerField.begin(), headerField.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
        cgiHeader.push_back('=');
        return (cgiHeader);
    }

    Cgi::Cgi( void )
    { }

    Cgi::~Cgi( void )
    { }

    void            Cgi::addEnvVariable(ws_cgi_env_var_t variable, std::string const & value)
    {
        this->_cgiEnviroStrings.push_back(cgiEnvMap.at(variable) + value);
    }

    void            Cgi::addEnvVariable(std::string const & variable, std::string const & value)
    {
        this->_cgiEnviroStrings.push_back(variable + value);
    }

    char* const*    Cgi::getCgiEnviroment( void )
    {
        std::vector<std::string>::iterator it;
        for (it = this->_cgiEnviroStrings.begin(); it != this->_cgiEnviroStrings.end(); ++it) {
            this->_cgiEnviroPtr.push_back(const_cast<char*>(it->c_str()));
        }
        this->_cgiEnviroPtr.push_back(NULL);
        
        return (this->_cgiEnviroPtr.data());
    }
}
