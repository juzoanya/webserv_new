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
#include "FileInfo.hpp"
#include "httpfunc.hpp"
#include "ClientData.hpp"

#define BUFFERSIZE 4096

int create_socket(std::string const & hostname, std::string const & port, int backlog)
{
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int gaierr = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res);
    if (gaierr != 0 || !res) {
        std::cout << "error: getaddrinfo: " << gai_strerror(gaierr) << std::endl;
        return (-1);
    }
    memcpy(&hints, res, sizeof(struct addrinfo));
    freeaddrinfo(res);
    int socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketfd == -1) {
        std::cout << "error: socket: " << strerror(errno) << std::endl;
        return (-1);
    }
    if (bind(socketfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cout << "error: bind: " << strerror(errno) << std::endl;
        close(socketfd);
        return (-1);
    }
    if (listen(socketfd, backlog) == -1) {
        std::cout << "error: listen: " << strerror(errno) << std::endl;
        close(socketfd);
        return (-1);
    }
    if (fcntl(socketfd, F_SETFL, O_NONBLOCK) == -1) {
        std::cout << "error: fcntl: " << strerror(errno) << std::endl;
        close(socketfd);
        return (-1);
    }
    return (socketfd);
}

int newClientConnection(int serversocket, int kq, std::map<int, ClientData>& client_data)
{
    (void)client_data;
    struct sockaddr addr;
    socklen_t   addrlen = sizeof(addr);
    int clientsocket = accept(serversocket, &addr, &addrlen);
    if (clientsocket == -1) {
        std::cout << "error: accept: " << strerror(errno) << std::endl;
    } else {
        if (fcntl(clientsocket, F_SETFL, O_NONBLOCK) == -1) {
            std::cout << "error: fcntl: " << strerror(errno) << std::endl;
            close(clientsocket);
            return (-1);
        }
        #ifdef __APPLE__
        int set = 1;
        if (setsockopt(clientsocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) == -1) {
            std::cout << "error: setsockopt: " << strerror(errno) << std::endl;
            close(clientsocket);
            return (-1);
        }
        #endif
        struct kevent kev[2];
        EV_SET(kev, clientsocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        EV_SET(kev + 1, clientsocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(kq, kev, 2, NULL, 0, NULL) == -1) {
            std::cout << "error: kevent: " << strerror(errno) << std::endl;
            close(clientsocket);
            return (-1);
        }
    }
    return (clientsocket);
}

int createServers(int kq, ConfigParser& config)
{
  for (int i = 0; i != config.getServerCount(); ++i) {
        ws_config_t& serverconfig = config.serverConfigs[i].serverConfig;
        ws_config_t::iterator it = serverconfig.find("listen");
        if (it == serverconfig.end() || it->second.size() == 0) {
            std::cout << "no listening host:port for this server" << std::endl;
            continue ;
        }
        int serversocket = 0;
        if (it->second.size() == 1) {
            std::cout << "create server:socket: " << serversocket << " | 0.0.0.0:" << it->second[0] << std::endl;
            serversocket = create_socket("0.0.0.0", it->second[0], 10);
        } else {
            serversocket = create_socket(it->second[0], it->second[1], 10);
            std::cout << "create server: socket: " << serversocket << " | " << it->second[0] << it->second[1] << std::endl;
        }
        if (serversocket != -1) {
            struct kevent kev;
            EV_SET(&kev, serversocket, EVFILT_READ, EV_ADD, 0, 0, &config.serverConfigs[i]);
            if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
                std::cout << "error: kevent: " << strerror(errno) << std::endl;
                close(serversocket);
            } 
        }
    }
    return (0);
}

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

int readClientData(struct kevent& currKev, buff_t & read_data, std::map<int, ClientData>& client_data)
{
    int sizeread = recv(currKev.ident, read_data.data(), 4096, 0);
    if (sizeread == -1) {
        std::cout << "error: recv: " << strerror(errno) << std::endl;
        close(currKev.ident);
        client_data.erase(currKev.ident);
    } else if (sizeread == 0) {
        std::cout << "the client closed his write side, no more data to read" << std::endl;
        close(currKev.ident);
        client_data.erase(currKev.ident);
        // fd_data[currKev.ident] = &currKev.ident;
    } else {
        std::vector<char>::iterator readit = std::search(read_data.begin(), read_data.begin() + sizeread,
            ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
        if (readit != read_data.begin() + sizeread)
            client_data[currKev.ident] = &currKev.ident;
        // handleClientData(client_data.at(currKev.ident), read_data.begin(), read_data.end());
        // client_data.at(currKev.ident).request.parseRequest(read_data.begin(), read_data.end());
    }
}

int writeClientData(struct kevent& currKev, std::map<int, ClientData>& client_data)
{
    ClientData& data = client_data.at(currKev.ident);
    if (client_data[currKev.ident]) {
        std::string response =
            "HTTP/1.1 200 OK" CRLF
            "content-type: text/plain" CRLF
            "content-legth: 13" CRLF CRLF
            "Guten Morgen!"
            ;
            // "connection: close" CRLF
            // "content-type: text/html" CRLF
    #ifdef __linux__
        int sizewritten = send(sockfd, req->buffPtr, req->remain, MSG_NOSIGNAL);
    #elif __APPLE__
        int sizewritten = send(currKev.ident, response.data(), response.size(), 0);
    #endif
        if (sizewritten == -1) {
            std::cout << "error: recv: " << strerror(errno) << std::endl;
            close(currKev.ident);
            client_data.erase(currKev.ident);
        } else if (sizewritten == 0) {
            std::cout << "the client closed his write side, no more data to read" << std::endl;
        } 
    }
}

int handle_event(struct kevent& currKev, buff_t& read_data, std::map<int, ClientData>& client_data)
{

}

int event_loop(ConfigParser& config)
{
    int kq = kqueue();
    if (kq == -1) {
        std::cout << "error: kqueue: " << strerror(errno) << std::endl;
        return (-1);
    }

    std::map<int, ClientData>   client_data;
    std::vector<int>        server_sockets;
    struct kevent           kevlist[100];
    buff_t                  read_data(4096);
    createServers(kq, config);
    while (true) {
        int nevents = kevent(kq, NULL, 0, kevlist, 100, NULL);
        for (int i = 0; i != nevents; ++i) {
            struct kevent& currKev = kevlist[i];
            if (currKev.filter == EVFILT_READ && currKev.udata) {
                int clientsocket = newClientConnection(currKev.ident, kq, client_data);
                std::cout << "new client connection on: " << currKev.ident << " | new clientSocket: " << clientsocket << std::endl;
                ConfigParser::ServerContext* serverConfig = static_cast<ConfigParser::ServerContext*>(currKev.udata);
                if (clientsocket != -1)
                    client_data.insert(std::make_pair(clientsocket, ClientData(*serverConfig)));
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
