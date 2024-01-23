


#include "headers.hpp"

int main(int argc, char* argv[]) //(void)
{
	(void) argc;
	//(void) argv;
	try
	{
		ConfigParser	parser;
		parser.configParser(argv[1]);
		// for (int i = 0; i < parser.getServerCount(); ++i)
		// {
		// 	ConfigHandler	handler;
		// 	handler.addServerConfig(&parser.serverConfigs[i]);
		// 	std::vector<std::string> value = parser.getConfigValue(&parser.serverConfigs[i], "root");
		// 	std::vector<std::string>::iterator	it;
		// 	for (it = value.begin(); it != value.end(); ++it)
		// 		std::cout << *it << std::endl;
			
		// }
		HttpServer	server(parser);
		server.start();

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}