/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 08:00:38 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 18:49:55 by mberline         ###   ########.fr       */
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

class Polling {
	public:
		Polling( void );
		~Polling( void );

		int     getAvailableIndex( void );
		void    startMonitoringFd( int fd, short events, APollEventHandler *handler, bool trackTimeout );
		void    stopMonitoringFd( APollEventHandler *handler );
		void    startPolling( void );

		unsigned int timeout_ms;
		volatile static std::sig_atomic_t	pollterminator;

	private:
		std::vector<struct pollfd>      _pollFds;
		std::vector<APollEventHandler*> _pollEventHandlers;
		std::vector<unsigned long>      _pollTimeouts;
		std::priority_queue<int, std::vector<int>, std::greater<int> >        _freeIndices;
};

#endif
