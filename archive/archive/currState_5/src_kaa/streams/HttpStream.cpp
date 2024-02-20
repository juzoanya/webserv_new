/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStream.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 12:53:44 by mberline          #+#    #+#             */
/*   Updated: 2024/02/07 17:17:04 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpStream.hpp"

SocketStreambuf::SocketStreambuf(int socketFd) : _socketFd(socketFd), _isReadable(false), _isWriteable(false), _lastReadByte(0)
{
    this->setg(this->ibuffer.data(), this->ibuffer.data(), this->ibuffer.data());
    this->setp(this->obuffer.data(), this->obuffer.data() + this->obuffer.size());
    std::cout << "constructor" << this->_socketFd << std::endl;
}

SocketStreambuf::~SocketStreambuf(void)
{
}

std::streamsize SocketStreambuf::xsputn(const std::streambuf::char_type *s, std::streamsize n)
{
    std::cout << "xsputn: " << std::string(s, n) << std::endl;
    std::copy(s, s + n, std::back_inserter(this->obuffer));

    this->setp(this->obuffer.data() + this->obuffer.size(), this->obuffer.data() + this->obuffer.size());

    // this->setp(this->obuffer.data(), this->obuffer.data() + this->obuffer.size());

    this->setg(this->obuffer.data(), this->obuffer.data(), this->obuffer.data() + this->obuffer.size());

    // std::cout << "buffer has: " << std::string(this->obuffer.begin(), this->obuffer.end()) << std::endl;
    // std::cout << "put pointer at: " << this->pptr() << std::endl;
    return (n);
}

std::streambuf::int_type SocketStreambuf::overflow(std::streambuf::int_type ch)
{
    std::cout << "overflow: " << (char)ch << std::endl;
    std::cout << "buffer has: " << std::string(this->obuffer.begin(), this->obuffer.end()) << std::endl;
    if (ch != traits_type::eof())
    {
        this->obuffer.push_back(ch);
        setp(obuffer.data() + obuffer.size(), obuffer.data() + obuffer.size()); // Put-Pointer aktualisieren
        this->setg(this->obuffer.data(), this->obuffer.data(), this->obuffer.data() + this->obuffer.size());
        // setp(obuffer.data(), obuffer.data() + obuffer.size()); // Put-Pointer aktualisieren
        return (ch);
    }
    return (1);
}

void SocketStreambuf::setReadable(void) { this->_isReadable = true; }
void SocketStreambuf::setWriteable(void) { this->_isWriteable = true; }

std::vector<char>::size_type SocketStreambuf::defCapa = 4096;

int SocketStreambuf::readSock(void)
{

    int readByte = recv(this->_socketFd, this->ibuffer.data(), defCapa, 0);
    if (readByte > 0)
    {
        this->setg(this->ibuffer.data(), this->ibuffer.data(), this->ibuffer.data() + readByte);
    }
    return (readByte);
}

std::streamsize SocketStreambuf::showmanyc( void )
{
    if (!this->_isReadable)
        return (-1);
    return (1);
}

std::streambuf::int_type SocketStreambuf::underflow(void)
{
    if (this->ibuffer.capacity() != defCapa)
        this->ibuffer.reserve(defCapa);

    if (!this->_isReadable)
        return (traits_type::eof());

    int readByte;
    if (this->_usedCapa < defCapa) {
        readByte = recv(this->_socketFd, this->ibuffer.data() + this->_usedCapa, defCapa - this->_usedCapa, 0);
    } else {
        this->_usedCapa = 0;
        readByte = recv(this->_socketFd, this->ibuffer.data(), defCapa / 2, 0);
    }
    if (readByte == -1)
        throw std::runtime_error(strerror(errno));
    if (readByte == 0)
        return (traits_type::eof());
    this->setg(this->ibuffer.data() + this->_usedCapa, this->ibuffer.data() + this->_usedCapa, this->ibuffer.data() + this->_usedCapa + readByte);
    this->_usedCapa += readByte;
    return (*this->egptr());
}

// std::streambuf::int_type SocketStreambuf::uflow( void )
// {

// }

std::streamsize SocketStreambuf::xsgetn(std::streambuf::char_type *s, std::streamsize n)
{
    // int_type c;
    // std::streamsize i = 0;
    // bool crFound = false;
    // while (i < n) {
    //     traits_type::char_type c = traits_type::to_char_type(this->sgetc());
    //     if (c == traits_type::eof()) {
    //         break ;
    //     } else if (c == '\r' && !crFound) {
    //         crFound = true;
    //     } else if (c == '\n' && crFound) {
    //         break ;
    //     }

        
    // }
    // return (0);

    // int_type c;
    // std::streamsize i = 0;
    // bool crFound = false;
    // while (i < n) {
    //     if (this->gptr() < this->egptr()) {
    //         if (*this->gptr() == '\r') {
    //             crFound = true;
    //         } else if (*this->gptr() == '\n' && crFound) {

    //         } else {
    //             crFound = false;
    //             *s = *this->gptr();
    //         }

    //         s++;
    //         i++;
    //         this->gbump(1);
    //     } else if ((c = uflow()) != traits_type::eof()) {
    //         // *__s = traits_type::to_char_type(__c);
    //         // ++__s;
    //         // ++__i;
    //     } else {
    //         break;
    //     }
    // }
    // return (i);

    std::cout << this->_lastReadByte;
    std::cout << "xsgetn: " << std::string(s, n) << "n: " << n << std::endl;
    std::copy(this->obuffer.begin(), this->obuffer.begin() + n, s);
    // this->setp(&this->obuffer[0], &this->obuffer[n]);
    // this->setg(&this->obuffer[0], &this->obuffer[n], &this->obuffer[n]);
    std::cout << "buffer has: " << std::string(this->obuffer.begin(), this->obuffer.end()) << std::endl;
    std::cout << "put pointer at: " << this->pptr() << std::endl;
    return (n);
}


SocketStream::SocketStream(int socketFd) : std::iostream(&sb), sb(socketFd)
{
}

SocketStream::~SocketStream(void)
{
}

void SocketStream::readFromSocket(void)
{
    // this->sb.sputn()
    std::cout << "read from socket";
}

void SocketStream::sendToSocket(void)
{
}

void SocketStream::setReadable(void) { this->sb.setReadable(); }
void SocketStream::setWriteable(void) { this->sb.setWriteable(); }

std::ostream & SocketStream::flush(void)
{
    std::cout << "flush!\n";
    return (*this);
}

void    SocketStream::readHttpLine( std::string & str )
{
    try
    {
        str.clear();
        bool crFound = false;
        while (true) {
            std::streambuf::int_type i = this->sb.sgetc();
            std::streamsize s;
            if (i == traits_type::eof()) {
                // EOF
            }
            std::streambuf::char_type ch = traits_type::to_char_type(i);
            if (ch == '\r') {
                crFound = true;
            } else if (ch == '\n' && crFound) {
                break ;
            }
            str.push_back(ch);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

#include "socket_helper.hpp"

void asse(int ret)
{
    if (ret == -1)
    {
        std::cout << strerror(errno) << std::endl;
        exit(1);
    }
}

int main(void)
{

    std::string a;

    std::istringstream iss ("123, moin wie geht es dir?");
    // iss >> std::skipws >> a >> b >> c;
    // iss >> std::skipws >> a;
    iss >> a;
    std::cout << a << std::endl;
    // std::cout << (int)a << std::endl;

    iss.seekg(0);
    // iss >> std::noskipws >> a >> b >> c;
    iss >> std::noskipws >> a;
    std::cout << a << std::endl;
    iss >> std::noskipws >> a;
    std::cout << a << std::endl;
    iss >> std::noskipws >> a;
    std::cout << a << std::endl;
    iss >> std::noskipws >> a;
    std::cout << a << std::endl;

    // int server_socket = make_socket("localhost", "8080", 10, false);
    // asse(server_socket);

    // int client_socket = accept(server_socket, NULL, NULL);
    // asse(client_socket);

    // SocketStream stream(client_socket);
    // stream.readFromSocket();



    // std::string line;

    // while (stream >> line)
    //     std::cout << "line from stream: " << line << std::endl;



    // SocketStream stream(1);
    // stream << "alpha, " << "beta, " << "gamma";

    // std::stringstream ss;
    // ss << 1 << 2 << 3;
    // stream << ss.rdbuf();

    // std::string jo;

    // while (stream >> jo)
    //     std::cout << jo << std::endl;

    // std::cout << "in avail: " << stream.rdbuf()->in_avail() << std::endl;
    // std::cout << "im stream buffer is: " << stream.rdbuf() << std::endl;

    return (0);
}
