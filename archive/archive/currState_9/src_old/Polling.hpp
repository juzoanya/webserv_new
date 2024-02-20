/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 08:00:38 by mberline          #+#    #+#             */
/*   Updated: 2024/02/09 18:20:14 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLING_HPP
#define POLLING_HPP

#include "headers.hpp"

class Polling;

class APollEventHandler {
    public:
        APollEventHandler( Polling & polling, bool deleteStopMonitoring );
        virtual ~APollEventHandler( void );
        virtual void handleEvent( struct pollfd & pollfd ) = 0;
        const int index;
        const bool deleteOnStopMonitoring;
    protected:
        Polling &   _polling;
    private:

};

struct NextPollEvent {
    NextPollEvent( struct pollfd& polls, APollEventHandler&  eventHandler );
    ~NextPollEvent( void );
    struct pollfd&      pollstruct;
    APollEventHandler&  handler;
};

class Polling {
    public:
        Polling( void );
        ~Polling( void );

        int     getAvailableIndex( void );
        void    startMonitoringFd( int fd, short events, APollEventHandler *handler );
        void    stopMonitoringFd( APollEventHandler *handler );
        void    startPolling( void );

        // std::array<char, 4096>  readBuffer;
        buff_t  readBuffer;
        NextPollEvent   getNextEvent( void );

    private:
        std::vector<struct pollfd>      _pollFds;
        std::vector<APollEventHandler*> _pollEventHandlers;
        // std::priority_queue<int>        _freeIndices;
        std::priority_queue<int, std::vector<int>, std::greater<int> >        _freeIndices;
        std::size_t _currPos;
        std::size_t _currSize;
};

#endif