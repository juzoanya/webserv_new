/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 19:34:27 by juzoanya          #+#    #+#             */
/*   Updated: 2024/02/23 13:40:41 by juzoanya         ###   ########.fr       */
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

#define EMPTY_STRING ""
#define DEFAULT_ROOT_PATH "/home/juzoanya/webserv_new/www"
#define DEFAULT_TIMEOUT_MS 10000
typedef std::string   buff_t;
typedef std::map<std::string, std::vector<std::string> > ws_config_t;

#include <exception>
#include <fstream>
#include <sstream>
#include <cctype>
#include <queue>
#include <bitset>

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
#include <limits>
#include <csignal>
#include <sys/wait.h>

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>


#include "HttpConstants.hpp"
#include "utils.hpp"
#include "Polling.hpp"
#include "ConfigParser.hpp"
#include "ConfigHandler.hpp"
#include "HttpHeader.hpp"
#include "HttpMessage.hpp"
#include "HttpServer.hpp" // uses IOnHttpMessageBodySize from HttpMessage.hpp
#include "CgiHandler.hpp"
#include "HttpHandler.hpp" // uses IParentProcess from CgiHandler.hpp

#endif
