


#include "headers.hpp"

void	sigchild_handler(int sig)
{
	if (sig != SIGCHLD)
		return ;
	int status;
	write(1, "SIGCHILD CAUGHT\n", 17);
	waitpid(-1, &status, WNOHANG);
	write(1, "CHILD TERMINATED\n", 18);
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

int	createHttpServersByConfig( Polling& polling, ConfigParser& config )
{
    std::vector<HttpServer*> servers;
	for (int i = 0; i != config.getServerCount(); ++i) {
        ConfigParser::ServerContext const & currContext = config.serverConfigs[i];
		try {
			std::cout << "create server: " << i << ": ";
			WsIpPort ipPort = getServerIpPort(currContext.serverConfig);
			std::cout << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
			std::vector<HttpServer*>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
			if (servIt != servers.end()) {
                (*servIt)->addServerConfig(currContext);
				std::cout << "added server config to existing HttpServer" << std::endl; 
			} else {
                servers.push_back(new HttpServer(ipPort, polling));
                servers.back()->addServerConfig(currContext);
				std::cout << "create instance of HttpServer" << std::endl; 
			}
		} catch(const std::exception& e) {
			std::cout << "error: " << e.what() << '\n';
		}
	}
    return (servers.size());
}

int main(int argc, char* argv[])
{
	ConfigParser	config;
	Polling			polling;
	polling.timeout_ms = 10000;
	try {
		int serversCreated = 0;
		if (argc == 2) {
			config.configParser(argv[1]);
			serversCreated = createHttpServersByConfig(polling, config);
		}
		if (serversCreated == 0)
			new HttpServer(WsIpPort("0.0.0.0", "80"), polling);
		signal(SIGCHLD, sigchild_handler);
		polling.startPolling();
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}

// void	createHttpServersByConfig(ConfigParser& config, Polling& polling)
// {
// 	std::vector<HttpServer*>	servers;
// 	for (int i = 0; i != config.getServerCount(); ++i) {
// 		try {
// 			ws_config_t::iterator it = config.serverConfigs[i].serverConfig.find("listen");
// 			std::size_t pos = std::string::npos;
// 			WsIpPort ipPort;
// 			if (it != config.serverConfigs[i].serverConfig.end())
// 				pos = it->second[0].find(':');
// 			if (it == config.serverConfigs[i].serverConfig.end() || it->second.size() == 0) {
// 				ipPort = WsIpPort("0.0.0.0", "8080");
// 			} else if (pos == std::string::npos) {
// 				ipPort = WsIpPort("0.0.0.0", it->second[0]);
// 			} else {
// 				ipPort = WsIpPort(it->second[0].substr(0, pos), it->second[0].substr(pos + 1, std::string::npos));
// 			}
// 			std::cout << "create server: " << i << ": [" << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
// 			std::vector<HttpServ>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
// 			if (servIt != servers.end()) {
// 				servIt->serverConfigs.addServerConfig(&config.serverConfigs[i]);
// 				std::cout << "added server config to existing HttpServer" << std::endl; 
// 			} else {
// 				servers.push_back(HttpServ(ipPort));
// 				servers.back().serverConfigs.addServerConfig(&config.serverConfigs[i]);
// 				std::cout << "create instance of HttpServer" << std::endl; 
// 			}
// 		} catch(const std::exception& e) {
// 			std::cout << "unable to create server: " << e.what() << '\n';
// 		}
// 	}
// 	// debPrintServerData(servers, false);
// 	if (servers.empty())
// 		servers.push_back(HttpServ(WsIpPort("0.0.0.0", "8080")));
// }



// int main(int argc, char* argv[])
// {
// 	(void) argc;
// 	std::vector<HttpServ>	servers;
// 	ConfigParser			config;
// 	Polling			polling;

// 	try {
// 		config.configParser(argv[1]);
// 		createHttpServersByConfig(config, polling);
// 		std::vector<HttpServ>::iterator it = servers.begin();
// 		while (true) {
// 			if (it == servers.end())
// 				it = servers.begin();
// 			try {
// 				it->runPoll();
// 			} catch(const std::exception& e) {
// 				std::cerr << e.what() << '\n';
// 			}
// 			it++;
// 		}
// 	} catch(const std::exception& e) {
// 		std::cerr << e.what() << '\n';
// 	}
// 	return (0);
// }
