/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 19:34:27 by juzoanya          #+#    #+#             */
/*   Updated: 2024/02/09 09:26:33 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERS_HPP
#define HEADERS_HPP


#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>

#define DEFAULT_ROOT_PATH "/var"
typedef std::vector<char>   buff_t;
typedef std::map<std::string, std::vector<std::string> > ws_config_t;

#include <exception>
#include <fstream>
#include <sstream>
#include <cctype>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include "Polling.hpp"
    #include "utils.hpp"
#include "ConfigParser.hpp"
    #include "debugHelpers.hpp"
#include "HttpConstants.hpp"
#include "HttpRequest.hpp"
#include "ConfigHandler.hpp"
    #include "HttpHandler.hpp"
    #include "PollingHandlers.hpp"
#include "HttpServer.hpp"


#endif