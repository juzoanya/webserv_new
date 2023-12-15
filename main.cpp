


#include "headers.hpp"

int main(void)
{
	try
	{
		// for (int i = 0; i < parser.getServerCount(); ++i)
		// {
		// 	HttpServer	server();
		// 	server.start();
		// }
		HttpServer	server;
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}