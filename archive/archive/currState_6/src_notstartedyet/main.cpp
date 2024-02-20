


#include "headers.hpp"

void	createHttpServersByConfig(ConfigParser& config, std::vector<HttpServ>& servers)
{
	for (int i = 0; i != config.getServerCount(); ++i) {
		try {
			ws_config_t::iterator it = config.serverConfigs[i].serverConfig.find("listen");
			std::size_t pos = std::string::npos;
			WsIpPort ipPort;
			if (it != config.serverConfigs[i].serverConfig.end())
				pos = it->second[0].find(':');
			if (it == config.serverConfigs[i].serverConfig.end() || it->second.size() == 0) {
				ipPort = WsIpPort("0.0.0.0", "8080");
			} else if (pos == std::string::npos) {
				ipPort = WsIpPort("0.0.0.0", it->second[0]);
			} else {
				ipPort = WsIpPort(it->second[0].substr(0, pos), it->second[0].substr(pos + 1, std::string::npos));
			}
			std::cout << "create server: " << i << ": [" << ipPort.getIpStr() << ":" << ipPort.getPortStr() << "]: ";
			std::vector<HttpServ>::iterator servIt = std::find_if(servers.begin(), servers.end(), ipPort);
			if (servIt != servers.end()) {
				servIt->serverConfigs.addServerConfig(&config.serverConfigs[i]);
				std::cout << "added server config to existing HttpServer" << std::endl; 
			} else {
				servers.push_back(HttpServ(ipPort));
				servers.back().serverConfigs.addServerConfig(&config.serverConfigs[i]);
				std::cout << "create instance of HttpServer" << std::endl; 
			}
		} catch(const std::exception& e) {
			std::cout << "unable to create server: " << e.what() << '\n';
		}
	}
	// debPrintServerData(servers, false);
	if (servers.empty())
		servers.push_back(HttpServ(WsIpPort("0.0.0.0", "8080")));
}

int main(int argc, char* argv[])
{
	(void) argc;
	std::vector<HttpServ>	servers;
	ConfigParser	config;

	// WsIpPort ipp;
	// ConfigHandler bla(ipp);
	// HttpRequest req(bla);
	// HttpHandler han(bla, ipp);
	// std::cout << "size of WsIpPort: " << sizeof(ipp) << std::endl;
	// std::cout << "size of HttpRequest: " << sizeof(req) << std::endl;
	// std::cout << "size of ConfigHandler: " << sizeof(bla) << std::endl;
	// std::cout << "size of HttpHandler: " << sizeof(han) << std::endl;


	// std::vector<struct pollfd*> polls;
	struct pollfd joo[4];
	poll(joo, 1, 0);

	try {
		config.configParser(argv[1]);
		createHttpServersByConfig(config, servers);
		std::vector<HttpServ>::iterator it = servers.begin();
		while (true) {
			if (it == servers.end())
				it = servers.begin();
			try {
				it->runPoll();
			} catch(const std::exception& e) {
				std::cerr << e.what() << '\n';
			}
			it++;
		}
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return (0);
}
