/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:39:29 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 22:01:13 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


std::string urlEncodeString(std::string const & value)
{
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		unsigned char c = *i;

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos) {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << '%' << std::uppercase << std::setw(2) << int((unsigned char) c) << std::nouppercase;
	}

	return escaped.str();
}

std::string getDateString(std::time_t time, const char* format)
{
	std::time_t t = time;
	if (t == 0)
		t = std::time(0);
	std::tm* now = std::gmtime(&t);
	char buff[1000];
	std::size_t size = strftime(buff, 1000, format, now);
	return (std::string(buff, size));
}

std::size_t getStingStreamSize( std::stringstream & ss )
{
	ss.seekg(0, std::ios::end);
	std::size_t pageSize = ss.tellg();
	ss.seekg(0, std::ios::beg);
	return (pageSize);
}

std::size_t getStreamSize( std::istream & is )
{
	is.seekg(0, std::ios::end);
	std::size_t pageSize = is.tellg();
	is.seekg(0, std::ios::beg);
	return (pageSize);
}

void    addStreamToBuff( buff_t & buffer, std::istream & is, std::size_t streamSize )
{
	std::size_t currBufferSize = buffer.size();
	buffer.resize(currBufferSize + streamSize);
	char* dataPtr = const_cast<char*>(buffer.data()) + currBufferSize;
	is.read(dataPtr, streamSize);
}


WsIpPort::WsIpPort( void )
{ }

WsIpPort::WsIpPort( struct sockaddr_in const & addr ) : _address(addr)
{
	this->_portStr = toStr(htons(addr.sin_port));
	std::stringstream ss;
	uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
	ss << ((ip_addr >> 24) & 0xFF) << ".";
	ss << ((ip_addr >> 16) & 0xFF) << ".";
	ss << ((ip_addr >> 8) & 0xFF) << ".";
	ss << (ip_addr & 0xFF);
	this->_ipStr = ss.str();
}

WsIpPort::WsIpPort( std::string const & ipStr, std::string const & portStr ) : _ipStr(ipStr), _portStr(portStr)
{
	memset(&this->_address, 0, sizeof(struct sockaddr_in));
	uint32_t ip = 0;
	unsigned int ipPartNbr;
	const char* ipPtr = ipStr.data();
	char* rest;
	int i = 0;
	for (; i != 4; ++i) {
		ipPartNbr = strtol(ipPtr, &rest, 10);
		if ((i == 3 && *rest != 0) || (i < 3 && *rest != '.') || (i < 3 && !std::isdigit(*(rest + 1))))
			throw::std::runtime_error("invalid ip address");
		ipPtr = rest + 1;
		ip |= (ipPartNbr << (i * 8));
	}
	unsigned int port = strtol(portStr.c_str(), &rest, 10);
	if (port == 0 || *rest != 0)
		throw::std::runtime_error("invalid port");
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = ip;
	this->_address.sin_port = htons(port);
}

WsIpPort::~WsIpPort( void )
{ }

bool WsIpPort::operator==( WsIpPort const & rhs ) const { return (rhs._ipStr == this->_ipStr && rhs._portStr == this->_portStr); }
bool WsIpPort::operator()( HttpServer const & rhs ) const { return (rhs.serverIpPort.getIpStr() == this->_ipStr && rhs.serverIpPort.getPortStr() == this->_portStr); }
bool WsIpPort::operator()( HttpServer const * rhs ) const { return (rhs->serverIpPort.getIpStr() == this->_ipStr && rhs->serverIpPort.getPortStr() == this->_portStr); }
std::string const & WsIpPort::getIpStr( void ) const { return (this->_ipStr); }
std::string const & WsIpPort::getPortStr( void ) const { return (this->_portStr); }
struct sockaddr_in const & WsIpPort::getSockaddrIn( void ) const { return (this->_address); }

FileInfo::FileInfo( std::string const & filePath, bool readDir )
 : _infos(0), _filePath(filePath)
{
	_dirListingInfos.clear();
	struct stat fileStat;
	if (stat(filePath.c_str(), &fileStat) == 0) {
		if (S_ISDIR(fileStat.st_mode))
			_infos |= EXISTS | IS_DIRECTORY;
		else if (S_ISREG(fileStat.st_mode))
			_infos |= EXISTS | IS_FILE;
		if (access(filePath.c_str(), R_OK) == 0)
			_infos |= READABLE;
		if (access(filePath.c_str(), W_OK) == 0)
			_infos |= WRITEABLE;
		if (access(filePath.c_str(), X_OK) == 0)
			_infos |= EXECUTABLE;
	}
	if (!(_infos & EXISTS) || !readDir)
		return ;

	DIR* currDir = opendir(filePath.c_str());
	if (currDir != NULL) {
		struct dirent *dirElem = NULL;
		while ((dirElem = readdir(currDir))) {
			struct stat fileStat;
			std::string filename = std::string(dirElem->d_name, strlen(dirElem->d_name));
			if (filename == "." || filename == "..")
				continue;
			std::string newFilePath = filePath + "/" + filename;

			if (stat(newFilePath.c_str(), &fileStat) == 0 && access(newFilePath.c_str(), R_OK) == 0
				&& (S_ISDIR(fileStat.st_mode) || S_ISREG(fileStat.st_mode))) {
				this->_dirListingInfos.push_back(DirListingInfos(filename, fileStat));
			}
		}
		closedir(currDir);
	}
}

FileInfo::~FileInfo( void )
{ }

bool 	FileInfo::checkInfo( int fileInfoFlags ) const { 
	if (_infos & READABLE)
		std::cout << "INFO STATE ------> READABLE\n";
	if (_infos & WRITEABLE)
		std::cout << "INFO STATE ------> READABLE\n";
	std::cout << "INFO STATE ---------->" << static_cast<int>(_infos) << std::endl;
	return (_infos & fileInfoFlags); }

bool	FileInfo::checkContainedFile( std::string const & filename )
{
	for (std::size_t i = 0; i != this->_dirListingInfos.size(); ++i) {
		if (filename == this->_dirListingInfos[i].fileName)
			return (true);
	}
	return (false);
}

int		FileInfo::setDirListing( std::stringstream & ss, std::string requestPathDecoded ) const
{
	if (requestPathDecoded[requestPathDecoded.size() - 1] != '/')
		requestPathDecoded.push_back('/');
	ss << "<!DOCTYPE html>";
	ss << "<html lang=\"en\">";
	ss << "<head>";
	ss << "<meta charset=\"UTF-8\">""<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	ss << "<title>Directory Listing</title>";
	ss << "<style>"
			"* { font-family: sans-serif; }\
			table { border-collapse: collapse; margin: 1em 0; font-size: 1em; min-width: 400px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.15); }\
			tr td:first-child { width: 20px; white-space: nowrap;overflow: hidden; text-overflow: ellipsis; }\
			thead tr { background-color: #009879; color: #ffffff; text-align: left; }\
			th, td { padding: 1em 2em; }\
			tbody tr:hover { background-color: #d9d7d7; }\
			tbody tr { border-bottom: 1px solid #dddddd; }\
			tbody tr:nth-of-type(even) { background-color: #f3f3f3; }\
			tr:nth-of-type(even):hover { background-color: #d9d7d7; }"
		"</style>";
	ss << "</head>";
	ss << "<body>";
	ss << "<h1>";
	std::string dirstrs, rootPart;
	std::stringstream dirstream(requestPathDecoded);
	while (std::getline(dirstream, dirstrs, '/')) {
		rootPart += dirstrs + "/";
		if (dirstrs.empty()) dirstrs = "~";
		std::string a = "<a href=\"" + rootPart + "\">" + dirstrs + "</a><span> / </span>";
		ss << a;
	}
	ss << "</h1>";
	ss << "<table>";
	ss << "<thead><tr><th>Name</th><th>Size</th><th>Last Modified</th></tr></thead>";
	ss << "<tbody>";
	for (std::size_t i = 0; i != this->_dirListingInfos.size(); ++i) {
		ss << "<tr>";
		ss << "<td><a href=\"" << requestPathDecoded << this->_dirListingInfos[i].fileName << "\">" << this->_dirListingInfos[i].fileName << "</a></td>";
		ss << "<td>" << this->_dirListingInfos[i].fileSize << "</td>";
		ss << "<td>" << this->_dirListingInfos[i].fileModDate << "</td>";
		ss << "</tr>";
	}
	ss << "</tbody>";
	ss << "</table>" "</body>" "</html>";
	return (0);
}


FileInfo::DirListingInfos::DirListingInfos( std::string const & name, struct stat & fileStat )
 : fileName(name), statFileType(fileStat.st_mode)
{
	const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	double dSize = static_cast<double>(fileStat.st_size);
	std::size_t k = 0;
	for (; dSize >= 1000.0 && k < 9; ++k, dSize /= 1000.0) ;
	std::stringstream ss;
	ss << std::fixed << std::setprecision(1) << dSize << " " << suffixes[k];
	this->fileSize = ss.str();
	this->fileModDate = getDateString(fileStat.st_mtime, "%d.%m.%Y %H:%M:%S");
}

FileInfo::DirListingInfos::~DirListingInfos( void )
{ }


void    printCharOrRep(char c, bool printEndl)
{
	std::cout << "c: ";
	switch (c)
	{
	case 0x00:
		std::cout << std::setw(3) << "NUL";
		break ;
	case 0x01:
		std::cout << std::setw(3) << "SOH";
		break ;
	case 0x02:
		std::cout << std::setw(3) << "STX";
		break ;
	case 0x03:
		std::cout << std::setw(3) << "ETX";
		break ;
	case 0x04:
		std::cout << std::setw(3) << "EOT";
		break ;
	case 0x05:
		std::cout << std::setw(3) << "ENQ";
		break ;
	case 0x06:
		std::cout << std::setw(3) << "ACK";
		break ;
	case 0x07:
		std::cout << std::setw(3) << "BEL";
		break ;
	case 0x08:
		std::cout << std::setw(3) << "BS";
		break ;
	case 0x09:
		std::cout << std::setw(3) << "TAB";
		break ;
	case 0x0A:
		std::cout << std::setw(3) << "LF";
		break ;
	case 0x0B:
		std::cout << std::setw(3) << "VT";
		break ;
	case 0x0C:
		std::cout << std::setw(3) << "FF";
		break ;
	case 0x0D:
		std::cout << std::setw(3) << "CR";
		break ;
	case 0x0E:
		std::cout << std::setw(3) << "SO";
		break ;
	case 0x0F:
		std::cout << std::setw(3) << "SI";
		break ;
	case 0x10:
		std::cout << std::setw(3) << "DLE";
		break ;
	case 0x11:
		std::cout << std::setw(3) << "DC1";
		break ;
	case 0x12:
		std::cout << std::setw(3) << "DC2";
		break ;
	case 0x13:
		std::cout << std::setw(3) << "DC3";
		break ;
	case 0x14:
		std::cout << std::setw(3) << "DC4";
		break ;
	case 0x15:
		std::cout << std::setw(3) << "NAK";
		break ;
	case 0x16:
		std::cout << std::setw(3) << "SYN";
		break ;
	case 0x17:
		std::cout << std::setw(3) << "ETB";
		break ;
	case 0x18:
		std::cout << std::setw(3) << "CAN";
		break ;
	case 0x19:
		std::cout << std::setw(3) << "EM";
		break ;
	case 0x1A:
		std::cout << std::setw(3) << "SUB";
		break ;
	case 0x1B:
		std::cout << std::setw(3) << "Esc";
		break ;
	case 0x1C:
		std::cout << std::setw(3) << "FS";
		break ;
	case 0x1D:
		std::cout << std::setw(3) << "GS";
		break ;
	case 0x1E:
		std::cout << std::setw(3) << "RS";
		break ;
	case 0x1F:
		std::cout << std::setw(3) << "US";
		break ;
	case 0x20:
		std::cout << std::setw(3) << "SP";
		break ;
	default:
		std::cout << std::setw(2) << c;
		break;
	}
	if (printEndl)
		std::cout << std::endl;
}

