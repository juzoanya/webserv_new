#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>

#define WS_PARENTSOCKET 0
#define WS_CHILDSOCKET 1

std::string getCurrDir()
{
    char buff[2000];
    char* dir = getcwd(buff, 2000);
    return (std::string(dir));
}

int main( int argc, char** argv, char**envp )
{
    if (argc != 2)
        return (1);
    std::string cgiBinPath, pathDecoded;
    if (std::string(argv[1]) == "py") {
        cgiBinPath = "/Users/maximilian/goinfre/.brew/bin/python3";
        pathDecoded = "/py-cgi/printCGIvariables.py";
    } else if (std::string(argv[1]) == "php") {
        cgiBinPath = "/Users/maximilian/goinfre/.brew/bin/php-cgi";
        pathDecoded = "/php-cgi/printCGIvariables.php";
    } else {
        return (1);
    }
    std::string docRoot = "/Users/maximilian/coding/c/projects/42coding/62_webserv/62_03_webserv_group_small/www/webserv.bla";
    std::string filePath = docRoot + pathDecoded;

    std::vector<char*>  arguments;
    std::vector<char*>  enviroment;

    std::vector<std::string>    cgiArgs;
    cgiArgs.push_back(cgiBinPath);
    cgiArgs.push_back(filePath);
    for (std::size_t i = 0; i != cgiArgs.size(); ++i)
        arguments.push_back(const_cast<char*>(cgiArgs[i].c_str()));
    arguments.push_back(NULL);

 for (std::size_t i = 0; envp[i]; ++i)
        enviroment.push_back(envp[i]);

    std::vector<std::string>    cgiEnv;

    cgiEnv.push_back(std::string("PHPRC=") + "/Users/maximilian/goinfre/.brew/etc/php/8.2/php.ini");
    cgiEnv.push_back(std::string("QUERY_STRING=") + "");
    cgiEnv.push_back(std::string("REQUEST_METHOD=") + "GET");
    cgiEnv.push_back(std::string("CONTENT_TYPE="));
    cgiEnv.push_back(std::string("CONTENT_LENGTH=") + "0");
    cgiEnv.push_back(std::string("SCRIPT_FILENAME=") + filePath);
    cgiEnv.push_back(std::string("SCRIPT_NAME=") + pathDecoded);
    cgiEnv.push_back(std::string("PATH_INFO="));
    cgiEnv.push_back(std::string("PATH_TRANSLATED=") + docRoot);
    cgiEnv.push_back(std::string("REQUEST_URI=") + "");
    cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
    cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
    cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
    cgiEnv.push_back(std::string("REMOTE_ADDR=") + "127.0.0.1");
    cgiEnv.push_back(std::string("REMOTE_PORT=") + "80");
    cgiEnv.push_back(std::string("SERVER_ADDR=") + "127.0.0.1");
    cgiEnv.push_back(std::string("SERVER_PORT=") + "53435");
    cgiEnv.push_back(std::string("SERVER_NAME=") + "localhost");
    cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));

    for (std::size_t i = 0; i != cgiEnv.size(); ++i)
        enviroment.push_back(const_cast<char*>(cgiEnv[i].c_str()));
    enviroment.push_back(NULL);

    // std::cout << "cgi bin path: " << cgiBinPath << std::endl;
    // std::cout << "cgi arguments: \n";
    // for (std::size_t i = 0; arguments[i]; ++i)
    //     std::cout << arguments[i] << std::endl;
    // std::cout << "\n";
    // std::cout << "cgi enviroment: \n";
    // for (std::size_t i = 0; enviroment[i]; ++i)
    //     std::cout << enviroment[i] << std::endl;
    // std::cout << "\n";

    std::cout << "change directory to: " << docRoot << std::endl;
    std::cout << "execute script.\n";
    // pid_t newPid = fork();
    // if (newPid == -1) {
    //     std::cerr << "Error fork: " << strerror(errno) << std::endl;
    // } else if (newPid == 0) {
        
    //     std::cout << "before chdir: current Dir: " << getCurrDir() << std::endl;
    //     if (chdir(docRoot.c_str()) == -1) {
    //         std::cerr << "Error chdir: " << strerror(errno) << std::endl;
    //         exit(1);
    //     }
    //     std::cout << "after chdir: current Dir: " << getCurrDir() << std::endl;
    //     // execve(cgiBinPath.c_str(), arguments.data(), NULL);
    //     execve(cgiBinPath.c_str(), arguments.data(), enviroment.data());
    //     std::cerr << "error execve: " << strerror(errno) << std::endl;
    // } else {
    //     std::cout << "parent after Fork" << std::endl;
    // }
    // wait(NULL);
    // return (0);

    int sock_pair[2];
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1) {
        std::cout << "error: socketpair: " << strerror(errno) << std::endl;
        return (-1);
    }
    // if (fcntl(sock_pair[WS_CHILDSOCKET], F_SETFL, O_NONBLOCK) == -1) {
    //     std::cout << "error: setting childSocket non blocking: " << strerror(errno) << std::endl;
    //     close(sock_pair[WS_CHILDSOCKET]);
    //     close(sock_pair[WS_PARENTSOCKET]);
    //     return (-1);
    // }
    // if (fcntl(sock_pair[WS_PARENTSOCKET], F_SETFL, O_NONBLOCK) == -1) {
    //     std::cout << "error: setting parentSocket non blocking: " << strerror(errno) << std::endl;
    //     close(sock_pair[WS_CHILDSOCKET]);
    //     close(sock_pair[WS_PARENTSOCKET]);
    //     return (-1);
    // }
    pid_t pid = fork();
    if (pid == 1) {
        std::cout << "error: fork: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        close(sock_pair[WS_PARENTSOCKET]);
        return (-1);
    } else if (pid == 0) {
        close(sock_pair[WS_PARENTSOCKET]);
        dup2(sock_pair[WS_CHILDSOCKET], STDOUT_FILENO);
        dup2(sock_pair[WS_CHILDSOCKET], STDIN_FILENO);
        if (chdir(docRoot.c_str()) == -1) {
            std::cerr << "Error chdir: " << strerror(errno) << std::endl;
            exit(1);
        }
        execve(cgiBinPath.c_str(), arguments.data(), enviroment.data());
        std::cout << "error: execve: " << strerror(errno) << std::endl;
        close(sock_pair[WS_CHILDSOCKET]);
        exit(1);
    } else {
        close(sock_pair[WS_CHILDSOCKET]);
        char buffer[8000];
        // while (true) {
            int readsize = recv(sock_pair[WS_PARENTSOCKET], buffer, 8000, 0);
            // if (readsize == -1) {
            //     std::cout << "error: " << strerror(errno) << std::endl;
            //     break ;
            // } else if (readsize == 0) {
            //     std::cout << "done - readsize 0" << std::endl;
            //     break ;
            // }
            buffer[readsize] = 0;
            std::cout << "byte read: " << readsize << std::endl;
            std::cout << "data read: " << buffer << std::endl;
        // }
        wait(NULL);
    }
    return (0);
}





// int main( int argc, char** argv, char**envp )
// {
//     if (argc != 2)
//         return (1);
//     std::string cgiBinPath, pathDecoded;
//     if (std::string(argv[1]) == "py") {
//         cgiBinPath = "/Users/maximilian/goinfre/.brew/bin/python3";
//         pathDecoded = "/py-cgi/printCGIvariables.py";
//     } else if (std::string(argv[1]) == "php") {
//         cgiBinPath = "/Users/maximilian/goinfre/.brew/bin/php-cgi";
//         pathDecoded = "/php-cgi/printCGIvariables.php";
//     } else {
//         return (1);
//     }
//     std::string docRoot = "/Users/maximilian/coding/c/projects/42coding/62_webserv/62_03_webserv_group_small/www/webserv.bla";
//     std::string filePath = docRoot + pathDecoded;

//     std::vector<char*>  arguments;
//     std::vector<char*>  enviroment;

//     std::vector<std::string>    cgiArgs;
//     cgiArgs.push_back(cgiBinPath);
//     // cgiArgs.push_back("php-cgi");
//     // cgiArgs.push_back("-C");
//     // cgiArgs.push_back("-f");
//     cgiArgs.push_back(filePath);
//     for (std::size_t i = 0; i != cgiArgs.size(); ++i)
//         arguments.push_back(const_cast<char*>(cgiArgs[i].c_str()));
//     arguments.push_back(NULL);

//     std::vector<std::string>    cgiEnv;

//     cgiEnv.push_back(std::string("QUERY_STRING=") + "");
//     cgiEnv.push_back(std::string("REQUEST_METHOD=") + "GET");
//     cgiEnv.push_back(std::string("CONTENT_TYPE="));
//     cgiEnv.push_back(std::string("CONTENT_LENGTH=") + "0");
//     cgiEnv.push_back(std::string("SCRIPT_FILENAME=") + filePath);
//     cgiEnv.push_back(std::string("SCRIPT_NAME=") + pathDecoded);
//     cgiEnv.push_back(std::string("PATH_INFO="));
//     cgiEnv.push_back(std::string("PATH_TRANSLATED=") + docRoot);
//     cgiEnv.push_back(std::string("REQUEST_URI=") + "");
//     cgiEnv.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
//     cgiEnv.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
//     cgiEnv.push_back(std::string("SERVER_SOFTWARE=webserv/0.0.0"));
//     cgiEnv.push_back(std::string("REMOTE_ADDR=") + "127.0.0.1");
//     cgiEnv.push_back(std::string("REMOTE_PORT=") + "80");
//     cgiEnv.push_back(std::string("SERVER_ADDR=") + "127.0.0.1");
//     cgiEnv.push_back(std::string("SERVER_PORT=") + "53435");
//     cgiEnv.push_back(std::string("SERVER_NAME=") + "localhost");
//     cgiEnv.push_back(std::string("REDIRECT_STATUS=200"));

//     for (std::size_t i = 0; envp[i]; ++i)
//         enviroment.push_back(envp[i]);

//     for (std::size_t i = 0; i != cgiEnv.size(); ++i)
//         enviroment.push_back(const_cast<char*>(cgiEnv[i].c_str()));
//     enviroment.push_back(NULL);

//     std::cout << "cgi bin path: " << cgiBinPath << std::endl;
//     std::cout << "cgi arguments: \n";
//     for (std::size_t i = 0; arguments[i]; ++i)
//         std::cout << arguments[i] << std::endl;
//     std::cout << "\n";
//     std::cout << "cgi enviroment: \n";
//     for (std::size_t i = 0; enviroment[i]; ++i)
//         std::cout << enviroment[i] << std::endl;
//     std::cout << "\n";

//     std::cout << "change directory to: " << docRoot << std::endl;
//     std::cout << "execute script.\n";
//     chdir(docRoot.c_str());
//     execve(cgiBinPath.c_str(), arguments.data(), enviroment.data());
//     std::cout << "error execve: " << strerror(errno) << std::endl;

//     return (0);
// }
