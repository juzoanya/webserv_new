/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:40:12 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 20:38:16 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include "headers.hpp"

std::string getDateString(std::time_t time, const char* format);
std::size_t getStingStreamSize(std::stringstream & ss);
std::size_t getStreamSize(std::istream & is );
void        addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize );
std::string urlEncodeString(std::string const & value);
void    printCharOrRep(char c, bool printEndl);

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

	file << getDateString(0, "%H:%M:%S ");
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

class FileInfo {
	public:
		enum FileMode { EXISTS = 0x01, IS_FILE = 0x02, IS_DIRECTORY = 0x04, READABLE = 0x08, WRITEABLE = 0x10, EXECUTABLE = 0x20 };

		FileInfo( std::string const & filePath, bool readDir );
		~FileInfo( void );

		void	readDirectory( void );
		int		setDirListing( std::stringstream & ss, std::string requestPathDecoded ) const;
		bool 	checkInfo( int fileInfoFlags ) const;
		bool	checkContainedFile( std::string const & filename );

	private:
		struct DirListingInfos {
			DirListingInfos( std::string const & name, struct stat & fileStat );
			~DirListingInfos( void );
			std::string fileName;
			std::string fileSize;
			std::string fileModDate;
			mode_t      statFileType;
		};

		char							_infos;
		std::string						_filePath;
		std::vector<DirListingInfos>    _dirListingInfos;
};

std::ostream & operator<<( std::ostream & o, WsIpPort const & src );

#endif
