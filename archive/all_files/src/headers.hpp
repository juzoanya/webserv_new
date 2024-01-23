/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 19:34:27 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/19 22:54:23 by mberline         ###   ########.fr       */
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

#include "ConfigParser.hpp"
#include "HttpConstants.hpp"
#include "RequestHandler.hpp"
#include "ResponseHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpStatic.hpp"
#include "ConfigHandler.hpp"
#include "HttpServer.hpp"



#endif