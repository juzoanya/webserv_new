


#include "HttpServer.hpp"

// HttpServer::HttpServer() : _serverSocket(-1)
// {
// 	// SocketAddr_in	serverAddress;
// }

HttpServer::HttpServer(/*ConfigParser::ServerContext serverConfig, std::map<std::string, std::vector<std::string> > httpConfig*/) : _serverSocket(-1), _nfds(0)
{
	unsigned short	port = 8082;

	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw HttpServer::CreatingServerSocketException();

	// Setup server address structure
	memset(&this->_serverAddress, 0, sizeof(this->_serverAddress));
	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(port);

	// Set the server socket to non-blocking mode
	fcntl(this->_serverSocket, F_SETFL, O_NONBLOCK);//, FD_CLOEXEC);
	
	// Bind the server socket to the specified address and port
	if (bind(this->_serverSocket, (struct sockaddr *)&this->_serverAddress, sizeof(this->_serverAddress)) == -1)
		throw HttpServer::BindSocketServerException();

	// Listen for incoming connection
	if (listen(this->_serverSocket, 10) == -1)
		throw HttpServer::ListeningForConnectionException();

	// Set up poll fd structures for the server socket and connected clients
	//struct pollfd fds[1024];
	this->_nfds = 1;
	memset(this->_fds, 0, sizeof(this->_fds));
	// Add the server socket to the pollfd array
	this->_fds[0].fd = this->_serverSocket;
	this->_fds[0].events = POLLIN;
}

HttpServer::~HttpServer()
{

}

// HttpServer::HttpServer(const HttpServer& src)
// {

// }

// HttpServer& HttpServer::operator=(const HttpServer& rhs)
// {

// }

void	HttpServer::start(void)
{
	


	while (true)
	{
		
		int ready = poll(this->_fds, this->_nfds, -1);
		if (ready == -1)
			throw HttpServer::EpollWaitException();

		int currSize = this->_nfds;

		for (int i = 0; i < currSize; ++i)
		{
			if (this->_fds[i].revents == 0)
				continue;
			if (this->_fds[0].revents != POLLIN)
				std::cout << "Add error herer" << std::endl;
			if (this->_fds[i].fd == this->_serverSocket)
				handleAccept();
			else if (this->_fds[i].revents & POLLOUT)
				handleWrite(this->_fds[i].fd);
			else
				handleRead(this->_fds[i].fd);

		}

		// for (int i = 1; i <= this->_maxClients; ++i)
		// {
		// 	if (this->_fds[i].revents & (POLLIN | POLLHUP))
		// 		handleRead(this->_fds[i].fd);
		// 	if (this->_fds[i].revents & POLLOUT)
		// 		handleWrite(this->_fds[i].fd);
		// }
	}
}

void	HttpServer::handleRead(int clientSocket)
{
	char	buffer[1024];

	ssize_t readByte = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (readByte == -1)
		throw HttpServer::ClientSocketReadException();
	else if (readByte == 0)
	{
		// Connection closed by client
		close(clientSocket);
		clientSocket = -1;
	}
	else
	{
		std::string	request(buffer, readByte);
		std::string	response = RequestHandler::handleRequest(request);
	}
}

void	HttpServer::handleWrite(int clientSocket)
{
	const char*	response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
	ssize_t writeByte = send(clientSocket, response, strlen(response), 0);
	if (writeByte == -1)
		throw HttpServer::ClientSocketWriteException();


	close(clientSocket);
	clientSocket = -1;
}

void	HttpServer::handleAccept()
{
	while (true)
	{
		//Accept all incoming connections
		int	clientSocket = accept(this->_serverSocket, NULL, NULL);
		if (clientSocket == -1)
			throw HttpServer::AcceptConnectionException();
		
		this->_fds[this->_nfds].fd = clientSocket;
		this->_fds[this->_nfds].events = POLLIN;
		this->_nfds++;
		break;
	}
}

