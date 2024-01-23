


#include "headers.hpp"

int eventLoop(std::vector<HttpServer>& servers)
{
	while (true) {
		for (std::vector<HttpServer>::iterator it = servers.begin(); it != servers.end(); ++it) {
			try {
				it->start();
			} catch(const std::exception& e) {
				std::cerr << e.what() << '\n';
			}
		}
	}
}

void	createHttpServersByConfig(ConfigParser& config, std::vector<HttpServer>& servers)
{
	for (int i = 0; i != config.getServerCount(); ++i) {
		try {
			ServerWasCreated createHelper(config.serverConfigs[i].serverConfig);
			std::cout << "create server: " << i << ": [" << createHelper.ipStr << ":" << createHelper.portStr << "]: ";
			std::vector<HttpServer>::iterator it;
			it = std::find_if<std::vector<HttpServer>::iterator, ServerWasCreated>(servers.begin(), servers.end(), createHelper);
			if (it != servers.end()) {
				it->serverConfigs.addServerConfig(&config.serverConfigs[i]);
				std::cout << "added server config to existing HttpServer" << std::endl; 
			} else {
				servers.push_back(HttpServer(createHelper.ipStr, createHelper.portStr));
				servers.back().serverConfigs.addServerConfig(&config.serverConfigs[i]);
				std::cout << "create instance of HttpServer" << std::endl; 
			}
		} catch(const std::exception& e) {
			std::cout << "unable to create server: " << e.what() << '\n';
		}
	}
	if (servers.empty())
		servers.push_back(HttpServer("0.0.0.0", "8080"));
}

int main(int argc, char* argv[])
{
	(void) argc;
	std::vector<HttpServer>	servers;
	ConfigParser	config;
	try {
		config.configParser(argv[1]);
		createHttpServersByConfig(config, servers);
		eventLoop(servers);
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
