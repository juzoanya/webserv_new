/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 08:04:21 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 19:38:06 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


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


NextPollEvent::NextPollEvent( struct pollfd& polls, APollEventHandler&  eventHandler )
 : pollstruct(polls), handler(eventHandler)
{ }

NextPollEvent::~NextPollEvent( void )
{ }


APollEventHandler::APollEventHandler( Polling & polling, bool deleteStopMonitoring )
 : index(polling.getAvailableIndex()), deleteOnStopMonitoring(deleteStopMonitoring), _polling(polling)
{
    std::cout << "APollEventHandler Constructor" << std::endl;
}

APollEventHandler::~APollEventHandler( void )
{
    std::cout << "APollEventHandler Destructor - index: " << this->index << std::endl;
    // _polling.stopMonitoringFd(this);
}



Polling::Polling( void ) : readBuffer(4096), _currPos(0), _currSize(0)
{ }

Polling::~Polling( void )
{ }

int Polling::getAvailableIndex( void )
{
    // std::cout << "\ngetAvailableIndex before" << std::endl;
    // printPollDetail(_pollFds, _pollEventHandlers);
    std::size_t freeIndex = 0;
    if(_freeIndices.empty()) {
        freeIndex = _pollFds.size();
        struct pollfd newPollStruct;
        newPollStruct.fd = -1;
        newPollStruct.events = 0;
        newPollStruct.revents = 0;
        _pollFds.push_back(newPollStruct);
        _pollEventHandlers.push_back(NULL);
    } else {
        freeIndex = _freeIndices.top();
        _freeIndices.pop();
    }
    // printPollDetail(_pollFds, _pollEventHandlers);
    return (freeIndex);
}

void    Polling::startMonitoringFd( int fd, short events, APollEventHandler *handler )
{
    // std::cout << "\nstart monitoring at index: " << handler->index << " | old fd: " << this->_pollFds[handler->index].fd << " | new fd: " << fd << std::endl;
    // printPollDetail(_pollFds, _pollEventHandlers, handler->index);
    _pollFds[handler->index].fd = fd;
    _pollFds[handler->index].events = events;
    _pollFds[handler->index].revents = 0;
    _pollEventHandlers[handler->index] = handler;
    // printPollDetail(_pollFds, _pollEventHandlers, handler->index);
}


void    Polling::stopMonitoringFd( APollEventHandler *handler )
{
    int index = handler->index;
    // std::cout << "\nstop monitoring at index: " << index << " | fd: " << this->_pollFds[index].fd << std::endl;
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
        _freeIndices.push(index);
    }
    // printPollDetail(_pollFds, _pollEventHandlers);
}

NextPollEvent   Polling::getNextEvent( void )
{
    // std::cout << "\n -- getNextEvent --\n";
    while (true) {
        // std::cout << "---------- LOOP --\n";

        // printPollArray(_pollFds, _pollEventHandlers, _currPos);

        if (_currPos == _currSize) {
            _currSize = _pollFds.size();
            int ret = poll(_pollFds.data(), _currSize, -1);
            // std::cout << "call poll function - pos: " << _currPos << " | size: " << _currSize << std::endl;
            if (ret == -1)
                throw std::runtime_error("error polling");
            _currPos = 0;
        } else {
            while (_currPos < _currSize && (_pollFds[_currPos].fd == -1 || _pollFds[_currPos].revents == 0)) {
                _currPos++;
            }
            if (_currPos < _currSize) {
                // std::cout << "_currPos < _currSize -> break and return" << std::endl;
                break ;
            }
        }
    }
    // std::cout << "next event currPos: " << _currPos << std::endl;
    // std::cout << "next event " << "fd: " << this->_pollFds[_currPos].fd << "| events: " << this->_pollFds[_currPos].events << " | revents: " << this->_pollFds[_currPos].revents << std::endl;
    if (!this->_pollEventHandlers[_currPos]) {
        std::cout << "is null\n";
        exit(1);
    }
    NextPollEvent nextEvent = NextPollEvent(this->_pollFds[_currPos], *this->_pollEventHandlers[_currPos]);
    _currPos++;
    return (nextEvent);
}

// NextPollEvent   Polling::getNextEvent( void )
// {
//     std::cout << "\n -- getNextEvent --\n";
//     while (true) {
//         std::cout << "---------- LOOP --\n";
//         if (_currPos == _currSize) {
//             _currSize = _pollFds.size();
//             int ret = poll(_pollFds.data(), _currSize, -1);
//             std::cout << "call poll function - pos: " << _currPos << " | size: " << _currSize << std::endl;
//             if (ret == -1)
//                 throw std::runtime_error("error polling");
//             _currPos = 0;
//         } else {
//             while (_currPos < _currSize && (_pollFds[_currPos].fd == -1 || _pollFds[_currPos].revents == 0)) {
//                 std::cout << "is -1 - pos: " << _currPos << " | size: " << _currSize << std::endl;
//                 std::cout << "event " << "fd: " << this->_pollFds[_currPos].fd << "| events: " << this->_pollFds[_currPos].events << " | revents: " << this->_pollFds[_currPos].revents << std::endl;
//                 _currPos++;
//             }
//             if (_currPos < _currSize) {
//                 std::cout << "_currPos < _currSize -> break and return" << std::endl;
//                 break ;
//             }
//         }
//     }
//     std::cout << "next event currPos: " << _currPos << std::endl;
//     std::cout << "next event " << "fd: " << this->_pollFds[_currPos].fd << "| events: " << this->_pollFds[_currPos].events << " | revents: " << this->_pollFds[_currPos].revents << std::endl;
//     NextPollEvent nextEvent = NextPollEvent(this->_pollFds[_currPos], *this->_pollEventHandlers[_currPos]);
//     _currPos++;
//     return (nextEvent);
// }
