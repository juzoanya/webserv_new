/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/24 15:53:02 by mberline          #+#    #+#             */
/*   Updated: 2024/01/07 10:28:11 by mberline         ###   ########.fr       */
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
#include "ConfigHandler.hpp"
#include "HttpClient.hpp"

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

// int createServerSocket(std::string const & host, std::string const & port, int backlog)
// {
//     struct addrinfo hints;
//     struct addrinfo *res = NULL;
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_protocol = IPPROTO_TCP;
//     int gaierr = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
//     if (gaierr != 0)
//         throw std::runtime_error(gai_strerror(gaierr));
//     memcpy(&hints, res, sizeof(struct addrinfo));
//     freeaddrinfo(res);
//     int serversocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
//     if (serversocket == -1)
//         throw std::runtime_error(strerror(errno));
//     int err = bind(serversocket, res->ai_addr, res->ai_addrlen);
//     if (err == 0)
//         err = listen(serversocket, backlog);
//     if (err == 0)
//         err = fcntl(serversocket, F_SETFL, O_NONBLOCK);
//     if (err != 0) {
//         close(serversocket);
//         throw std::runtime_error(strerror(errno));
//     }
//     this->_sockfd = serversocket;
//     setIpPort(*reinterpret_cast<struct sockaddr_in*>(&hints.ai_addr), this->_ipString, this->_portString);
// }

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


// std::string getIpPort(struct sockaddr_in const & addr)
// {
//     uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
//     std::stringstream ss;
//     ss << ((ip_addr >> 24) & 0xFF) << ".";
//     ss << ((ip_addr >> 16) & 0xFF) << ".";
//     ss << ((ip_addr >> 8) & 0xFF) << ".";
//     ss << (ip_addr & 0xFF);
//     ss << ":";
//     ss << htons(addr.sin_port);
//     return (ss.str());
// }

// int createServerSocket(std::string const & host, std::string const & port, int backlog, int kq, ws_config_t& serverConfig)
// {
//     struct addrinfo hints;
//     struct addrinfo *res = NULL;
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_family = AF_INET;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_protocol = IPPROTO_TCP;
//     int err = 0, serversocket = -1;
//     if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
//         return (-1);
//     std::vector<std::string>    configValues(1);
//     configValues[0] = getIpPort(*reinterpret_cast<struct sockaddr_in*>(&res->ai_addr));
//     serversocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
//     if (err == 0 && serversocket != -1)
//         err = bind(serversocket, res->ai_addr, res->ai_addrlen);
//     if (err == 0 && serversocket != -1)
//         err = listen(serversocket, backlog);
//     if (err == 0 && serversocket != -1)
//         err = fcntl(serversocket, F_SETFL, O_NONBLOCK);
//     if (err == 0 && serversocket != -1) {
//         struct kevent   kev;
//         EV_SET(&kev, serversocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
//         err = kevent(kq, &kev, 1, NULL, 0, NULL);
//     }
//     if (err == 1 || serversocket == -1) {
//         configValues[0] = "";
//         close(serversocket);
//         serversocket = -1;
//     }
//     freeaddrinfo(res);
//     serverConfig["listen"] = configValues;
//     return (serversocket);
// }

// void    createServers(int kq, ConfigParser& config, std::map<int, HttpConfiguration>& serverData)
// {
//     for (int i = 0; i != config.getServerCount(); ++i) {
//         ws_config_t& serverconfig = config.serverConfigs[i].serverConfig;
//         ws_config_t::iterator it = serverconfig.find("listen");
//         std::cout << "create server: " << i << ": ";
//         std::string host, port;
//         std::size_t pos = it->second[0].find(':');
//         if (it == serverconfig.end() || it->second.size() == 0) {
//             host = "0.0.0.0";
//             port = "8080";
//         } else if (pos == std::string::npos) {
//             host = "0.0.0.0";
//             port = it->second[0];
//         } else {
//             host = it->second[0].substr(0, pos);
//             port = it->second[0].substr(pos + 1, std::string::npos);
//         }

     
//     }
// }

// std::string getIpPort(struct sockaddr_in const & addr)
// {
//     uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
//     std::stringstream ss;
//     ss << ((ip_addr >> 24) & 0xFF) << ".";
//     ss << ((ip_addr >> 16) & 0xFF) << ".";
//     ss << ((ip_addr >> 8) & 0xFF) << ".";
//     ss << (ip_addr & 0xFF);
//     ss << ":";
//     ss << htons(addr.sin_port);
//     return (ss.str());
// }

std::string getPortStr(struct sockaddr_in const & addr)
{
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    std::stringstream ss;
    ss << htons(addr.sin_port);
    return (ss.str());
}

std::string getIpStr(struct sockaddr_in const & addr)
{
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    std::stringstream ss;
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
    return (ss.str());
}

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

void    checkHttpConfig(int kq, std::string const & host, std::string const & port, ConfigParser::ServerContext* currConfig, std::map<int, ConfigHandler>& serverData)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
        return ;
    std::string ipStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&res->ai_addr));
    std::string portStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&res->ai_addr));
    std::map<int, ConfigHandler>::iterator serverDataIt;
    for (serverDataIt = serverData.begin(); serverDataIt != serverData.end(); ++serverDataIt) {
        if (serverDataIt->second.serverIp == ipStr && serverDataIt->second.serverPort == portStr) {
            serverDataIt->second.addServerConfig(currConfig);
            freeaddrinfo(res);
            return ;
        }
    }
    int serversocket = createServerSocket(res, 10, kq);
    if (serversocket != -1) {
        serverData.insert(std::make_pair(serversocket, ConfigHandler(currConfig, ipStr, portStr)));
    }
    freeaddrinfo(res);
}

void    createServers(int kq, ConfigParser& config, std::map<int, ConfigHandler>& serverData)
{
    for (int i = 0; i != config.getServerCount(); ++i) {
        ws_config_t& serverconfig = config.serverConfigs[i].serverConfig;
        ws_config_t::iterator it = serverconfig.find("listen");
        std::cout << "create server: " << i << ": ";
        std::string host, port;
        std::size_t pos = it->second[0].find(':');
        if (it == serverconfig.end() || it->second.size() == 0) {
            host = "0.0.0.0";
            port = "8080";
        } else if (pos == std::string::npos) {
            host = "0.0.0.0";
            port = it->second[0];
        } else {
            host = it->second[0].substr(0, pos);
            port = it->second[0].substr(pos + 1, std::string::npos);
        }
        checkHttpConfig(kq, host, port, &config.serverConfigs[i], serverData);
    }
    if (serverData.size() == 0)
        checkHttpConfig(kq, "0.0.0.0", "8080", NULL, serverData);
}

void    newClientConnection(int kq, int serversocket, ConfigHandler& httpConfig, std::map<int, HttpClient>& clientData)
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
    std::string ipStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
    std::string portStr = getIpStr(*reinterpret_cast<struct sockaddr_in*>(&addr));
    clientData.insert(std::make_pair(clientsocket, HttpClient(httpConfig, ipStr, portStr)));
}

void    handleClient(struct kevent& kev, std::map<int, HttpClient> clientData, buff_t& readData)
{
    std::map<int, HttpClient>::iterator clientDataIt = clientData.find(kev.ident);
    if (clientDataIt == clientData.end())
        return ;
    if (kev.filter == EVFILT_READ) {
        int sizeread = recv(kev.ident, readData.data(), 4096, 0);
        if (sizeread == -1) {
            std::cout << "error: recv: " << strerror(errno) << std::endl;
            close(kev.ident);
            clientData.erase(clientDataIt);
        } else if (sizeread == 0) {
            std::cout << "the client closed his write side, no more data to read" << std::endl;
        } else {
            clientDataIt->second.handleData(readData.begin(), readData.begin() + sizeread);
        }
    } else if (kev.filter == EVFILT_WRITE) {
        Content*    currCont = clientDataIt->second.getHttpContent();
    #ifdef __linux__
        int sizewritten = send(kev.ident, currCont->data, currCont->size, MSG_NOSIGNAL);
    #elif __APPLE__
        int sizewritten = send(kev.ident, currCont->data, currCont->size, 0);
    #endif
        if (sizewritten == -1) {
            std::cout << "error: recv: " << strerror(errno) << std::endl;
            close(kev.ident);
            clientData.erase(clientDataIt);
        } else {
            currCont->data += sizewritten;
            currCont->size -= sizewritten;
        } 
    }
}

int	main(int argc, char** argv)
{
    try {
        if (argc != 2)
            throw std::runtime_error("invalid number of arguments");
	    ConfigParser config;
	    config.configParser(argv[1]);
        int kq = kqueue();
        if (kq == -1)
            throw std::runtime_error(strerror(errno));
        std::map<int, ConfigHandler>        serverData;
        std::map<int, HttpClient>           clientData;
        struct kevent                       kevlist[100];
        buff_t                              read_data(4096);
        createServers(kq, config, serverData);
        while (true) {
            int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
            for (int i = 0; i != nevents; ++i) {
                struct kevent& currKev = kevlist[i];
                std::map<int, ConfigHandler>::iterator serverDataIt = serverData.find(currKev.ident);
                if (currKev.filter == EVFILT_READ && serverDataIt != serverData.end()) {
                    newClientConnection(kq, currKev.ident, serverDataIt->second, clientData);
                } else {
                    handleClient(currKev, clientData, read_data);
                }
            }
        }
    } catch(const std::exception& e) {
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
//     std::map<int, HttpConfiguration>    serverData;
//     std::map<int, HttpClient>           clientData;
//     struct kevent                   kevlist[100];
//     buff_t                          read_data(4096);
//     createServers(kq, config, serverData);
//     if (serverData.size() == 0)
//         return (-1);
//     while (true) {
//         int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
//         for (int i = 0; i != nevents; ++i) {
//             struct kevent& currKev = kevlist[i];
//             std::map<int, HttpConfiguration>::iterator serverDataIt = serverData.find(currKev.ident);
//             if (currKev.filter == EVFILT_READ && serverDataIt != serverData.end()) {
//                 newClientConnection(kq, currKev.ident, serverDataIt->second, clientData);
//                 continue;
//             }
//             std::map<int, HttpClient>::iterator clientDataIt = clientData.find(currKev.ident);
//             if (currKev.filter == EVFILT_READ && clientDataIt != clientData.end()) {
//                 // readClientData(currKev, read_data, client_data);
//             } else if (currKev.filter == EVFILT_WRITE && clientDataIt != clientData.end()) {
//                 Content*    currCont = client.getHttpContent();
//                 if (!currCont)
//                     return ;
//             #ifdef __linux__
//                 int sizewritten = send(clientsocket, currCont->data, currCont->size, MSG_NOSIGNAL);
//             #elif __APPLE__
//                 int sizewritten = send(clientsocket, currCont->data, currCont->size, 0);
//             #endif
//                 if (sizewritten == -1) {
//                     std::cout << "error: recv: " << strerror(errno) << std::endl;
//                     close(clientsocket);
//                     client_data.erase(currKev.ident);
//                 } else {
//                     currCont->data += sizewritten;
//                     currCont->size -= sizewritten;
//                     std::cout << "the client closed his write side, no more data to read" << std::endl;
//                 } 
//             }

//         }
//     }
// }


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
