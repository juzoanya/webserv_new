/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   siegetest.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/04 14:49:23 by mberline          #+#    #+#             */
/*   Updated: 2024/01/04 20:19:21 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

int createServerSocket(struct addrinfo *res, int backlog, int kq)
{
    int serversocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serversocket == -1)
        return (-1);
    int err = bind(serversocket, res->ai_addr, res->ai_addrlen);
    if (err == 0)
        err = listen(serversocket, backlog);
    if (err == 0)
        err = fcntl(serversocket, F_SETFL, O_NONBLOCK);
    if (err == 0) {
        struct kevent   kev;
        EV_SET(&kev, serversocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        err = kevent(kq, &kev, 1, NULL, 0, NULL);
    }
    if (err == -1) {
        close(serversocket);
        return (-1);
    }
    return (serversocket);
}

void    newClientConnection(int kq, int serversocket, std::map<int, bool>& clientData)
{
    struct sockaddr addr;
    socklen_t   addrlen = sizeof(addr);
    int clientsocket = accept(serversocket, &addr, &addrlen);
    if (clientsocket == -1)
        return ;
    if (fcntl(clientsocket, F_SETFL, O_NONBLOCK) == -1) {
        close(clientsocket);
        return ;
    }
    #ifdef __APPLE__
    int set = 1;
    if (setsockopt(clientsocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) == -1) {
        close(clientsocket);
        return ;
    }
    #endif
    struct kevent kev[2];
    EV_SET(kev, clientsocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(kev + 1, clientsocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, kev, 2, NULL, 0, NULL) == -1) {
        close(clientsocket);
        return ;
    }
    clientData.insert(std::make_pair(clientsocket, false));
}


void    handleClient(struct kevent& kev, std::map<int, bool>& clientData, std::vector<char>& readData)
{
    std::map<int, bool>::iterator clientDataIt = clientData.find(kev.ident);
    if (clientDataIt == clientData.end()) {
        std::cout << "clientdata not found in map\n";
        return ;
    }
    if (kev.filter == EVFILT_READ) {
        // std::cout << "read event, socket: " << kev.ident << std::endl;
        int sizeread = recv(kev.ident, readData.data(), 4096, 0);
        if (sizeread == -1) {
            std::cout << "error: recv: " << strerror(errno) << std::endl;
            close(kev.ident);
            clientData.erase(clientDataIt);
        } else if (sizeread == 0) {
            std::cout << "the client closed his write side, no more data to read" << std::endl;
        } else {
            std::string ending = "\r\n\r\n";
            if (std::search(readData.begin(), readData.begin() + sizeread, ending.begin(), ending.end()) != readData.begin() + sizeread) {
                // std::cout << "ending found!\n";
                clientDataIt->second = true;
            } else
                std::cout << "ending NOT found!\n";
        }
    } else if (kev.filter == EVFILT_WRITE) {
        if (clientDataIt->second == false)
            return ;
        // std::cout << "write event and true\n";
        std::string httpresponse = 
            "HTTP/1.1 200 OK\r\n"
            "connection: close\r\n"
            "content-legth: 13\r\n"
            "content-type: text/plain\r\n"
            "\r\n"
            "Guten Morgen!";
        int sizewritten = send(kev.ident, httpresponse.data(), httpresponse.size(), 0);
        if (sizewritten == -1) {
            std::cout << "error: recv: " << strerror(errno) << std::endl;
            close(kev.ident);
            clientData.erase(clientDataIt);
        } else {
            if (sizewritten == httpresponse.size()) {
                // std::cout << "response sent!\n";
            }
            // std::cout << "delete client!\n";
            close(kev.ident);
            clientData.erase(clientDataIt);
        } 
    }
}


int main( void )
{
    int kq = kqueue();
    if (kq == -1) {
        std::cerr << strerror(errno) << std::endl;
        return (1);
    }
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int gaierr = getaddrinfo("localhost", "8080", &hints, &res);
    if (gaierr != 0) {
        std::cerr << gai_strerror(gaierr) << std::endl;
        return (1);
    }
    int serversocket = createServerSocket(res, 100, kq);
    freeaddrinfo(res);
    if (serversocket == -1) {
        std::cerr << strerror(errno) << std::endl;
        return (1);
    }
    struct kevent   kevlist[100];
    std::vector<char>    read_data(4096);
    std::map<int, bool> clientData;
    while (true) {
        int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
        for (int i = 0; i != nevents; ++i) {
            struct kevent& currKev = kevlist[i];
            if (currKev.flags & EV_EOF) {
                std::cout << "eof found at socket: " << currKev.ident << std::endl;
            }
            if (currKev.flags & EV_ERROR) {
                std::cout << "error found: " << strerror(currKev.data) << " | at socket: " << currKev.ident << std::endl;
            }
            if (currKev.filter == EVFILT_READ && currKev.ident == serversocket) {
                newClientConnection(kq, currKev.ident, clientData);
            } else {
                handleClient(currKev, clientData, read_data);
            }
        }
    }
}