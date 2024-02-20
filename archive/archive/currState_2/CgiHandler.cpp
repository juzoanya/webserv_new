/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 09:44:36 by mberline          #+#    #+#             */
/*   Updated: 2024/01/25 20:25:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

"QUERY_STRING="
"REQUEST_METHOD="
"CONTENT_TYPE="
"CONTENT_LENGTH="
"SCRIPT_FILE_NAME="
"SCRIPT_NAME="
"PATH_INFO="
"PATH_TRANSLATED="
"REQUEST_URI="
"SERVER_PROTOCOL="
"GATEWAY_INTERFACE="
"SERVER_SOFTWARE="
"REMOTE_ADDR="
"REMOTE_PORT="
"SERVER_ADDR="
"SERVER_NAME="
"SERVER_PORT="
"REDIRECT_STATUS="

#define WS_PARENTSOCKET 0
#define WS_CHILDSOCKET 1

void    createChildProcess()
{

    char** args;
    char** envp;

    int sock_pair[2];
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
        std::cout << "error: socketpair: " << strerror(errno) << std::endl;
        return ;
    }
    if (fcntl(sock_pair[WS_CHILDSOCKET], F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        close(sock_pair[WS_PARENTSOCKET]);
        return ;
    }
    if (fcntl(sock_pair[WS_PARENTSOCKET], F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        close(sock_pair[WS_PARENTSOCKET]);
        return ;
    }
    pid_t pid = fork();
    if (pid == 1) {
        std::cout << "error: fork: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        close(sock_pair[WS_PARENTSOCKET]);
        return ;
    } else if (pid == 0) {
        close(sock_pair[WS_PARENTSOCKET]);
        execve("path/to/cgi-executable", args, envp);
        std::cout << "error: execve: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        exit(1);
    } else {
        close(sock_pair[WS_CHILDSOCKET]);
        
    }
}