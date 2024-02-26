/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 08:04:21 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 18:49:05 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

volatile std::sig_atomic_t	Polling::pollterminator = 0;

std::string getPollEvent(short events)
{
	std::string str = "";
	if (events & POLLIN)
		str += "IN ";
	if (events & POLLOUT)
		str += "OUT ";
	if (events & POLLHUP)
		str += "HUP ";
	return (str);
}

void    printPollArray(std::vector<struct pollfd>& pollfdArr, std::vector<APollEventHandler*>& eventHandler, int currPos)
{
	std::cout << " [ ";
	for (std::size_t i = 0; i != pollfdArr.size(); ++i) {
		std::cout << std::setw(3) << pollfdArr[i].fd;
	}
	std::cout << " ] " << std::endl;
	std::cout << " [ ";
	for (std::size_t i = 0; i != pollfdArr.size(); ++i) {
		std::cout << std::setw(3) << pollfdArr[i].revents;
	}
	std::cout << " ] " << std::endl;
	std::cout << " [ ";
	for (std::size_t i = 0; i != eventHandler.size(); ++i) {
		std::cout << std::setw(3) << (eventHandler[i] ? "a" : "n");
	}
	std::cout << " ] " << std::endl;
	if (currPos >= 0)
		std::cout << std::setw(6 + currPos * 3) << "x" << std::endl;
}

void    printPollDetail(std::vector<struct pollfd>& pollfdArr, std::vector<APollEventHandler*>& eventHandler, int currPos=-1)
{
	std::cout << " ------ whats in my pollFd list? ------ \n";
	std::cout << "\t      Fd |  events |  revents | \n";
	for (std::size_t i = 0; i != pollfdArr.size(); ++i) {
		std::cout <<"\t" << std::setw(2) << i << ": " << "[ ";
		std::cout << std::setw(2) << pollfdArr[i].fd;
		std::cout << " | ";
		std::cout << std::setw(8) << getPollEvent(pollfdArr[i].events);
		std::cout << "| ";
		std::cout << std::setw(8) << getPollEvent(pollfdArr[i].revents);
		std::cout << " ] - [" << (void*)eventHandler[i] << "]";
		if (currPos >= 0 && i == static_cast<unsigned long>(currPos))
			std::cout << "  <---";
		std::cout << std::endl;
	}
}


APollEventHandler::APollEventHandler( Polling & polling, bool deleteStopMonitoring )
 : index(polling.getAvailableIndex()), deleteOnStopMonitoring(deleteStopMonitoring), _polling(polling)
{ }

APollEventHandler::~APollEventHandler( void )
{ }


Polling::Polling( void ) : timeout_ms(0)
{ }

Polling::~Polling( void )
{ }

int Polling::getAvailableIndex( void )
{
	std::size_t freeIndex = 0;
	if(_freeIndices.empty()) {
		freeIndex = _pollFds.size();
		struct pollfd newPollStruct;
		newPollStruct.fd = -1;
		newPollStruct.events = 0;
		newPollStruct.revents = 0;
		_pollFds.push_back(newPollStruct);
		_pollEventHandlers.push_back(NULL);
		_pollTimeouts.push_back(0);
	} else {
		freeIndex = _freeIndices.top();
		_freeIndices.pop();
	}
	return (freeIndex);
}


unsigned long getCurrTimeMs( void )
{
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	return (spec.tv_sec * 1000 + spec.tv_nsec / 1000000);
}

void    Polling::startMonitoringFd( int fd, short events, APollEventHandler *handler, bool trackTimeout )
{
	// std::cout << "\n---- POLLING: START monitoring at index: " <<  handler->index << std::endl;
	// logging("---- start monitoring at index: ", handler->index, this->_pollFds[handler->index].fd, " = old fd | new fd = ", fd);
	// logging(" --- before:", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	// std::cout << "before: " << std::endl;
	// printPollDetail(_pollFds, _pollEventHandlers, handler->index);
	_pollFds[handler->index].fd = fd;
	_pollFds[handler->index].events = events;
	_pollFds[handler->index].revents = 0;
	_pollEventHandlers[handler->index] = handler;
	if (trackTimeout)
		_pollTimeouts[handler->index] = getCurrTimeMs() + timeout_ms;
	// logging(" --- after:", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
	// printPollDetail(_pollFds, _pollEventHandlers, handler->index);
	// logging(" --------------------------------------", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
}


void    Polling::stopMonitoringFd( APollEventHandler *handler )
{

	int index = handler->index;
	// logging("stop monitoring at index: ", index, " | fd: ", this->_pollFds[index].fd, EMPTY_STRING);
	// printPollDetail(_pollFds, _pollEventHandlers, index);
	if (handler && this->_pollFds[index].fd != -1) {
		close(this->_pollFds[index].fd);
		this->_pollFds[index].fd = -1;
		this->_pollFds[index].events = 0;
		this->_pollFds[index].revents = 0;
		if (handler->deleteOnStopMonitoring) {
			delete handler;
		}
		this->_pollEventHandlers[index] = NULL;
		this->_pollTimeouts[index] = 0;
		_freeIndices.push(index);
	}
	// printPollDetail(_pollFds, _pollEventHandlers);
}

void    Polling::startPolling( void )
{
	int poll_timeout = -1;
	if (timeout_ms > 0)
		poll_timeout = 200;
	while (Polling::pollterminator == 0) {
		std::size_t currSize = _pollFds.size();
		int ret = poll(_pollFds.data(), currSize, poll_timeout);
		if (ret == -1 && errno == EINTR)
			continue;
		if (ret == -1) {
			std::cerr << "poll error: " << strerror(errno) << "\n";
			throw std::runtime_error("error polling");
		}
		unsigned long curr_time = getCurrTimeMs();
		for (std::size_t i = 0; i != currSize; ++i) {
			if (_pollFds[i].fd == -1)
				continue ;
			
			if (poll_timeout == 200 && _pollTimeouts[i] > 0 && _pollTimeouts[i] < curr_time) {
				// logging("STOP MONITORING - TIMEOUT", EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
				stopMonitoringFd(_pollEventHandlers[i]);
			}
			else if (_pollFds[i].revents != 0)
				_pollEventHandlers[i]->handleEvent(_pollFds[i]);
		}
	}
	for (std::size_t i = 0; i != _pollFds.size(); ++i) {
		if (_pollEventHandlers[i] && _pollEventHandlers[i]->deleteOnStopMonitoring)
			delete _pollEventHandlers[i];
	}
}
