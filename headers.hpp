/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 19:34:27 by juzoanya          #+#    #+#             */
/*   Updated: 2024/01/11 15:39:36 by juzoanya         ###   ########.fr       */
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
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include "ConfigParser.hpp"
#include "ResponseHandler.hpp"
#include "HttpConstants.hpp"
#include "ConfigHandler.hpp"
#include "RequestHandler.hpp"
#include "HttpServer.hpp"
#include "HttpStatic.hpp"




#endif