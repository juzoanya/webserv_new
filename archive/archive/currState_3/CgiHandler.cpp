/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 09:44:36 by mberline          #+#    #+#             */
/*   Updated: 2024/02/03 20:11:42 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


// #define WS_PARENTSOCKET 0
// #define WS_CHILDSOCKET 1


// CgiHandler::CgiHandler( HttpRequest const & request, HttpConfig const & currConfig,
//             std::string const & serverIp, std::string const & serverPort,
//             std::string const & clientIp, std::string const & clientPort )
// {
//     http_header_map_t::const_iterator headerIt;
//     this->_cgiEnv.push_back(std::string("QUERY_STRING=") + request.query);
//     this->_cgiEnv.push_back(std::string("REQUEST_METHOD=") + request.method);
//     headerIt = request.headerMap.find("content-type");
//     if (headerIt == request.headerMap.end())
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE=") + headerIt->second);
//     else
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE="));
//     this->_cgiEnv.push_back(std::string("CONTENT_LENGTH=") + toStr(request.requestBody.bodyBuffer.size()));
//     this->_cgiEnv.push_back(std::string("SCRIPT_NAME=") + request.pathDecoded);
//     this->_cgiEnv.push_back(std::string("PATH_INFO="));
//     this->_cgiEnv.push_back(std::string("PATH_TRANSLATED=") + currConfig.getRootPath());
//     this->_cgiEnv.push_back(std::string("REQUEST_URI=") + request.fullUri);
//     this->_cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
//     this->_cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
//     this->_cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
//     this->_cgiEnv.push_back(std::string("REMOTE_ADDR=") + clientIp);
//     this->_cgiEnv.push_back(std::string("REMOTE_PORT=") + clientPort);
//     this->_cgiEnv.push_back(std::string("SERVER_ADDR=") + serverIp);
//     this->_cgiEnv.push_back(std::string("SERVER_PORT=") + serverPort);
//     this->_cgiEnv.push_back(std::string("SERVER_NAME=") + currConfig.getServerName());
//     this->_cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));
//     for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//         this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//     this->_enviroment.push_back(NULL);
// }

// int    CgiHandler::createChildProcess( std::string const & cgiBinPath, std::string const & cgiScriptPath )
// {
//     this->_cgiEnv.push_back(std::string("SCRIPT_FILE_NAME=") + cgiScriptPath);
//     this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv.back().c_str()));
//     this->_enviroment.push_back(NULL);
//     this->_cgiArgs.push_back(cgiBinPath);
//     this->_cgiArgs.push_back(cgiScriptPath);
//     for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//         this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//     this->_arguments.push_back(NULL);

//     int sock_pair[2];
//     if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
//         std::cout << "error: socketpair: " << strerror(errno) << std::endl;
//         return (-1);
//     }
//     if (fcntl(sock_pair[WS_CHILDSOCKET], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[WS_CHILDSOCKET]);
//         close(sock_pair[WS_PARENTSOCKET]);
//         return (-1);
//     }
//     if (fcntl(sock_pair[WS_PARENTSOCKET], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[WS_CHILDSOCKET]);
//         close(sock_pair[WS_PARENTSOCKET]);
//         return (-1);
//     }
//     pid_t pid = fork();
//     if (pid == 1) {
//         std::cout << "error: fork: " << strerror(errno) << std::endl;
//         close(sock_pair[WS_CHILDSOCKET]);
//         close(sock_pair[WS_PARENTSOCKET]);
//         return (-1);
//     } else if (pid == 0) {
//         close(sock_pair[WS_PARENTSOCKET]);
//         dup2(sock_pair[WS_CHILDSOCKET], STDOUT_FILENO);
//         dup2(sock_pair[WS_CHILDSOCKET], STDIN_FILENO);
//         // if (chdir(docRoot.c_str()) == -1) {
//         //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
//         //     exit(1);
//         // }
//         execve(this->_cgiArgs[1].c_str(), this->_arguments.data(), this->_enviroment.data());
//         std::cout << "error: execve: " << strerror(errno) << std::endl;
//         close(sock_pair[WS_CHILDSOCKET]);
//         exit(1);
//     } else {
//         close(sock_pair[WS_CHILDSOCKET]);
        
//     }
// }



// "QUERY_STRING="
// "REQUEST_METHOD="
// "CONTENT_TYPE="
// "CONTENT_LENGTH="
// "SCRIPT_FILE_NAME="
// "SCRIPT_NAME="
// "PATH_INFO="
// "PATH_TRANSLATED="
// "REQUEST_URI="
// "SERVER_PROTOCOL="
// "GATEWAY_INTERFACE="
// "SERVER_SOFTWARE="
// "REMOTE_ADDR="
// "REMOTE_PORT="
// "SERVER_ADDR="
// "SERVER_NAME="
// "SERVER_PORT="
// "REDIRECT_STATUS="

