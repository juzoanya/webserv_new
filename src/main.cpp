


#include "headers.hpp"

// void	sigchild_handler(int sig)
// {
// 	if (sig != SIGCHLD)
// 		return ;
// 	int status;
// 	// write(1, "SIGCHILD CAUGHT\n", 17);
// 	int ret = waitpid(-1, &status, WNOHANG);
// 	if (ret == 0) {
// 		write(1, "no childprocesses to terminate\n", 32);
// 	} else if (ret == -1) {
// 		// write(1, "error waitpid: ", 16);
// 		write(1, strerror(errno), 20);
// 		// write(1, "\n", 1);
// 	// } else {
// 	// 	write(1, "CHILD TERMINATED\n", 18);
// 	}
// 	if (WIFEXITED(status)) {
// 		if (WEXITSTATUS(status) == -1)
// 			write(1, strerror(errno), 20);
// 	} else if (WIFSIGNALED(status)) {
// 		write(1, strerror(errno), 20);
// 	} else if (WIFSTOPPED(status)) {
// 		write(1, strerror(errno), 20);
// 	}
// }

void    sigchild_handler(int sig)
{
    if (sig != SIGCHLD)
        return ;
    int status;

    while (true) {
        int childpid = waitpid(-1, &status, WNOHANG);
		if (childpid == 0)
			break;
        if (childpid == -1) {
            if (errno != ECHILD) {
                const char* errnoname = strerror(errno);
                while (errnoname)
                    write(STDERR_FILENO, errnoname++, 1);
                write(STDERR_FILENO, "\n", 1);
            }
            break ;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            write(STDERR_FILENO, "cgi child process: exited with non zero value", 45);
        } else if (WIFSIGNALED(status)) {
            write(STDERR_FILENO, "cgi child process: terminated by signal: ", 41);
            const char *signame = strsignal(WTERMSIG(status));
            while (*signame)
                write(STDERR_FILENO, signame++, 1);
            if (WCOREDUMP(status))
                write(STDERR_FILENO, " - CORE DUMP", 12);
            write(STDERR_FILENO, "\n", 1);
        } else if (WIFSTOPPED(status)) {
            write(STDERR_FILENO, "cgi child process: stopped", 26);
        }
    }
}

void	sigint_handler(int sig)
{
	if (sig != SIGINT)
		return ;
	Polling::pollterminator = 1;
}


WsIpPort	getServerIpPort( ws_config_t const & serverDirectives )
{
	ws_config_t::const_iterator it = serverDirectives.find("listen");
	if (it == serverDirectives.end() || it->second.size() == 0)
		return (WsIpPort("0.0.0.0", "8080"));
	std::size_t pos = it->second[0].find(':');
	if (pos == std::string::npos)
		return (WsIpPort("0.0.0.0", it->second[0]));
	return (WsIpPort(it->second[0].substr(0, pos), it->second[0].substr(pos + 1, std::string::npos)));
}


int    createHttpServersByConfig( Polling& polling, ConfigParser& config )
{
	std::vector<HttpServer*> servers;
	for (int i = 0; i != config.getServerCount(); ++i) {
		ConfigParser::ServerContext const & currContext = config.serverConfigs[i];
		try {
			WsIpPort ipPort = getServerIpPort(currContext.serverConfig);
			std::vector<HttpServer*>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
			if (servIt != servers.end()) {
				(*servIt)->addServerConfig(currContext);
				Polling::logFile << "server: " << i << ": " << ipPort << ": added server config to existing HttpServer" << std::endl;
			} else {
				servers.push_back(new HttpServer(ipPort, polling));
				servers.back()->addServerConfig(currContext);
				Polling::logFile << "server: " << i << ": " << ipPort << ": create instance of HttpServer" << std::endl;
			}
		} catch(const std::exception& e) {
			std::cerr << "server: " << i << ": " << "error creating server: " << e.what() << '\n';
		}
	}
	return (servers.size());
}

int main(int argc, char* argv[])
{
	std::remove("logfile.txt");

	ConfigParser	config;
	Polling			polling;
	// polling.timeout_ms = 30000;
	// polling.timeout_ms = 10000;
	try {
		int serversCreated = 0;
		if (argc == 2) {
			config.configParser(argv[1]);
			serversCreated = createHttpServersByConfig(polling, config);
		}
		if (serversCreated == 0){
			new HttpServer(WsIpPort("0.0.0.0", "80"), polling);
			Polling::logFile << " -> No servers provided: create default server" << std::endl;
		}
		signal(SIGCHLD, sigchild_handler);
		signal(SIGINT, sigint_handler);
		polling.startPolling();
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
