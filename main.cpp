


#include "headers.hpp"

int main(int argc, char* argv[]) //(void)
{
	(void) argc;
	try
	{
		// for (int i = 0; i < parser.getServerCount(); ++i)
		// {
		// 	HttpServer	server();
		// 	server.start();
		// }
		// HttpServer	server;
		// server.start();
		ConfigParser	parser;
		parser.configParser(argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}