/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 09:39:29 by mberline          #+#    #+#             */
/*   Updated: 2024/02/10 21:28:59 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"


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
    is.read(buffer.data() + currBufferSize, streamSize);
}


WsIpPort::WsIpPort( void )
{ }

WsIpPort::WsIpPort( struct sockaddr_in const & addr ) : _address(addr)
{
	this->_portStr = toStr(htons(addr.sin_port));
    // std::cout << "htons port int: " << htons(addr.sin_port) << std::endl;
    // std::cout << "port int: " << (addr.sin_port) << std::endl;
	std::stringstream ss;
    uint32_t ip_addr = ntohl(addr.sin_addr.s_addr);
    ss << ((ip_addr >> 24) & 0xFF) << ".";
    ss << ((ip_addr >> 16) & 0xFF) << ".";
    ss << ((ip_addr >> 8) & 0xFF) << ".";
    ss << (ip_addr & 0xFF);
	this->_ipStr = ss.str();
    // std::cout << "wsIpPort ip: " << this->_ipStr << " | port: " << this->_portStr << std::endl;
}

// WsIpPort::WsIpPort( std::string const & ip, std::string const & port ) : _ipStr(ip), _portStr(port)
// {
// 	memset(&this->_address, 0, sizeof(struct sockaddr_in));
// 	uint32_t ipInt = 0;
// 	unsigned int ipPartNbr;
//     // std::size_t pos_start = 0, pos = 0;
//     const char* ipPtr = ip.data();
//     char* rest;
// 	for (int i = 0; i != 4; ++i) {
//         ipPartNbr = strtol(ipPtr, &rest, 10);
//         if ((i == 3 && *rest != 0) || (i < 3 && *rest != '.') || (i < 3 && !std::isdigit(*(rest + 1))))
//             throw::std::runtime_error("invalid ip address");
//         ipPtr = rest + 1;
// 		ipInt |= (ipPartNbr << (i * 8));
// 	}
//     unsigned int portInt = strtol(port.c_str(), &rest, 10);
//     if (portInt == 0 || *rest != 0)
//         throw::std::runtime_error("invalid port");
//     this->_address.sin_family = AF_INET;
//     this->_address.sin_addr.s_addr = ipInt;
//     this->_address.sin_port = htons(portInt);
// }

WsIpPort::WsIpPort( std::string const & ipStr, std::string const & portStr ) : _ipStr(ipStr), _portStr(portStr)
{
    // std::cout << "wsIpPort ip: " << ipStr << " | port: " << portStr << std::endl;
    memset(&this->_address, 0, sizeof(struct sockaddr_in));
	uint32_t ip = 0;
	unsigned int ipPartNbr;
    // std::size_t pos_start = 0, pos = 0;
    const char* ipPtr = ipStr.data();
    char* rest;
	for (int i = 0; i != 4; ++i) {
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
    // this->_address.sin_addr.s_addr = htonl(ip);
    this->_address.sin_port = htons(port);
    // std::cout << "htons port: " << htons(port) << std::endl;
    // std::cout << "port: " << (port) << std::endl;
    // std::cout << "htons ip: " << htons(ip) << std::endl;
    // std::cout << "ip: " << (ip) << std::endl;
}

WsIpPort::~WsIpPort( void )
{ }

bool WsIpPort::operator==( WsIpPort const & rhs ) const { return (rhs._ipStr == this->_ipStr && rhs._portStr == this->_portStr); }
bool WsIpPort::operator()( HttpServer const & rhs ) const { return (rhs.serverIpPort.getIpStr() == this->_ipStr && rhs.serverIpPort.getPortStr() == this->_portStr); }
bool WsIpPort::operator()( HttpServer const * rhs ) const { return (rhs->serverIpPort.getIpStr() == this->_ipStr && rhs->serverIpPort.getPortStr() == this->_portStr); }
std::string const & WsIpPort::getIpStr( void ) const { return (this->_ipStr); }
std::string const & WsIpPort::getPortStr( void ) const { return (this->_portStr); }
struct sockaddr_in const & WsIpPort::getSockaddrIn( void ) const { return (this->_address); }



FileInfos::FileInfos( std::string const & filePath, std::string const & requestPathDecoded )
 : _fileType(0), _fileError(0), _requestPathDecoded(requestPathDecoded)
//  : _fileType(0), _fileError(0), _filePath(filePath), _requestPathDecoded(requestPathDecoded)
{
    std::cout << "---- FileInfo -- constructor" << std::endl;
    std::cout << " ---> filePath: " << filePath << std::endl;
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0 && access(filePath.c_str(), R_OK) == 0) {
        this->_fileType = fileStat.st_mode;
    } else {
        this->_fileError = errno;
        return ;
    }
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

FileInfos::~FileInfos( void )
{ }

mode_t FileInfos::getMode( void ) const { return (this->_fileType); }

int FileInfos::getError( void ) const { return (this->_fileError); }

std::string const & FileInfos::checkContainedFile( std::vector<std::string> const & files )
{
    for (std::size_t i = 0; i != this->_dirListingInfos.size(); ++i) {
        std::vector<std::string>::const_iterator it = std::find(files.begin(), files.end(), this->_dirListingInfos[i].fileName);
        if (it != files.end())
            return (*it);
    }
    return (dummyStr);
}

int     FileInfos::setDirListing( std::stringstream & ss )
{
    if (this->_dirListingInfos.empty())
        return (-1);
    if (this->_requestPathDecoded[this->_requestPathDecoded.size() - 1] != '/')
        this->_requestPathDecoded.push_back('/');
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
    std::stringstream dirstream(this->_requestPathDecoded);
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
        ss << "<td><a href=\"" << this->_requestPathDecoded << this->_dirListingInfos[i].fileName << "\">" << this->_dirListingInfos[i].fileName << "</a></td>";
        ss << "<td>" << this->_dirListingInfos[i].fileSize << "</td>";
        ss << "<td>" << this->_dirListingInfos[i].fileModDate << "</td>";
        ss << "</tr>";
    }
    ss << "</tbody>";
    ss << "</table>" "</body>" "</html>";
    return (0);
}



FileInfos::DirListingInfos::DirListingInfos( std::string const & name, struct stat & fileStat )
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

FileInfos::DirListingInfos::~DirListingInfos( void )
{ }

