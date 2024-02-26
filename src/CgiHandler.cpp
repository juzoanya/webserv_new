/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:57 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 18:40:01 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

int ChildProcessHandler::parentsock = 1;
int ChildProcessHandler::childsock = 0;

void    ChildProcessHandler::addArgument( std::string const & argument )
{
	this->_cgiArgs.push_back(argument);
}

void    ChildProcessHandler::addEnvVariable( std::string const & envVar )
{
	this->_cgiEnv.push_back(envVar);
}

ChildProcessHandler::ChildProcessHandler( Polling & polling, HttpHandler& parent )
 : APollEventHandler(polling, true), _parent(parent)
{ }

ChildProcessHandler::~ChildProcessHandler( void )
{ }

void    ChildProcessHandler::handleEvent( struct pollfd & pollfd )
{
	_parent.handleChildEvent(pollfd);
}

bool    ChildProcessHandler::createChildProcess( std::string const & binPath, std::string const & chToDir )
{
	FileInfo   cgiExecutable(binPath, false);
	if (!cgiExecutable.checkInfo(FileInfo::EXECUTABLE))
		return (false);
	logging("CREATE CHILD PROCESS", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	
	std::vector<char*> envp;
	for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i)
		envp.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
	envp.push_back(NULL);

	std::vector<char*> args;
	this->_cgiArgs.insert(this->_cgiArgs.begin(), binPath);
	for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i)
		args.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
	args.push_back(NULL);

	int sock_pair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock_pair) == -1)
		return (false);

	pid_t pid = fork();
	if (pid == 0) {
		if (chdir(chToDir.c_str()) == -1)
			exit(1);
		dup2(sock_pair[ChildProcessHandler::childsock], STDOUT_FILENO);
		dup2(sock_pair[ChildProcessHandler::childsock], STDIN_FILENO);
		close(sock_pair[ChildProcessHandler::childsock]);
		close(sock_pair[ChildProcessHandler::parentsock]);
		execve(args[0], args.data(), envp.data());
		exit(1);
	} else if (pid > 0) {
		close(sock_pair[ChildProcessHandler::childsock]);
		if (fcntl(sock_pair[ChildProcessHandler::parentsock], F_SETFL, O_NONBLOCK) == 0) {
			_polling.startMonitoringFd(sock_pair[ChildProcessHandler::parentsock], POLLIN | POLLOUT, this, true);
			return (true);
		}
	}
	close(sock_pair[ChildProcessHandler::parentsock]);
	return (false);
}


