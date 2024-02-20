/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpCgiDef.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 18:26:15 by mberline          #+#    #+#             */
/*   Updated: 2023/11/04 11:07:33 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CGI_DEF
#define HTTP_CGI_DEF

#include <map>
#include <vector>
#include <string>

namespace ws_cgi {

    typedef enum {
        WS_CGI_QUERY_STRING,
        WS_CGI_REQUEST_METHOD,
        WS_CGI_CONTENT_TYPE,
        WS_CGI_CONTENT_LENGTH,
        WS_CGI_SCRIPT_FILE_NAME,
        WS_CGI_SCRIPT_NAME,
        WS_CGI_PATH_INFO,
        WS_CGI_PATH_TRANSLATED,
        WS_CGI_REQUEST_URI,
        WS_CGI_SERVER_PROTOCOL,
        WS_CGI_GATEWAY_INTERFACE,
        WS_CGI_SERVER_SOFTWARE,
        WS_CGI_REMOTE_ADDR,
        WS_CGI_REMOTE_PORT,
        WS_CGI_SERVER_ADDR,
        WS_CGI_SERVER_NAME,
        WS_CGI_SERVER_PORT,
        WS_CGI_REDIRECT_STATUS
    } ws_cgi_env_var_t;

    extern const std::map<ws_cgi_env_var_t, const std::string>  cgiEnvMap;

    std::string headerToCgiEnv( std::string const &headerField );
    
    class Cgi {
    public:
        Cgi( void );
        ~Cgi( void );
        void            addEnvVariable(ws_cgi_env_var_t variable, std::string const & value);
        void            addEnvVariable(std::string const & variable, std::string const & value);
        char* const*    getCgiEnviroment( void );
    private:
        std::vector<std::string>    _cgiEnviroStrings;
        std::vector<char*>          _cgiEnviroPtr;
    };
    
}


#endif
