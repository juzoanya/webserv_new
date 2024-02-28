


#include "headers.hpp"

void	sigchild_handler(int sig)
{
	if (sig != SIGCHLD)
		return ;
	int status;
	write(1, "SIGCHILD CAUGHT\n", 17);
	int ret = waitpid(-1, &status, WNOHANG);
	if (ret == 0) {
		write(1, "no childprocesses to terminate\n", 32);
	} else if (ret == -1) {
		write(1, "error waitpid: ", 16);
		write(1, strerror(errno), 20);
		write(1, "\n", 1);
	} else {
		write(1, "CHILD TERMINATED\n", 18);
	}
	if (WIFEXITED(status)) {
		write(1, "CHILD terminated normally\n", 27);
	} else if (WIFSIGNALED(status)) {
		write(1, "CHILD terminated due signal\n", 29);
	} else if (WIFSTOPPED(status)) {
		write(1, "CHILD stopped\n", 15);
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
			std::cout << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
			std::vector<HttpServer*>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
			if (servIt != servers.end()) {
				(*servIt)->addServerConfig(currContext);
				logging("server: ", i, ": ", ipPort, ": added server config to existing HttpServer");
			} else {
				servers.push_back(new HttpServer(ipPort, polling));
				servers.back()->addServerConfig(currContext);
				logging("server: ", i, ": ", ipPort, ": create instance of HttpServer");
			}
		} catch(const std::exception& e) {
			std::cerr << "server: " << i << ": " << "error creating server: " << e.what() << '\n';
		}
	}
	return (servers.size());
}

// int	createHttpServersByConfig( Polling& polling, ConfigParser& config )
// {
// 	std::vector<HttpServer*> servers;
// 	for (int i = 0; i != config.getServerCount(); ++i) {
// 		ConfigParser::ServerContext const & currContext = config.serverConfigs[i];
// 		try {
// 			logging("create server: ", i, ": ", EMPTY_STRING, EMPTY_STRING);
// 			WsIpPort ipPort = getServerIpPort(currContext.serverConfig);
// 			std::cout << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
// 			std::vector<HttpServer*>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
// 			if (servIt != servers.end()) {
// 				(*servIt)->addServerConfig(currContext);
// 				logging("added server config to existing HttpServer", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
// 			} else {
// 				servers.push_back(new HttpServer(ipPort, polling));
// 				servers.back()->addServerConfig(currContext);
// 				logging("create instance of HttpServer", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
// 			}
// 		} catch(const std::exception& e) {
// 			std::cout << "error: " << e.what() << '\n';
// 		}
// 	}
// 	return (servers.size());
// }

int main(int argc, char* argv[])
{
	std::remove("logfile.txt");

	ConfigParser	config;
	Polling			polling;
	polling.timeout_ms = 10000;
	try {
		int serversCreated = 0;
		if (argc == 2) {
			config.configParser(argv[1]);
			serversCreated = createHttpServersByConfig(polling, config);
		}
		if (serversCreated == 0){
			new HttpServer(WsIpPort("0.0.0.0", "80"), polling);
			logging("no servers provided: create default server", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
		}
		signal(SIGCHLD, sigchild_handler);
		signal(SIGINT, sigint_handler);
		polling.startPolling();
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
