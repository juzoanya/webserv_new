/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:57 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 22:20:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


int ChildProcessHandler::parentsock = 0;
int ChildProcessHandler::childsock = 1;


void    ChildProcessHandler::addArgument( std::string const & argument )
{
    this->_cgiArgs.push_back(argument);
}

void    ChildProcessHandler::addEnvVariable( std::string const & envVar )
{
    this->_cgiEnv.push_back(envVar);
}

ChildProcessHandler::ChildProcessHandler( Polling & polling, const void * dataPtr, std::size_t dataSize )
 : APollEventHandler(polling, true), _dataToSendPtr(static_cast<const unsigned char*>(dataPtr)), _dataToSendSize(dataSize)
{ }

ChildProcessHandler::~ChildProcessHandler( void )
{ }

void    ChildProcessHandler::handleEvent( struct pollfd & pollfd )
{
    if (pollfd.revents & POLLHUP) {

    }
    if (pollfd.revents & POLLIN) {
        
    }
    if (pollfd.revents & POLLOUT) {
        int sendBytes = send(pollfd.fd, _dataToSendPtr, _dataToSendSize, 0);
        if (sendBytes == -1) {
            
        } else if (sendBytes == 0) {

        } else {
            _dataToSendPtr += sendBytes;
            _dataToSendSize -= sendBytes;
        }
    }
}

int    ChildProcessHandler::createChildProcess( std::string const & binPath )
{
    int sock_pair[2];
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1)
        return (-1);
    if (fcntl(sock_pair[ChildProcessHandler::childsock], F_SETFL, O_NONBLOCK) == 0
        && fcntl(sock_pair[ChildProcessHandler::parentsock], F_SETFL, O_NONBLOCK) == 0) {
        pid_t pid = fork();
        if (pid == 0) {
            dup2(sock_pair[ChildProcessHandler::childsock], STDOUT_FILENO);
            dup2(sock_pair[ChildProcessHandler::childsock], STDIN_FILENO);
            close(sock_pair[ChildProcessHandler::childsock]);
            close(sock_pair[ChildProcessHandler::parentsock]);
            // if (chdir(docRoot.c_str()) == -1) {
            //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
            //     exit(1);
            // }

            for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
                this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
            this->_enviroment.push_back(NULL);
            this->_cgiArgs.insert(this->_cgiArgs.begin(), binPath);
            for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
                this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
            this->_arguments.push_back(NULL);
            execve(this->_arguments[0], this->_arguments.data(), this->_enviroment.data());
            std::cout << "error: execve: " << strerror(errno) << std::endl;
            close(sock_pair[ChildProcessHandler::childsock]);
            exit(1);
        } else if (pid > 0) {
            close(sock_pair[ChildProcessHandler::childsock]);
            return (sock_pair[ChildProcessHandler::parentsock]);
        }
    }
    close(sock_pair[ChildProcessHandler::childsock]);
    close(sock_pair[ChildProcessHandler::parentsock]);
    return (-1);
}



// int    ChildProcessHandler::createChildProcess( std::string const & binPath )
// {
//     int sock_pair[2];
//     if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
//         std::cout << "error: socketpair: " << strerror(errno) << std::endl;
//         return (-1);
//     }
//     if (fcntl(sock_pair[ChildProcessHandler::childsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[this->childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     }
//     if (fcntl(sock_pair[ChildProcessHandler::parentsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     }
//     pid_t pid = fork();
//     if (pid == 1) {
//         std::cout << "error: fork: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     } else if (pid == 0) {
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         dup2(sock_pair[ChildProcessHandler::childsock], STDOUT_FILENO);
//         dup2(sock_pair[ChildProcessHandler::childsock], STDIN_FILENO);
//         // if (chdir(docRoot.c_str()) == -1) {
//         //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
//         //     exit(1);
//         // }

//         for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//             this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//         this->_enviroment.push_back(NULL);
//         this->_cgiArgs.insert(this->_cgiArgs.begin(), binPath);
//         for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//             this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//         this->_arguments.push_back(NULL);
//         execve(this->_arguments[0], this->_arguments.data(), this->_enviroment.data());
//         std::cout << "error: execve: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         exit(1);
//     } else {
//         close(sock_pair[ChildProcessHandler::childsock]);
//     }
//     return (sock_pair[ChildProcessHandler::parentsock]);
// }









// int ChildProcessHandler::parentsock = 0;
// int ChildProcessHandler::childsock = 1;

// int charToupperUnderscore( int c )
// {
//     if (c == '-')
//         return ('_');
//     return (::toupper(c));
// }


// void    ChildProcessHandler::setScriptInfos( std::string const & scriptName, std::string const & scriptFileName,
//                                         std::string const & pathInfo, std::string const & pathTranslated )
// {
//     this->_cgiEnv.push_back(std::string("SCRIPT_NAME=") + scriptName);
//     this->_cgiEnv.push_back(std::string("SCRIPT_FILE_NAME=") + scriptFileName);
//     this->_cgiEnv.push_back(std::string("PATH_INFO=") + pathInfo);
//     this->_cgiEnv.push_back(std::string("PATH_TRANSLATED=") + pathTranslated);
// }

// void    ChildProcessHandler::setProtocolInfos( std::string const & httpProtocol, std::string const & cgiProtocol )
// {
//     this->_cgiEnv.push_back(std::string("SERVER_PROTOCOL=") + httpProtocol);
//     this->_cgiEnv.push_back(std::string("GATEWAY_INTERFACE=" + cgiProtocol));
// }

// void    ChildProcessHandler::setRequestPathInfos( std::string const & requestQuery, std::string const & requestFullUriEncoded )
// {
//     this->_cgiEnv.push_back(std::string("QUERY_STRING=") + requestQuery);
//     this->_cgiEnv.push_back(std::string("REQUEST_URI=") + requestFullUriEncoded);
// }

// void    ChildProcessHandler::setRequestMethodAndContentInfos( std::string const & method, std::string const & contentType, std::string const & contentLength)
// {
//     this->_cgiEnv.push_back(std::string("REQUEST_METHOD=") + method);
//     this->_cgiEnv.push_back(std::string("CONTENT_TYPE=") + contentType);
//     this->_cgiEnv.push_back(std::string("CONTENT_LENGTH=") + contentLength);
 
// }

// void    ChildProcessHandler::setServerInfos( std::string const & serverIp, std::string const & serverPort, std::string const & serverName, std::string const & serverSoftware )
// {
//     this->_cgiEnv.push_back(std::string("SERVER_ADDR=") + serverIp);
//     this->_cgiEnv.push_back(std::string("SERVER_PORT=") + serverPort);
//     this->_cgiEnv.push_back(std::string("SERVER_NAME=") + serverName);
//     this->_cgiEnv.push_back(std::string("SERVER_SOFTWARE=") + serverSoftware);
// }

// void    ChildProcessHandler::setClientInfos( std::string const & clientIp, std::string const & clientPort )
// {
//     this->_cgiEnv.push_back(std::string("REMOTE_ADDR=") + clientIp);
//     this->_cgiEnv.push_back(std::string("REMOTE_PORT=") + clientPort);
// }


// ChildProcessHandler::ChildProcessHandler( Polling & polling ) : APollEventHandler(polling, true)
// { }

// ChildProcessHandler::~ChildProcessHandler( void )
// { }

// void    ChildProcessHandler::handleEvent( struct pollfd & pollfd )
// {
//     (void)pollfd;
// }

// int    ChildProcessHandler::createChildProcess( void )
// {
//     int sock_pair[2];
//     if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
//         std::cout << "error: socketpair: " << strerror(errno) << std::endl;
//         return (-1);
//     }
//     if (fcntl(sock_pair[ChildProcessHandler::childsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[this->childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     }
//     if (fcntl(sock_pair[ChildProcessHandler::parentsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     }
//     pid_t pid = fork();
//     if (pid == 1) {
//         std::cout << "error: fork: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         return (-1);
//     } else if (pid == 0) {
//         close(sock_pair[ChildProcessHandler::parentsock]);
//         dup2(sock_pair[ChildProcessHandler::childsock], STDOUT_FILENO);
//         dup2(sock_pair[ChildProcessHandler::childsock], STDIN_FILENO);
//         // if (chdir(docRoot.c_str()) == -1) {
//         //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
//         //     exit(1);
//         // }

//         // char *envArr[this->_cgiEnv.size() + 1];
//         // char *argArr[this->_cgiArgs.size() + 1];
        
//         // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//         //     envArr[i] = const_cast<char*>(this->_cgiEnv[i].c_str());
//         // envArr[this->_cgiEnv.size()] = NULL;
//         // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//         //     argArr[i] = const_cast<char*>(this->_cgiArgs[i].c_str());
//         // argArr[this->_cgiArgs.size()] = NULL;
//         // execve(argArr[0], argArr, envArr);

//         execve(this->_arguments[0], this->_arguments.data(), this->_enviroment.data());
//         std::cout << "error: execve: " << strerror(errno) << std::endl;
//         close(sock_pair[ChildProcessHandler::childsock]);
//         exit(1);
//     } else {
//         close(sock_pair[ChildProcessHandler::childsock]);
//     }
//     return (sock_pair[ChildProcessHandler::parentsock]);
// }


// CgiHandler::CgiHandler( Polling & polling, WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, HttpContent & clientResponseBuffer)
//  : APollEventHandler(polling, true), _requestHelper(NULL, false), _clientResponseBuffer(clientResponseBuffer)
// {

//     (void)_clientResponseBuffer;
//     // this->httpContent.setContentExternalBuffer(request.requestBody.bodyBuffer);

//     std::cout << "---- CgiHandler constuctor ----\n";
//     std::cout << "rootPath: " << config.getRootPath() << std::endl;
//     std::cout << "filePath: " << config.getFilePath() << std::endl;
//     this->_cgiArgs.push_back(config.getCgiExecutable());
//     this->_cgiArgs.push_back(config.getFilePath());

//     http_header_map_t::const_iterator headerIt;
//     this->_cgiEnv.push_back(std::string("QUERY_STRING=") + request.query);
//     this->_cgiEnv.push_back(std::string("REQUEST_METHOD=") + request.method);
//     headerIt = request.headerMap.find("content-type");
//     if (headerIt != request.headerMap.end())
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE=") + headerIt->second);
//     else
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE="));
//     this->_cgiEnv.push_back(std::string("CONTENT_LENGTH=") + toStr(request.requestBody.bodyBuffer.size()));
//     this->_cgiEnv.push_back(std::string("SCRIPT_FILE_NAME=") + config.getFilePath());
//     this->_cgiEnv.push_back(std::string("SCRIPT_NAME=") + request.pathDecoded);
//     this->_cgiEnv.push_back(std::string("PATH_INFO="));
//     this->_cgiEnv.push_back(std::string("PATH_TRANSLATED=") + config.getRootPath());
//     this->_cgiEnv.push_back(std::string("REQUEST_URI=") + request.fullUri);
//     this->_cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
//     this->_cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
//     this->_cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
//     this->_cgiEnv.push_back(std::string("REMOTE_ADDR=") + clientIpPort.getIpStr());
//     this->_cgiEnv.push_back(std::string("REMOTE_PORT=") + clientIpPort.getPortStr());
//     this->_cgiEnv.push_back(std::string("SERVER_ADDR=") + serverIpPort.getIpStr());
//     this->_cgiEnv.push_back(std::string("SERVER_PORT=") + serverIpPort.getPortStr());
//     this->_cgiEnv.push_back(std::string("SERVER_NAME=") + config.getServerName());
//     this->_cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));

//     for (http_header_map_t::const_iterator it = request.headerMap.begin(); it != request.headerMap.end(); ++it) {
//         std::string cgiHeader = "HTTP_";
//         std::transform(it->first.begin(), it->first.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
//         this->_cgiEnv.push_back(cgiHeader + "=" + it->second);
//     }

//     for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i) {
//         this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//         std::cout << this->_arguments.back() << std::endl;
//     }
//     this->_arguments.push_back(NULL);
//     for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i) {
//         this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//         std::cout << this->_enviroment.back() << std::endl;
//     }
//     this->_enviroment.push_back(NULL);

//     // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//     //     this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//     // this->_enviroment.push_back(NULL);
//     // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//     //     this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//     // this->_arguments.push_back(NULL);
// }





// int CgiHandler::parentsock = 0;
// int CgiHandler::childsock = 1;

// int charToupperUnderscore( int c )
// {
//     if (c == '-')
//         return ('_');
//     return (::toupper(c));
// }


// CgiHandler::CgiHandler( Polling & polling, WsIpPort const & clientIpPort, WsIpPort const & serverIpPort, HttpConfig const & config, HttpRequest const & request, HttpContent & clientResponseBuffer)
//  : APollEventHandler(polling, true), _requestHelper(NULL, false), _clientResponseBuffer(clientResponseBuffer)
// {

//     (void)_clientResponseBuffer;
//     // this->httpContent.setContentExternalBuffer(request.requestBody.bodyBuffer);

//     std::cout << "---- CgiHandler constuctor ----\n";
//     std::cout << "rootPath: " << config.getRootPath() << std::endl;
//     std::cout << "filePath: " << config.getFilePath() << std::endl;
//     this->_cgiArgs.push_back(config.getCgiExecutable());
//     this->_cgiArgs.push_back(config.getFilePath());

//     http_header_map_t::const_iterator headerIt;
//     this->_cgiEnv.push_back(std::string("QUERY_STRING=") + request.query);
//     this->_cgiEnv.push_back(std::string("REQUEST_METHOD=") + request.method);
//     headerIt = request.headerMap.find("content-type");
//     if (headerIt != request.headerMap.end())
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE=") + headerIt->second);
//     else
//         this->_cgiEnv.push_back(std::string("CONTENT_TYPE="));
//     this->_cgiEnv.push_back(std::string("CONTENT_LENGTH=") + toStr(request.requestBody.bodyBuffer.size()));
//     this->_cgiEnv.push_back(std::string("SCRIPT_FILE_NAME=") + config.getFilePath());
//     this->_cgiEnv.push_back(std::string("SCRIPT_NAME=") + request.pathDecoded);
//     this->_cgiEnv.push_back(std::string("PATH_INFO="));
//     this->_cgiEnv.push_back(std::string("PATH_TRANSLATED=") + config.getRootPath());
//     this->_cgiEnv.push_back(std::string("REQUEST_URI=") + request.fullUri);
//     this->_cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
//     this->_cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
//     this->_cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
//     this->_cgiEnv.push_back(std::string("REMOTE_ADDR=") + clientIpPort.getIpStr());
//     this->_cgiEnv.push_back(std::string("REMOTE_PORT=") + clientIpPort.getPortStr());
//     this->_cgiEnv.push_back(std::string("SERVER_ADDR=") + serverIpPort.getIpStr());
//     this->_cgiEnv.push_back(std::string("SERVER_PORT=") + serverIpPort.getPortStr());
//     this->_cgiEnv.push_back(std::string("SERVER_NAME=") + config.getServerName());
//     this->_cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));

//     for (http_header_map_t::const_iterator it = request.headerMap.begin(); it != request.headerMap.end(); ++it) {
//         std::string cgiHeader = "HTTP_";
//         std::transform(it->first.begin(), it->first.end(), std::back_inserter(cgiHeader), charToupperUnderscore);
//         this->_cgiEnv.push_back(cgiHeader + "=" + it->second);
//     }

//     for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i) {
//         this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//         std::cout << this->_arguments.back() << std::endl;
//     }
//     this->_arguments.push_back(NULL);
//     for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i) {
//         this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//         std::cout << this->_enviroment.back() << std::endl;
//     }
//     this->_enviroment.push_back(NULL);

//     // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//     //     this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
//     // this->_enviroment.push_back(NULL);
//     // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//     //     this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
//     // this->_arguments.push_back(NULL);
// }

// CgiHandler::~CgiHandler( void )
// { }

// void    CgiHandler::handleEvent( struct pollfd & pollfd )
// {
//     (void)pollfd;
// }

// int    CgiHandler::createChildProcess( void )
// {
//     int sock_pair[2];
//     if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
//         std::cout << "error: socketpair: " << strerror(errno) << std::endl;
//         return (-1);
//     }
//     if (fcntl(sock_pair[CgiHandler::childsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[this->childsock]);
//         close(sock_pair[CgiHandler::parentsock]);
//         return (-1);
//     }
//     if (fcntl(sock_pair[CgiHandler::parentsock], F_SETFL, O_NONBLOCK) == -1) {
//         std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
//         close(sock_pair[CgiHandler::childsock]);
//         close(sock_pair[CgiHandler::parentsock]);
//         return (-1);
//     }
//     pid_t pid = fork();
//     if (pid == 1) {
//         std::cout << "error: fork: " << strerror(errno) << std::endl;
//         close(sock_pair[CgiHandler::childsock]);
//         close(sock_pair[CgiHandler::parentsock]);
//         return (-1);
//     } else if (pid == 0) {
//         close(sock_pair[CgiHandler::parentsock]);
//         dup2(sock_pair[CgiHandler::childsock], STDOUT_FILENO);
//         dup2(sock_pair[CgiHandler::childsock], STDIN_FILENO);
//         // if (chdir(docRoot.c_str()) == -1) {
//         //     std::cerr << "Error chdir: " << strerror(errno) << std::endl;
//         //     exit(1);
//         // }

//         // char *envArr[this->_cgiEnv.size() + 1];
//         // char *argArr[this->_cgiArgs.size() + 1];
        
//         // for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
//         //     envArr[i] = const_cast<char*>(this->_cgiEnv[i].c_str());
//         // envArr[this->_cgiEnv.size()] = NULL;
//         // for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
//         //     argArr[i] = const_cast<char*>(this->_cgiArgs[i].c_str());
//         // argArr[this->_cgiArgs.size()] = NULL;
//         // execve(argArr[0], argArr, envArr);

//         execve(this->_arguments[0], this->_arguments.data(), this->_enviroment.data());
//         std::cout << "error: execve: " << strerror(errno) << std::endl;
//         close(sock_pair[CgiHandler::childsock]);
//         exit(1);
//     } else {
//         close(sock_pair[CgiHandler::childsock]);
//     }
//     return (sock_pair[CgiHandler::parentsock]);
// }
