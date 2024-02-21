/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:40:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 12:38:58 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include "headers.hpp"

template<typename T>
std::string toStr(T data)
{
	std::stringstream ss;
	ss << data;
	return (ss.str());
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
void	logging(const T1& first, T2& second, T3& third, T4 fourth, T5& fifth)
{
	const char*	fileName = "logfile.txt";
	std::ofstream file(fileName, std::ios_base::app);
	if (!file.is_open()){
		std::ofstream createFile(fileName);
		createFile.close();
		file.open(fileName, std::ios::app);
		if (!file.is_open()){
			std::cerr << "Faile to open Log file for writting!" << std::endl;
		}
	}

	time_t now = time(0);
	char	*dateTime = ctime(&now);

	file << dateTime << " ";
	if (sizeof(T1) != 0)
		file << first;
	if (sizeof(T2) != 0)
		file << second;
	if (sizeof(T3) != 0)
		file << third;
	if (sizeof(T4) != 0)
		file << fourth;
	if (sizeof(T5) != 0)
		file << fifth << std::endl;
	file.close();
}

class HttpServer;

class WsIpPort {
	public:
		WsIpPort( void );
		WsIpPort( struct sockaddr_in const & addr );
		WsIpPort( std::string const & ip, std::string const & port );
		~WsIpPort( void );

		bool operator==( WsIpPort const & rhs ) const;
		bool operator()( HttpServer const & rhs ) const;
		bool operator()( HttpServer const * rhs ) const;

		std::string const & getIpStr( void ) const;
		std::string const & getPortStr( void ) const;
		struct sockaddr_in const & getSockaddrIn( void ) const;

	private:
		std::string         _ipStr;
		std::string         _portStr;
		struct sockaddr_in  _address;
};

class FileInfos {
	public:
		FileInfos( void );
		FileInfos( std::string const & filePath, int accessType, bool readDirectory );
		~FileInfos( void );

		void                changePath( std::string const & filePath, int accessType, bool readDirectory );
		int                 setDirListing( std::stringstream & ss, std::string requestPathDecoded );
		bool                isDirectory( void ) const;
		ws_http::statuscodes_t getStatus( void ) const;

		bool                checkContainedFile( std::string const & filename );
		std::string const & checkContainedFile( std::vector<std::string> const & files );

	private:
		struct DirListingInfos {
			DirListingInfos( std::string const & name, struct stat & fileStat );
			~DirListingInfos( void );
			std::string fileName;
			std::string fileSize;
			std::string fileModDate;
			mode_t      statFileType;
		};

		ws_http::statuscodes_t  _status;
		char                    _isDir;
		std::vector<DirListingInfos>    _dirListingInfos;
};


std::string getDateString(std::time_t time, const char* format);
std::size_t getStingStreamSize(std::stringstream & ss);
std::size_t getStreamSize(std::istream & is );
void        addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize );

void    printCharOrRep(char c, bool printEndl);

#endif
