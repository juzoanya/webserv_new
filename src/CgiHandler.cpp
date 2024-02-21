/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:01:57 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 20:41:41 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


int ChildProcessHandler::parentsock = 0;
int ChildProcessHandler::childsock = 1;

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

int    ChildProcessHandler::createChildProcess( std::string const & binPath )
{
	FileInfos   cgiExecutable(binPath, X_OK, false);
	if (cgiExecutable.getStatus() != ws_http::STATUS_200_OK)
		return (-1);
	logging("CREATE CHILD PROCESS", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	for (std::size_t i = 0; i != this->_cgiEnv.size(); ++i) {
		this->_enviroment.push_back(const_cast<char*>(this->_cgiEnv[i].c_str()));
	}
	this->_enviroment.push_back(NULL);
	this->_cgiArgs.insert(this->_cgiArgs.begin(), binPath);
	for (std::size_t i = 0; i != this->_cgiArgs.size(); ++i) {
		this->_arguments.push_back(const_cast<char*>(this->_cgiArgs[i].c_str()));
	}
	this->_arguments.push_back(NULL);
	int sock_pair[2];
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sock_pair) == -1)
		return (-1);
	if (fcntl(sock_pair[ChildProcessHandler::childsock], F_SETFL, O_NONBLOCK) == 0
		&& fcntl(sock_pair[ChildProcessHandler::parentsock], F_SETFL, O_NONBLOCK) == 0) {
		pid_t pid = fork();
		if (pid == 0) {
			pid_t	loggingPid = getpgid(0);
			logging("PROCESS GROUP ID: ", loggingPid, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);

			dup2(sock_pair[ChildProcessHandler::childsock], STDOUT_FILENO);
			dup2(sock_pair[ChildProcessHandler::childsock], STDIN_FILENO);
			close(sock_pair[ChildProcessHandler::childsock]);
			close(sock_pair[ChildProcessHandler::parentsock]);
			execve(this->_arguments[0], this->_arguments.data(), this->_enviroment.data());
			close(sock_pair[ChildProcessHandler::childsock]);
			exit(1);
		} else if (pid > 0) {
			pid_t	loggingPid = getpgid(0);
			logging("PROCESS GROUP ID: ", loggingPid, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);

			close(sock_pair[ChildProcessHandler::childsock]);
			return (sock_pair[ChildProcessHandler::parentsock]);
		}
	}
	close(sock_pair[ChildProcessHandler::childsock]);
	close(sock_pair[ChildProcessHandler::parentsock]);
	return (-1);
}


