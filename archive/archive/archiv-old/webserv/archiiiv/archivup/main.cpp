/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 15:53:02 by mberline          #+#    #+#             */
/*   Updated: 2024/01/02 11:46:11 by mberline         ###   ########.fr       */
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

#include "ConfigParser.hpp"
#include "ClientData.hpp"
#include "SocketHandle.hpp"

#define BUFFERSIZE 4096

// void    createServers(int kq, ConfigParser& config, std::map<SocketHandle, ConfigParser::ServerContext*, SocketCmp> serverData)
// {
//     for (int i = 0; i != config.getServerCount(); ++i) {
//         ws_config_t& serverconfig = config.serverConfigs[i].serverConfig;
//         ws_config_t::iterator it = serverconfig.find("listen");
//         std::cout << "create server: " << i << ": ";
//         if (it == serverconfig.end() || it->second.size() == 0) {
//             std::cout << "no listening host:port for this server" << std::endl;
//             continue;
//         }
//         std::string host, port;
//         std::size_t pos = it->second[0].find(':');
//         host = pos == std::string::npos ? "0.0.0.0" : it->second[0].substr(0, pos);
//         port = pos == std::string::npos ? it->second[0] : it->second[0].substr(pos + 1, std::string::npos);

//         try {
//             SocketHandle    serverHandle(host, port, 100);
//             serverData.insert(std::make_pair(serverHandle, &config.serverConfigs[i]));
//             struct kevent   kev;
//             EV_SET(&kev, serverHandle.getSocket(), EVFILT_READ, EV_ADD, 0, 0, const_cast<SocketHandle*>(&serverData.find(serverHandle)->first));
//             if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
//                 serverData.erase(serverHandle);
//                 throw std::runtime_error(strerror(errno));
//             }
//         } catch(const std::exception& e) {
//             std::cerr << e.what() << '\n';
//         }
//     }
// }

int createServerSocket(std::string const & host, std::string const & port, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int gaierr = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (gaierr != 0)
        throw std::runtime_error(gai_strerror(gaierr));
    memcpy(&hints, res, sizeof(struct addrinfo));
    freeaddrinfo(res);
    int serversocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (serversocket == -1)
        throw std::runtime_error(strerror(errno));
    int err = bind(serversocket, res->ai_addr, res->ai_addrlen);
    if (err == 0)
        err = listen(serversocket, backlog);
    if (err == 0)
        err = fcntl(serversocket, F_SETFL, O_NONBLOCK);
    if (err != 0) {
        close(serversocket);
        throw std::runtime_error(strerror(errno));
    }
    this->_sockfd = serversocket;
    setIpPort(*reinterpret_cast<struct sockaddr_in*>(&hints.ai_addr), this->_ipString, this->_portString);
}

void    createServers(int kq, ConfigParser& config, std::map<SocketHandle, ConfigParser::ServerContext*, SocketCmp> serverData)
{
    for (int i = 0; i != config.getServerCount(); ++i) {
        ws_config_t& serverconfig = config.serverConfigs[i].serverConfig;
        ws_config_t::iterator it = serverconfig.find("listen");
        std::cout << "create server: " << i << ": ";
        if (it == serverconfig.end() || it->second.size() == 0) {
            std::cout << "no listening host:port for this server" << std::endl;
            continue;
        }
        std::string host, port;
        std::size_t pos = it->second[0].find(':');
        host = pos == std::string::npos ? "0.0.0.0" : it->second[0].substr(0, pos);
        port = pos == std::string::npos ? it->second[0] : it->second[0].substr(pos + 1, std::string::npos);

        try {
            SocketHandle    serverHandle(host, port, 100);
            serverData.insert(std::make_pair(serverHandle, &config.serverConfigs[i]));
            struct kevent   kev;
            EV_SET(&kev, serverHandle.getSocket(), EVFILT_READ, EV_ADD, 0, 0, const_cast<SocketHandle*>(&serverData.find(serverHandle)->first));
            if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
                serverData.erase(serverHandle);
                throw std::runtime_error(strerror(errno));
            }
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}


int event_loop(ConfigParser& config)
{
    int kq = kqueue();
    if (kq == -1) {
        std::cout << "error: kqueue: " << strerror(errno) << std::endl;
        return (-1);
    }
    // std::map<SocketHandle, ConfigParser::ServerContext*, SocketCmp> serverData;
    // std::map<SocketHandle, HttpClientData, SocketCmp>               clientData;
    std::map<int, ConfigParser::ServerContext*> serverData;
    std::map<int, HttpClientData>               clientData;
    struct kevent                   kevlist[100];
    buff_t                          read_data(4096);
    createServers(kq, config, serverData);
    if (serverData.size() == 0)
        return (-1);
    while (true) {
        int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
        for (int i = 0; i != nevents; ++i) {
            struct kevent& currKev = kevlist[i];
            if (currKev.filter == EVFILT_READ) {
                
            } else if (currKev.filter == EVFILT_READ) {
                readClientData(currKev, read_data, client_data);
            } else if (currKev.filter == EVFILT_WRITE) {
                writeClientData(currKev, client_data);
            }

        }
    }
}

int	main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "invalid number of arguments";
		return (1);
	}
	ConfigParser parser;
    try
    {
	    parser.configParser(argv[1]);
        event_loop(parser);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return (0);
}



// int event_loop(ConfigParser& config)
// {
//     int kq = kqueue();
//     if (kq == -1) {
//         std::cout << "error: kqueue: " << strerror(errno) << std::endl;
//         return (-1);
//     }
//     // std::map<SocketHandle, ConfigParser::ServerContext*, SocketCmp> serverData;
//     // std::map<SocketHandle, HttpClientData, SocketCmp>               clientData;
//     std::map<int, ConfigParser::ServerContext*> serverData;
//     std::map<int, HttpClientData>               clientData;
//     struct kevent                   kevlist[100];
//     buff_t                          read_data(4096);
//     createServers(kq, config, serverData);
//     if (serverData.size() == 0)
//         return (-1);
//     while (true) {
//         int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
//         for (int i = 0; i != nevents; ++i) {
//             struct kevent& currKev = kevlist[i];
//             if (currKev.filter == EVFILT_READ) {
                
//             } else if (currKev.filter == EVFILT_READ) {
//                 readClientData(currKev, read_data, client_data);
//             } else if (currKev.filter == EVFILT_WRITE) {
//                 writeClientData(currKev, client_data);
//             }

//         }
//     }
// }

// int	main(int argc, char** argv)
// {
// 	if (argc != 2) {
// 		std::cerr << "invalid number of arguments";
// 		return (1);
// 	}
// 	ConfigParser parser;
//     try
//     {
// 	    parser.configParser(argv[1]);
//         event_loop(parser);
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
    
//     return (0);
// }



// int setHeadersAndError(ClientData &clientData, ConfigHandler &handler, ws_http::statuscodes_t statusCode, std::string const & mimeType)
// {
//     std::stringstream   headerstream;
//     int fileSize = clientData.bodyIterator.it_end - clientData.bodyIterator.it_start;
//     if (statusCode != ws_http::STATUS_200_OK) {
//         headerstream << statusCode;
//         clientData.bodyIterator = clientData.fileInfo.getErrorPage(statusCode, handler.getErrorPage(headerstream.str()));
//     }
//     headerstream << "HTTP/1.1 " << ws_http::statuscodes.at(statusCode) << CRLF;
//     // std::map<const std::string, const std::string>::const_iterator  mimeTypeIt = ws_http::mimetypes.find(ws_http::getFileExtension(filePath));
//     // headerstream << "content-type: " << (mimeTypeIt == ws_http::mimetypes.end() ? "application/ octet-stream" : mimeTypeIt->second) << CRLF;
//     // std::map<const std::string, const std::string>::const_iterator  mimeTypeIt = ws_http::mimetypes.find(ws_http::getFileExtension(filePath));
//     headerstream << "connection: close" << CRLF;
//     headerstream << "content-type: " << mimeType << CRLF;
//     headerstream << "content-length: " << fileSize << CRLF;
//     std::time_t t = std::time(0);
//     std::tm* now = std::gmtime(&t);
//     char buff[1000];
//     std::size_t size = strftime(buff, 1000, "%d.%m.%Y %H:%M:%S", now);
//     buff[size] = 0;
//     headerstream << "date: " << buff << CRLF << CRLF;
//     clientData.headerData = headerstream.str();
//     clientData.headerIterator.it_start = clientData.headerData.begin();
//     clientData.headerIterator.it_end = clientData.headerData.end();
//     return (0);
// }

// int handleClientData(ClientData &clientData, buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     ws_http_parser_status_t parserStatus = clientData.request.parseRequest(it_start, it_end);
//     if (parserStatus == WS_HTTP_PARSE_PARSING)
//         return (0);
//     ConfigHandler handler(clientData.serverConfig, clientData.request);
//     if (parserStatus >= WS_HTTP_PARSE_ERROR_DEFAULT) {
//         setHeadersAndError(clientData, handler, ws_http::STATUS_400_BAD_REQUEST, "text/html");
//         return (-1);
//     }
//     if (!handler.checkAllowedMethod(ws_http::methods_rev.at(clientData.request.getRequestLine().getMethod()))) {
//         setHeadersAndError(clientData, handler, ws_http::STATUS_405_METHOD_NOT_ALLOWED, "text/html");
//         return (-1);
//     }
//     ws_file_type_t type = clientData.fileInfo.readStat(handler.getDocRoot(), clientData.request.getRequestLine().getPathDecoded());
//     std::map<const std::string, const std::string>::const_iterator  mimeTypeIt;
//     mimeTypeIt = ws_http::mimetypes.find(ws_http::getFileExtension(clientData.request.getRequestLine().getPathDecoded()));
//     if (type == WS_FILE_PERMISSION_DENIED) {
//         setHeadersAndError(clientData, handler, ws_http::STATUS_403_FORBIDDEN, "text/html");
//     } else if (type == WS_FILE_NOT_FOUND) {
//         setHeadersAndError(clientData, handler, ws_http::STATUS_404_NOT_FOUND, "text/html");
//     } else if (type == WS_FILE_REGULAR) {
//         clientData.bodyIterator = clientData.fileInfo.getContent(handler.getIndexFile());
//     } else if (type == WS_FILE_DIRECTORY) {

//     }
//     return (0);
// }





// int readClientData(struct kevent& currKev, buff_t & read_data, std::map<int, HttpClientData>& client_data)
// {
//     int sizeread = recv(currKev.ident, read_data.data(), 4096, 0);
//     if (sizeread == -1) {
//         std::cout << "error: recv: " << strerror(errno) << std::endl;
//         close(currKev.ident);
//         client_data.erase(currKev.ident);
//     } else if (sizeread == 0) {
//         std::cout << "the client closed his write side, no more data to read" << std::endl;
//         close(currKev.ident);
//         client_data.erase(currKev.ident);
//         // fd_data[currKev.ident] = &currKev.ident;
//     } else {
//         std::vector<char>::iterator readit = std::search(read_data.begin(), read_data.begin() + sizeread,
//             ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//         if (readit != read_data.begin() + sizeread)
//             client_data[currKev.ident] = &currKev.ident;
//         // handleClientData(client_data.at(currKev.ident), read_data.begin(), read_data.end());
//         // client_data.at(currKev.ident).request.parseRequest(read_data.begin(), read_data.end());
//     }
// }

    // if (client_data[currKev.ident]) {
    //     std::string response =
    //         "HTTP/1.1 200 OK" CRLF
    //         "content-type: text/plain" CRLF
    //         "content-legth: 13" CRLF CRLF
    //         "Guten Morgen!"
    //         ;
    //         // "connection: close" CRLF
    //         // "content-type: text/html" CRLF
    // }
