/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 08:54:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 09:06:17 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

void    printCharOrRep(char c)
{
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
}

HttpMessage::HttpMessage( void )
 : _status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _bits("011000"), _contentLength(0), _maxBodySize(4096), _chunkSize(0), _reqLinePos(REQLINE_PARSE_METHOD)
{ }

HttpMessage::HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter )
 : _status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _bits("011000"), _contentLength(0), _maxBodySize(4096), _chunkSize(0), _reqLinePos(REQLINE_PARSE_METHOD)
{ }

HttpMessage::~HttpMessage( void )
{ }

void HttpMessage::printMessage( void ) const
{
    std::cout << " --- REQUEST HEADER --- " << std::endl;
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
    }
    std::cout << std::endl;
    std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
    std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
    std::cout << " ---------------------------------------------------- " << std::endl;
}

std::string const & HttpMessage::getHeader( std::string const & key ) const
{
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        if (_headerMap[i].first == key) {
            return (_headerMap[i].second);
        }
    }
    return (ws_http::dummyValue);
}

bool    HttpMessage::hasHeader( std::string const & key ) const
{
    for (std::size_t i = 0; i != _headerMap.size(); ++i) {
        if (_headerMap[i].first == key)
            return (true);
    }
    return (false);
}

buff_t const &                  HttpMessage::getBody( void ) const 
{
    return (_buffer2);
}

HttpRequest::headers_t const &  HttpMessage::getHeaderVector( void ) const 
{
    return (_headerMap);
}

ws_http::statuscodes_t          HttpMessage::getStatus( void ) const
{
    return (_status);
}

ws_http::statuscodes_t  HttpMessage::parseHeaderLine(unsigned char c)
{
    if (!_bits[CR_FOUND] && c == '\r') {
        _bits[CR_FOUND] = true;
        if (_buffer1.empty()) {
            _bits[HEADER_DONE] = true;
        } else {
            _buffer2.erase(_buffer2.end() - _chunkSize, _buffer2.end());
            _chunkSize = 0;
            _bits[HEADER_VALUE_START_FOUND] = false;
            _bits[HEADER_PARSE_KEY] = true;
            insertHeader(_buffer1, _buffer2);
        }
    } else if (_bits[HEADER_PARSE_KEY] && c == ':') {
        _bits[HEADER_PARSE_KEY] = false;
    } else if (_bits[HEADER_PARSE_KEY] && (std::isalnum(c) || ws_http::headerTchar.find(c) != std::string::npos)) {
        _buffer1.push_back(std::tolower(c));
    } else if (!_bits[HEADER_PARSE_KEY] && (c >= 128 || (c >= 33 && c <= 126))) {
        _chunkSize = 0;
        _buffer2.push_back(c);
        if(!_bits[HEADER_VALUE_START_FOUND]) {
            _bits[HEADER_VALUE_START_FOUND] = true;
        }
    } else if ((c == ' ' || c == '\t')) {
        if (_bits[HEADER_VALUE_START_FOUND]) {
            _chunkSize++;
            _buffer2.push_back(c);
        }
    } else {
        return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

int getCharFromHex(unsigned char a)
{
    if (std::isdigit(a))
        return (a - '0');
    if (a >= 'A' && a <= 'F')
        return (a - ('A' + 10));
    if (a >= 'a' && a <= 'f')
        return (a - ('a' + 10));
    return (std::numeric_limits<unsigned char>::max());
}

void    HttpMessage::insertHeader(std::string const & key, std::string & buffer)
{
    _headerMap.push_back( std::make_pair(key, buffer));
    buffer.clear();
}

void    HttpMessage::insertHeader(std::string & key, std::string & buffer)
{
    _headerMap.push_back( std::make_pair(key, buffer));
    key.clear();
    buffer.clear();
}

ws_http::statuscodes_t  HttpMessage::setStatus( ws_http::statuscodes_t statusCode )
{
    _status = statusCode;
    if (_status >= ws_http::STATUS_400_BAD_REQUEST) {
        _headerMap.clear();
        _buffer1.clear();
        _buffer2.clear();
    }
    return (_status);
}

ws_http::statuscodes_t    HttpMessage::parseMethod(unsigned char c)
{
    if (!_buffer1.empty() && c == ' ') {
        _reqLinePos = REQLINE_PARSE_PATH;
        if (ws_http::methods.find(_buffer1) == ws_http::methods.end())
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        insertHeader("@method", _buffer1);
    } else if (isalpha(c)) {
        _buffer1.push_back(c);
    } else {
        return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

ws_http::statuscodes_t    HttpMessage::parsePath(unsigned char c)
{
    if (!_buffer1.empty() && (c == '?' || c == ' ')) {
        _headerMap.push_back(std::make_pair("@fulluri", _buffer1));
        insertHeader("@path", _buffer1);
        insertHeader("@pathdecoded", _buffer2);
        _reqLinePos = c == '?' ? REQLINE_PARSE_QUERY : REQLINE_PARSE_VERSION ;
    } else if (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos) {
        if (_buffer1.empty() && c != '/')
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        _buffer1.push_back(c);
        if (_buffer1.size() > 2 && _buffer1[_buffer1.size() - 3] == '%') {
            int decoded = 16 * getCharFromHex(_buffer1[_buffer1.size() - 2]) + getCharFromHex(_buffer1[_buffer1.size() - 1]);
            if (decoded <= std::numeric_limits<unsigned char>::max())
                _buffer2.push_back(decoded);
        } else {
            _buffer2.push_back(c);
        }
    } else {
        return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

ws_http::statuscodes_t    HttpMessage::parseQuery(unsigned char c)
{
    if (!_buffer1.empty() && c == ' ') {
        _reqLinePos = REQLINE_PARSE_VERSION;
        for (std::size_t i = 0; i != _headerMap.size(); ++i) {
            if (_headerMap[i].first == "@fulluri")
                _headerMap[i].second.append(_buffer1);
        }
        insertHeader("@query", _buffer1);
    } else if (_reqLinePos == REQLINE_PARSE_QUERY && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
        _buffer1.push_back(c);
    } else {
        return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

ws_http::statuscodes_t    HttpMessage::parseVersion(unsigned char c)
{
    if (!_bits[CR_FOUND] && c == '\r') {
        _bits[CR_FOUND] = true;
        _reqLinePos = REQLINE_DONE;
        if (ws_http::versions.find(_buffer1) == ws_http::versions.end())
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        insertHeader("@version", _buffer1);
    } else if (isalpha(c) || isdigit(c) || c == '.' || c == '/') {
        _buffer1.push_back(c);
    } else {
        return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    // std::cout << "bodyBuff: " << _buffer2 << std::endl;
    if (_status != ws_http::STATUS_UNDEFINED)
        return (_status);
    if (_bits[HEADER_DONE])
        return (parseBody(it_start, it_end));
    for (; _status < ws_http::STATUS_400_BAD_REQUEST && it_start != it_end; ++it_start) {

        // std::cout << "c: ";
        // printCharOrRep(*it_start);
        // std::cout << " ";
        // std::cout << std::endl;

        if (*it_start == '\n') {
            if (!_bits[CR_FOUND])
                return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
            if (_bits[HEADER_DONE])
                break ;
        } else if (_reqLinePos == REQLINE_DONE) {
            parseHeaderLine(*it_start);
        } else if (_reqLinePos == REQLINE_PARSE_VERSION) {
            parseVersion(*it_start);
        } else if (_reqLinePos == REQLINE_PARSE_QUERY) {
            parseQuery(*it_start);
        } else if (_reqLinePos == REQLINE_PARSE_PATH) {
            parsePath(*it_start);
        } else if (_reqLinePos == REQLINE_PARSE_METHOD) {
            parseMethod(*it_start);
        }
        if (*it_start != '\r')
            _bits[CR_FOUND] = false;
    }
    // std::cout << std::endl;
    it_start++;
    if (_bits[HEADER_DONE]) {
        if (getHeader("host").empty())
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        // if (_maxBodySizeGetter)
            // _maxBodySize = (*_maxBodySizeGetter)(getHeader("@pathdecoded"), getHeader("host"));
        _contentLength = getHeader("transfer-encoding") != "" ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
        parseBody(it_start, it_end);
    }
    return (_status);
}

ws_http::statuscodes_t  HttpMessage::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_contentLength >= 0) {
        long buffDist = std::distance(it_start, it_end);
        // std::cout << "buffsize: " << _buffer2.size() << " | buffDist: " << buffDist << std::endl;
        // std::cout << "buffsize + buffDist: " << _buffer2.size() + buffDist << " content-length: " << _contentLength << std::endl;
        if (static_cast<long>(_buffer2.size()) + buffDist > _contentLength)
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        if (static_cast<long>(_buffer2.size()) + buffDist > _maxBodySize)
            return (setStatus(ws_http::STATUS_413_PAYLOAD_TOO_LARGE));
        std::copy(it_start, it_end, std::back_inserter(_buffer2));
        if (static_cast<long>(_buffer2.size()) == _contentLength)
            return (setStatus(ws_http::STATUS_200_OK));
    } else {
        return (parseBodyChunked(it_start, it_end));
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}

ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    for (; it_start != it_end; ++it_start) {
        unsigned char c = *it_start;
        if (_chunkSize == 0 && c == '\r') {
            _bits[CR_FOUND] = true;
            continue;
        } 
        if (!_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
            _bits[PARSE_SIZE] = true;
            if (_bits[BODY_DONE])
                return (setStatus(ws_http::STATUS_200_OK));
        } else if (_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
            _chunkSize = strtol(_buffer1.c_str(), NULL, 16);
            _buffer1.clear();
            if (_chunkSize == 0)
                _bits[BODY_DONE] = true;
            _bits[PARSE_SIZE] = false;
        } else if ((unsigned long)_chunkSize > 0 && _buffer2.size() + 1 > (unsigned long)_maxBodySize) {
            return (setStatus(ws_http::STATUS_413_PAYLOAD_TOO_LARGE));
        } else if (!_bits[PARSE_SIZE] && _chunkSize > 0) {
            _buffer2.push_back(c);
            _chunkSize--;
        } else if (_bits[PARSE_SIZE] && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            _buffer1.push_back(c);
        } else {
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        }
        _bits[CR_FOUND] = false;
    }
    return (setStatus(ws_http::STATUS_UNDEFINED));
}































































// ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     std::cout << "bodyBuff: " << _buffer2 << std::endl;
//     if (_status != ws_http::STATUS_UNDEFINED)
//         return (_status);
//     if (_bits[HEADER_DONE])
//         return (parseBody(it_start, it_end));
//     for (; _status < ws_http::STATUS_400_BAD_REQUEST && !_bits[HEADER_DONE] && it_start != it_end; ++it_start) {
//         if (_bits[CR_FOUND] && *it_start == '\n') {
//             if (_bits[HEADER_DONE])
//                 break ;
//         } else if (_reqLinePos == REQLINE_DONE) {
//             parseHeaderLine(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_VERSION) {
//             parseVersion(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_QUERY) {
//             parseQuery(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_PATH) {
//             parsePath(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_METHOD) {
//             parseMethod(*it_start);
//         } else {
//             return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//         }
//         if (*it_start != '\r')
//             _bits[CR_FOUND] = false;
//     }
//     if (_bits[HEADER_DONE]) {
//         if (getHeader("host").empty())
//             return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//         // if (_maxBodySizeGetter)
//             // _maxBodySize = (*_maxBodySizeGetter)(getHeader("@pathdecoded"), getHeader("host"));
//         _contentLength = getHeader("transfer-encoding") != "" ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
//         parseBody(it_start, it_end);
//     }
//     return (_status);
// }


// void printBitset(std::bitset<6>& _bits)
// {
//     std::cout << "i: " << std::setw(2) << 0 << " | "; std::cout << std::setw(30) << "HEADER_DONE: "              ; std::cout << _bits[0] << std::endl;
//     std::cout << "i: " << std::setw(2) << 1 << " | "; std::cout << std::setw(30) << "BODY_DONE: "                ; std::cout << _bits[1] << std::endl;
//     std::cout << "i: " << std::setw(2) << 2 << " | "; std::cout << std::setw(30) << "CR_FOUND: "                 ; std::cout << _bits[2] << std::endl;
//     std::cout << "i: " << std::setw(2) << 3 << " | "; std::cout << std::setw(30) << "HEADER_PARSE_KEY: "         ; std::cout << _bits[3] << std::endl;
//     std::cout << "i: " << std::setw(2) << 4 << " | "; std::cout << std::setw(30) << "PARSE_SIZE: "               ; std::cout << _bits[4] << std::endl;
//     std::cout << "i: " << std::setw(2) << 5 << " | "; std::cout << std::setw(30) << "HEADER_VALUE_START_FOUND: " ; std::cout << _bits[5] << std::endl;
// }



// // HttpRequestLine::HttpRequestLine( void ) : status(ws_http::STATUS_UNDEFINED)
// // { }

// // HttpRequestLine::HttpRequestLine( ws_http::statuscodes_t statusCode ) : status(statusCode)
// // { }

// // HttpRequestLine::~HttpRequestLine( void )
// // { }

// HttpMessage::HttpMessage( void )
//  : _bits("011000"), _reqLinePos(REQLINE_PARSE_METHOD), status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// {

//     // bad because revers "1000011010000"
//     // good! "0000101100001"
//     _nbrs[0] = 0;
//     _nbrs[1] = 0;
//     std::cout << "bits: " << _bits << std::endl;

//     printBitset(_bits);
//     // std::cout << "i: " << std::setw(2) << HEADER_DONE               << " | "; std::cout << std::setw(30) << "HEADER_DONE: "              ; std::cout << _bits[HEADER_DONE]               << std::endl;
//     // std::cout << "i: " << std::setw(2) << BODY_DONE                 << " | "; std::cout << std::setw(30) << "BODY_DONE: "                ; std::cout << _bits[BODY_DONE]                 << std::endl;
//     // std::cout << "i: " << std::setw(2) << CR_FOUND                  << " | "; std::cout << std::setw(30) << "CR_FOUND: "                 ; std::cout << _bits[CR_FOUND]                  << std::endl;
//     // std::cout << "i: " << std::setw(2) << HEADER_PARSE_KEY          << " | "; std::cout << std::setw(30) << "HEADER_PARSE_KEY: "         ; std::cout << _bits[HEADER_PARSE_KEY]          << std::endl;
//     // std::cout << "i: " << std::setw(2) << PARSE_SIZE                << " | "; std::cout << std::setw(30) << "PARSE_SIZE: "               ; std::cout << _bits[PARSE_SIZE]                << std::endl;
//     // std::cout << "i: " << std::setw(2) << HEADER_VALUE_START_FOUND  << " | "; std::cout << std::setw(30) << "HEADER_VALUE_START_FOUND: " ; std::cout << _bits[HEADER_VALUE_START_FOUND]  << std::endl;
    
// }

// HttpMessage::HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter )
//  : _bits("011000"), _reqLinePos(REQLINE_PARSE_METHOD), status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// {
//     _nbrs[0] = 0;
//     _nbrs[1] = 0;
// }

// HttpMessage::~HttpMessage( void )
// { }

// void HttpMessage::printMessage( void ) const
// {
//     std::cout << " --- REQUEST HEADER --- " << std::endl;
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
//     }
//     std::cout << std::endl;
//     std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
//     std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
//     std::cout << " ---------------------------------------------------- " << std::endl;
// }

// std::string const & HttpMessage::getHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key) {
//             return (_headerMap[i].second);
//         }
//     }
//     return (ws_http::dummyValue);
// }

// bool    HttpMessage::hasHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key)
//             return (true);
//     }
//     return (false);
// }

// buff_t const &                  HttpMessage::getBody( void ) const          { return (this->_buffer2); }
// HttpRequest::headers_t const &  HttpMessage::getHeaderVector( void ) const  { return (this->_headerMap); }
// ws_http::statuscodes_t          HttpMessage::getStatus( void ) const        { return (this->status); }


// ws_http::statuscodes_t  HttpMessage::parseHeaderLine(unsigned char c)
// {
//     if (!_bits[CR_FOUND] && c == '\r') {
//         std::cout << " - crFound";
//         _bits[CR_FOUND] = true;
//         if (_buffer1.empty()) {
//             std::cout << " - HEADER END\n";
//             _bits[HEADER_DONE] = true;
//         } else {
//             std::cout << " - endline - ";
//             _buffer2.erase(_buffer2.end() - _nbrs[NBR_HEADER_WHITEPOS_LEN], _buffer2.end());
//             _nbrs[NBR_HEADER_WHITEPOS_LEN] = 0;
//             _bits[HEADER_VALUE_START_FOUND] = false;
//             _bits[HEADER_PARSE_KEY] = true;
//             std::cout << "parseHeaderLine end -> key: $" << _buffer1 << "$ - " << "value: $" << _buffer2 << "$" << std::endl;
//             insertHeader(_buffer1, _buffer2);
//             // this->_headerMap.push_back( std::make_pair(_buffer2, _buffer1));
//             // _buffer1.clear();
//             // _buffer2.clear();
//         }
//     } else if (_bits[HEADER_PARSE_KEY] && c == ':') {
//         std::cout << " - key end found\n";
//         _bits[HEADER_PARSE_KEY] = false;
//     } else if (_bits[HEADER_PARSE_KEY] && (std::isalnum(c) || ws_http::headerTchar.find(c) != std::string::npos)) {
//         std::cout << " - push key char\n";
//         _buffer1.push_back(std::tolower(c));
//         // std::cout << " - push key";
//         std::cout << " - push key: $" << _buffer1 << "$";
//     } else if (!_bits[HEADER_PARSE_KEY] && (c >= 128 || (c >= 33 && c <= 126))) {
//         _nbrs[NBR_HEADER_WHITEPOS_LEN] = 0;
//         _buffer2.push_back(c);
//         std::cout << " - push val";
//         // std::cout << " - push val: $" << _buffer2 << "$";
//         if(!_bits[HEADER_VALUE_START_FOUND]) {
//             std::cout << " - first non white\n";
//             _bits[HEADER_VALUE_START_FOUND] = true;
//         } else {
//             std::cout << "\n";
//         }
//     } else if ((c == ' ' || c == '\t')) {
//         std::cout << " - whitespace";
//         if (_bits[HEADER_VALUE_START_FOUND]) {
//             _nbrs[NBR_HEADER_WHITEPOS_LEN]++;
//             _buffer2.push_back(c);
//             std::cout << " - push whitespace, whitespace len: " << _nbrs[NBR_HEADER_WHITEPOS_LEN] << std::endl;
//         } else {
//             std::cout << std::endl;
//         }
//     } else {
//         std::cout << " - else -false\n" << std::endl;
//         return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//     }
//     return (setStatus(ws_http::STATUS_UNDEFINED));
// }

// int getCharFromHex(unsigned char a)
// {
//     if (std::isdigit(a))
//         return (a - '0');
//     if (a >= 'A' && a <= 'F')
//         return (a - ('A' + 10));
//     if (a >= 'a' && a <= 'f')
//         return (a - ('a' + 10));
//     return (std::numeric_limits<unsigned char>::max());
// }

// // void    HttpMessage::insertHeader(std::string const & key, std::string const & buffer)
// // {
// //     this->_headerMap.push_back( std::make_pair(key, buffer));
// // }

// void    HttpMessage::insertHeader(std::string const & key, std::string & buffer)
// {
//     this->_headerMap.push_back( std::make_pair(key, buffer));
//     buffer.clear();
// }

// void    HttpMessage::insertHeader(std::string & key, std::string & buffer)
// {
//     this->_headerMap.push_back( std::make_pair(key, buffer));
//     key.clear();
//     buffer.clear();
// }

// ws_http::statuscodes_t  HttpMessage::setStatus( ws_http::statuscodes_t statusCode )
// {
//     this->status = statusCode;
//     if (this->status >= ws_http::STATUS_400_BAD_REQUEST) {
//         this->_headerMap.clear();
//         this->_buffer1.clear();
//         this->_buffer2.clear();
//     }
//     return (this->status);
// }

// // bool    HttpMessage::parsePath(unsigned char c)
// // {
// //     if (!_buffer1.empty() && c == '?') {
// //         std::cout << "reqLine path end -> path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
// //         this->_headerMap.push_back( std::make_pair("@path", _buffer1));
// //         this->_headerMap.push_back( std::make_pair("@pathdecoded", _buffer2));
// //         _bits[REQLINE_HAS_QUERY] = true;
// //         _reqLinePos = REQLINE_PARSE_QUERY;
// //         _buffer1.clear();
// //         _buffer2.clear();
// //     } else if (!_buffer1.empty() && c == ' ') {
// //         std::cout << "reqLine path end -> path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
// //         this->_headerMap.push_back( std::make_pair("@path", _buffer1));
// //         this->_headerMap.push_back( std::make_pair("@pathdecoded", _buffer2));
// //         _reqLinePos = REQLINE_PARSE_VERSION;
// //         this->_headerMap.push_back( std::make_pair("@fulluri", _buffer1));
// //         _buffer1.clear();
// //         _buffer2.clear();
// //     } else if (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos) {
// //         if (_buffer1.empty() && c != '/')
// //             return (false);
// //         _buffer1.push_back(c);
// //         if (_buffer1.size() > 2 && _buffer1[_buffer1.size() - 3] == '%') {
// //             int decoded = 16 * getCharFromHex(_buffer1[_buffer1.size() - 2]) + getCharFromHex(_buffer1[_buffer1.size() - 1]);
// //             if (decoded <= std::numeric_limits<unsigned char>::max())
// //                 _buffer2.push_back(decoded);
// //         } else {
// //             _buffer2.push_back(c);
// //         }
// //         std::cout << "reqLine push path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
// //     } else {
// //         return (false);
// //     }
// //     return (true);
// // }

// ws_http::statuscodes_t    HttpMessage::parseMethod(unsigned char c)
// {
//     std::cout << " - parse method";
//     if (!_buffer1.empty() && c == ' ') {
//         std::cout << " - reqLine method end -> method: $" << _buffer1 << "$" << std::endl;
//         _reqLinePos = REQLINE_PARSE_PATH;
//         if (ws_http::methods.find(_buffer1) == ws_http::methods.end())
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//         else
//             insertHeader("@method", _buffer1);
//     } else if (isalpha(c)) {
//         _buffer1.push_back(c);
//         std::cout << " - reqLine push method: $" << _buffer1 << "$" << std::endl;
//     } else {
//         this->status = ws_http::STATUS_400_BAD_REQUEST;
//     }
//     return (setStatus(ws_http::STATUS_UNDEFINED));
// }

// ws_http::statuscodes_t    HttpMessage::parsePath(unsigned char c)
// {
//     std::cout << " - parse path";
//     if (!_buffer1.empty() && (c == '?' || c == ' ')) {
//         std::cout << " - reqLine path end -> path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//         insertHeader("@path", _buffer1);
//         insertHeader("@pathdecoded", _buffer2);
//         _reqLinePos = c == '?' ? REQLINE_PARSE_QUERY : REQLINE_PARSE_VERSION ;
//         this->_headerMap.push_back( std::make_pair("@fulluri", _buffer1));
//     } else if (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos) {
//         if (_buffer1.empty() && c != '/')
//             return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//         _buffer1.push_back(c);
//         if (_buffer1.size() > 2 && _buffer1[_buffer1.size() - 3] == '%') {
//             int decoded = 16 * getCharFromHex(_buffer1[_buffer1.size() - 2]) + getCharFromHex(_buffer1[_buffer1.size() - 1]);
//             if (decoded <= std::numeric_limits<unsigned char>::max())
//                 _buffer2.push_back(decoded);
//         } else {
//             _buffer2.push_back(c);
//         }
//         std::cout << " - reqLine push path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//     } else {
//         return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//     }
//     return (setStatus(ws_http::STATUS_UNDEFINED));
// }

// ws_http::statuscodes_t    HttpMessage::parseQuery(unsigned char c)
// {
//     std::cout << " - parse query";
//     if (!_buffer1.empty() && c == ' ') {
//         std::cout << " - reqLine query end -> query: $" << _buffer1 << "$" << std::endl;
//         _reqLinePos = REQLINE_PARSE_VERSION;
//         for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//             if (_headerMap[i].first == "@fulluri")
//                 _headerMap[i].second.append(_buffer1);
//         }
//         insertHeader("@query", _buffer1);
//     } else if (_reqLinePos == REQLINE_PARSE_QUERY && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         _buffer1.push_back(c);
//         std::cout << " - reqLine push query: $" << _buffer1 << "$" << std::endl;
//     } else {
//         return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//     }
//     return (setStatus(ws_http::STATUS_UNDEFINED));
// }

// ws_http::statuscodes_t    HttpMessage::parseVersion(unsigned char c)
// {
//     std::cout << " - parse Version";
//     // printBitset(_bits);
//     if (!_bits[CR_FOUND] && c == '\r') {
//         _bits[CR_FOUND] = true;
//         _reqLinePos = REQLINE_DONE;
//         std::cout << " - reqLine version end -> crFound and push. version: $" << _buffer1 << "$" << std::endl;
//         if (ws_http::versions.find(_buffer1) == ws_http::versions.end())
//             return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//         insertHeader("@version", _buffer1);
//     } else if (isalpha(c) || isdigit(c) || c == '.' || c == '/') {
//         _buffer1.push_back(c);
//         std::cout << " - reqLine push version: $" << _buffer1 << "$" << std::endl;
//     } else {
//         return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//     }
//     return (setStatus(ws_http::STATUS_UNDEFINED));
// }


// bool    HttpMessage::parseReqline(unsigned char c)
// {
//     // std::cout << "parse Reqline - ";

//     // std::cout << "\n --------- parse Reqline ---------" << std::endl;
//     if (c == '\r')
//         std::cout << "c: " << "\\r" << std::endl;
//     else
//         std::cout << "c: " << std::setw(2) << c << std::endl;
//     // std::cout << "parse method: " << printBool(_bits[REQLINE_PARSE_METHOD]) << std::endl;
//     // std::cout << "parse path: " << printBool(_bits[REQLINE_PARSE_PATH]) << std::endl;
//     // std::cout << "parse query: " << printBool(_bits[REQLINE_PARSE_QUERY]) << std::endl;
//     // std::cout << "parse version: " << printBool(_bits[REQLINE_PARSE_VERSION]) << std::endl;

//     if (!_bits[CR_FOUND] && c == '\r' && _reqLinePos == REQLINE_PARSE_VERSION ) {
//         std::cout << "reqLine version end -> crFound and push. version: $" << _buffer1 << "$" << std::endl;
//         _bits[CR_FOUND] = true;
//         if (ws_http::versions.find(_buffer1) == ws_http::versions.end())
//             return (false);
//         this->_headerMap.push_back( std::make_pair("@version", _buffer1));
//         _buffer1.clear();
//     } else if (!_buffer1.empty() && c == ' ' && _reqLinePos == REQLINE_PARSE_QUERY) {
//         std::cout << "reqLine query end -> query: $" << _buffer1 << "$" << std::endl;
//         // _bits[REQLINE_PARSE_VERSION] = true;
//         _reqLinePos = REQLINE_PARSE_VERSION;
//         // if (_bits[REQLINE_HAS_QUERY])
//         //     this->_headerMap.push_back( std::make_pair("@fulluri", getHeader("@path") + "?" + _buffer1));
//         this->_headerMap.push_back( std::make_pair("@query", _buffer1));
//     } else if (!_buffer1.empty() && (c == '?' || c == ' ') && _reqLinePos == REQLINE_PARSE_PATH) {
//         std::cout << "reqLine path end -> path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//         this->_headerMap.push_back( std::make_pair("@path", _buffer1));
//         this->_headerMap.push_back( std::make_pair("@pathdecoded", _buffer2));
//         if (c == '?') {
//             // _bits[REQLINE_HAS_QUERY] = true;
//             // _bits[REQLINE_PARSE_QUERY] = true;
//             _reqLinePos = REQLINE_PARSE_QUERY;
//         }
//         else {
//             _reqLinePos = REQLINE_PARSE_VERSION;
//             // _bits[REQLINE_PARSE_VERSION] = true;
//             this->_headerMap.push_back( std::make_pair("@fulluri", _buffer1));
//         }
//         _buffer1.clear();
//         _buffer2.clear();
//     } else if (!_buffer1.empty() && c == ' ' && _reqLinePos == REQLINE_PARSE_METHOD) {
//         std::cout << "reqLine method end -> method: $" << _buffer1 << "$" << std::endl;
//         // _bits[REQLINE_PARSE_PATH] = true;
//         _reqLinePos = REQLINE_PARSE_PATH;
//         this->_headerMap.push_back( std::make_pair("@method", _buffer1));
//         if (ws_http::methods.find(_buffer1) == ws_http::methods.end())
//             return (false);
//         _buffer1.clear();
//     } else if (_reqLinePos == REQLINE_PARSE_VERSION && isalpha(c)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push version: $" << _buffer1 << "$" << std::endl;
//     } else if (_reqLinePos == REQLINE_PARSE_QUERY && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push query: $" << _buffer1 << "$" << std::endl;
//     } else if (_reqLinePos == REQLINE_PARSE_PATH && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         if (_buffer1.empty() && c != '/')
//             return (false);
//         _buffer1.push_back(c);
//         if (_buffer1.size() > 2 && _buffer1[_buffer1.size() - 3] == '%') {
//             int decoded = 16 * getCharFromHex(_buffer1[_buffer1.size() - 2]) + getCharFromHex(_buffer1[_buffer1.size() - 1]);
//             if (decoded <= std::numeric_limits<unsigned char>::max())
//                 _buffer2.push_back(decoded);
//         } else {
//             _buffer2.push_back(c);
//         }
//         std::cout << "reqLine push path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//     } else if (_reqLinePos == REQLINE_PARSE_METHOD && isalpha(c)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push method: $" << _buffer1 << "$" << std::endl;
//     } else {
//         return (false);
//     }
//     return (true);
// }

// ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     std::cout << "status begin: " << ws_http::statuscodes.at(status) << std::endl;
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (_bits[HEADER_DONE]) {
//         this->status = this->parseBody(it_start, it_end);
//         return (this->status);
//     }
//     for (; status < ws_http::STATUS_400_BAD_REQUEST && !_bits[HEADER_DONE] && it_start != it_end; ++it_start) {

        
//         if (*it_start == '\r')
//             std::cout << "c: \\r";
//         else if (*it_start == '\n')
//             std::cout << "c: \\n";
//         else
//             std::cout << "c: " << std::setw(2) << *it_start;
//         std::cout << " | status: " << status;


//         if (_bits[CR_FOUND] && *it_start == '\n') {
//             std::cout << " - NEWLINE\n" << std::endl;
//             if (_bits[HEADER_DONE])
//                 break ;
//         } else if (_reqLinePos == REQLINE_DONE) {
//             parseHeaderLine(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_VERSION) {
//             parseVersion(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_QUERY) {
//             parseQuery(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_PATH) {
//             parsePath(*it_start);
//         } else if (_reqLinePos == REQLINE_PARSE_METHOD) {
//             parseMethod(*it_start);
//         } else {
//             std::cout << "ELSE!\n";
//             return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
//         }
//         if (*it_start != '\r')
//             _bits[CR_FOUND] = false;
//     }
//     printBitset(_bits);
//     std::cout << "\nafter parse headers: status" << status << std::endl;
//     std::cout << "size of message: " << sizeof(HttpMessage) << std::endl;
//     if (_bits[HEADER_DONE]) {
//         if (getHeader("host").empty()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//         } else {
//             // this->_headerDone = true;
//             if (this->_maxBodySizeGetter) {
//                 // this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
//             }
//             this->_contentLength = getHeader("transfer-encoding") != ""
//                 ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
//             std::cout << "content-lenght: " << this->_contentLength << std::endl;
//             this->status = this->parseBody(it_start, it_end);
//         }
//     }
//     return (this->status);
// }


// ws_http::statuscodes_t  HttpMessage::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     std::cout << "parse Body\n";
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         std::cout << "buff2 size: " << _buffer2.size() << " | dist: " << buffDist << " | buff + dist: " << static_cast<long>(_buffer2.size()) + buffDist << " | _contentLength: " << _contentLength << std::endl;
//         if (static_cast<long>(_buffer2.size()) + buffDist > _contentLength) {
//             std::cout << "static_cast<long>(_buffer2.size()) + buffDist > _contentLength" << std::endl;
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         if (static_cast<long>(_buffer2.size()) + buffDist > _maxBodySize) {
//             std::cout << "static_cast<long>(_buffer2.size()) + buffDist > _maxBodySize" << std::endl;
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         }
//         std::copy(it_start, it_end, std::back_inserter(_buffer2));
//         if (static_cast<long>(_buffer2.size()) == _contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     } else {
//         return (parseBodyChunked(it_start, it_end));
//     }
// }

// ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (_nbrHelper == 0 && c == '\r') {
//             _bits.flip(CR_FOUND);
//             // _crFound = true;
//             continue;
//         } 
//         if (!_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = true;
//             if (_bits[BODY_DONE])
//                 return (ws_http::STATUS_200_OK);
//         } else if (_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _nbrHelper = strtol(_buffer1.c_str(), NULL, 16);
//             _buffer1.clear();
//             if (_nbrHelper == 0)
//                 _bits.flip(BODY_DONE);
//                 // _bodyDone = true;
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = false;
//         } else if ((unsigned long)_nbrHelper > 0 && _buffer2.size() + 1 > (unsigned long)_maxBodySize) {
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         } else if (!_bits[PARSE_SIZE] && _nbrHelper > 0) {
//             _buffer2.push_back(c);
//             _nbrHelper--;
//         } else if (_bits[PARSE_SIZE] && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
//             _buffer1.push_back(c);
//         } else {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         _bits.flip(CR_FOUND);
//         // _crFound = false;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }




// // HttpRequestLine::HttpRequestLine( void ) : status(ws_http::STATUS_UNDEFINED)
// // { }

// // HttpRequestLine::HttpRequestLine( ws_http::statuscodes_t statusCode ) : status(statusCode)
// // { }

// // HttpRequestLine::~HttpRequestLine( void )
// // { }

// HttpMessage::HttpMessage( void )
//  : _bits("0000101100001"), status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// {

//     // bad because revers "1000011010000"
//     // good! "0000101100001"
//     _nbrs[0] = 0;
//     _nbrs[1] = 0;
//     std::cout << "bits: " << _bits << std::endl;

//     std::cout << "i: " << std::setw(2) << PARSE_REQLINE             << " | "; std::cout << std::setw(30) << "PARSE_REQLINE: "            ; std::cout << _bits[PARSE_REQLINE]             << std::endl;
//     std::cout << "i: " << std::setw(2) << LINE_DONE                 << " | "; std::cout << std::setw(30) << "LINE_DONE: "                ; std::cout << _bits[LINE_DONE]                 << std::endl;
//     std::cout << "i: " << std::setw(2) << HEADER_DONE               << " | "; std::cout << std::setw(30) << "HEADER_DONE: "              ; std::cout << _bits[HEADER_DONE]               << std::endl;
//     std::cout << "i: " << std::setw(2) << BODY_DONE                 << " | "; std::cout << std::setw(30) << "BODY_DONE: "                ; std::cout << _bits[BODY_DONE]                 << std::endl;
//     std::cout << "i: " << std::setw(2) << CR_FOUND                  << " | "; std::cout << std::setw(30) << "CR_FOUND: "                 ; std::cout << _bits[CR_FOUND]                  << std::endl;
//     std::cout << "i: " << std::setw(2) << HEADER_PARSE_KEY          << " | "; std::cout << std::setw(30) << "HEADER_PARSE_KEY: "         ; std::cout << _bits[HEADER_PARSE_KEY]          << std::endl;
//     std::cout << "i: " << std::setw(2) << PARSE_SIZE                << " | "; std::cout << std::setw(30) << "PARSE_SIZE: "               ; std::cout << _bits[PARSE_SIZE]                << std::endl;
//     std::cout << "i: " << std::setw(2) << HEADER_VALUE_START_FOUND  << " | "; std::cout << std::setw(30) << "HEADER_VALUE_START_FOUND: " ; std::cout << _bits[HEADER_VALUE_START_FOUND]  << std::endl;
//     std::cout << "i: " << std::setw(2) << REQLINE_PARSE_METHOD      << " | "; std::cout << std::setw(30) << "REQLINE_PARSE_METHOD: "     ; std::cout << _bits[REQLINE_PARSE_METHOD]      << std::endl;
//     std::cout << "i: " << std::setw(2) << REQLINE_PARSE_PATH        << " | "; std::cout << std::setw(30) << "REQLINE_PARSE_PATH: "       ; std::cout << _bits[REQLINE_PARSE_PATH]        << std::endl;
//     std::cout << "i: " << std::setw(2) << REQLINE_PARSE_QUERY       << " | "; std::cout << std::setw(30) << "REQLINE_PARSE_QUERY: "      ; std::cout << _bits[REQLINE_PARSE_QUERY]       << std::endl;
//     std::cout << "i: " << std::setw(2) << REQLINE_PARSE_VERSION     << " | "; std::cout << std::setw(30) << "REQLINE_PARSE_VERSION: "    ; std::cout << _bits[REQLINE_PARSE_VERSION]     << std::endl;
//     std::cout << "i: " << std::setw(2) << REQLINE_HAS_QUERY         << " | "; std::cout << std::setw(30) << "REQLINE_HAS_QUERY: "        ; std::cout << _bits[REQLINE_HAS_QUERY]         << std::endl;
    
// }

// HttpMessage::HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter )
//  : _bits("1000011010000"), status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// {
//     _nbrs[0] = 0;
//     _nbrs[1] = 0;
// }

// HttpMessage::~HttpMessage( void )
// { }

// void HttpMessage::printMessage( void ) const
// {
//     std::cout << " --- REQUEST HEADER --- " << std::endl;
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
//     }
//     std::cout << std::endl;
//     std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
//     std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
//     std::cout << " ---------------------------------------------------- " << std::endl;
// }

// std::string const & HttpMessage::getHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key) {
//             return (_headerMap[i].second);
//         }
//     }
//     return (ws_http::dummyValue);
// }

// bool    HttpMessage::hasHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key)
//             return (true);
//     }
//     return (false);
// }

// buff_t const &                  HttpMessage::getBody( void ) const          { return (this->_buffer2); }
// HttpRequest::headers_t const &  HttpMessage::getHeaderVector( void ) const  { return (this->_headerMap); }
// ws_http::statuscodes_t          HttpMessage::getStatus( void ) const        { return (this->status); }


// bool  HttpMessage::parseHeaderLine(unsigned char c)
// {
//     if (!_bits[CR_FOUND] && c == '\r') {
//         _bits[CR_FOUND] = true;
//         if (_buffer1.empty()) {
//             _bits[HEADER_DONE] = true;
//         } else {
//             _buffer1.erase(_buffer1.end() - _nbrs[NBR_HEADER_WHITEPOS_LEN], _buffer1.end());
//             _nbrs[NBR_HEADER_WHITEPOS_LEN] = 0;
//             this->_headerMap.push_back( std::make_pair(_buffer2, _buffer1));
//             _buffer1.clear();
//             _buffer2.clear();
//         }
//     } else if (_bits[HEADER_PARSE_KEY] && c == ':') {
//         _bits[HEADER_PARSE_KEY] = false;
//     } else if (_bits[HEADER_PARSE_KEY] && (std::isalnum(c) || ws_http::headerTchar.find(c) != std::string::npos)) {
//         _buffer1.push_back(std::tolower(c));
//     } else if (!_bits[HEADER_PARSE_KEY] && (c >= 128 || std::isprint(c))) {
//         _nbrs[NBR_HEADER_WHITEPOS_LEN] = 0;
//         _buffer2.push_back(c);
//         if(!_bits[HEADER_VALUE_START_FOUND])
//             _bits[HEADER_VALUE_START_FOUND] = true;
//     } else if (_bits[HEADER_VALUE_START_FOUND] && (c == ' ' || c == '\t')) {
//         _nbrs[NBR_HEADER_WHITEPOS_LEN]++;
//         _buffer2.push_back(c);
//     } else {
//         return (false);
//     }
//     return (true);
// }

// int getCharFromHex(unsigned char a)
// {
//     if (std::isdigit(a))
//         return (a - '0');
//     if (a >= 'A' && a <= 'F')
//         return (a - ('A' + 10));
//     if (a >= 'a' && a <= 'f')
//         return (a - ('a' + 10));
//     return (std::numeric_limits<unsigned char>::max());
// }


// // void    HttpMessage::pushValue(unsigned char delim)
// // {

// // }

// bool    HttpMessage::parseReqline(unsigned char c)
// {
//     // std::cout << "parse Reqline - ";

//     // std::cout << "\n --------- parse Reqline ---------" << std::endl;
//     if (c == '\r')
//         std::cout << "c: " << "\\r" << std::endl;
//     else
//         std::cout << "c: " << std::setw(2) << c << std::endl;
//     // std::cout << "parse method: " << printBool(_bits[REQLINE_PARSE_METHOD]) << std::endl;
//     // std::cout << "parse path: " << printBool(_bits[REQLINE_PARSE_PATH]) << std::endl;
//     // std::cout << "parse query: " << printBool(_bits[REQLINE_PARSE_QUERY]) << std::endl;
//     // std::cout << "parse version: " << printBool(_bits[REQLINE_PARSE_VERSION]) << std::endl;

//     if (!_bits[CR_FOUND] && c == '\r' && _bits[REQLINE_PARSE_VERSION] ) {
//         std::cout << "reqLine version end -> crFound and push. version: $" << _buffer1 << "$" << std::endl;
//         _bits[CR_FOUND] = true;
//         if (ws_http::versions.find(_buffer1) == ws_http::versions.end())
//             return (false);
//         this->_headerMap.push_back( std::make_pair("@version", _buffer1));
//         _buffer1.clear();
//     } else if (!_buffer1.empty() && c == ' ' && _bits[REQLINE_PARSE_QUERY]) {
//         std::cout << "reqLine query end -> query: $" << _buffer1 << "$" << std::endl;
//         _bits[REQLINE_PARSE_VERSION] = true;
//         if (_bits[REQLINE_HAS_QUERY])
//             this->_headerMap.push_back( std::make_pair("@fulluri", getHeader("@path") + "?" + _buffer1));
//         this->_headerMap.push_back( std::make_pair("@query", _buffer1));
//     } else if (!_buffer1.empty() && (c == '?' || c == ' ') && _bits[REQLINE_PARSE_PATH]) {
//         std::cout << "reqLine path end -> path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//         this->_headerMap.push_back( std::make_pair("@path", _buffer1));
//         this->_headerMap.push_back( std::make_pair("@pathdecoded", _buffer2));
//         if (c == '?') {
//             _bits[REQLINE_HAS_QUERY] = true;
//             _bits[REQLINE_PARSE_QUERY] = true;
//         }
//         else {
//             _bits[REQLINE_PARSE_VERSION] = true;
//             this->_headerMap.push_back( std::make_pair("@fulluri", _buffer1));
//         }
//         _buffer1.clear();
//         _buffer2.clear();
//     } else if (!_buffer1.empty() && c == ' ' && _bits[REQLINE_PARSE_METHOD]) {
//         std::cout << "reqLine method end -> method: $" << _buffer1 << "$" << std::endl;
//         _bits[REQLINE_PARSE_PATH] = true;
//         this->_headerMap.push_back( std::make_pair("@method", _buffer1));
//         if (ws_http::methods.find(_buffer1) == ws_http::methods.end())
//             return (false);
//         _buffer1.clear();
//     } else if (_bits[REQLINE_PARSE_VERSION] && isalpha(c)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push version: $" << _buffer1 << "$" << std::endl;
//     } else if (_bits[REQLINE_PARSE_QUERY] && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push query: $" << _buffer1 << "$" << std::endl;
//     } else if (_bits[REQLINE_PARSE_PATH] && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         if (_buffer1.empty() && c != '/')
//             return (false);
//         _buffer1.push_back(c);
//         if (_buffer1.size() > 2 && _buffer1[_buffer1.size() - 3] == '%') {
//             int decoded = 16 * getCharFromHex(_buffer1[_buffer1.size() - 2]) + getCharFromHex(_buffer1[_buffer1.size() - 1]);
//             if (decoded <= std::numeric_limits<unsigned char>::max())
//                 _buffer2.push_back(decoded);
//         } else {
//             _buffer2.push_back(c);
//         }
//         std::cout << "reqLine push path: $" << _buffer1 << "$ - " << "path decoded: $" << _buffer2 << "$" << std::endl;
//     } else if (_bits[REQLINE_PARSE_METHOD] && isalpha(c)) {
//         _buffer1.push_back(c);
//         std::cout << "reqLine push method: $" << _buffer1 << "$" << std::endl;
//     } else {
//         return (false);
//     }
//     return (true);
// }

// // ws_http::statuscodes_t    HttpMessage::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {

// //     // } else if (c == '\n') {
// //     //     if (_bits[HEADER_DONE])
// //     //         return (ws_http::STATUS_200_OK);
// //     //     std::string key = std::string(itStartKey, itEndKey);
// //     //     std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //     //     if (hasHeader(key))
// //     //         return (ws_http::STATUS_400_BAD_REQUEST);
// //     //     this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //     //     _bits[HEADER_PARSE_KEY] = true;
// //     //     itStartKey = it_start + 1;
// //     // } 
// //     // if (*it_start == '\r') {
// //     //     _bits[CR_FOUND] = true;
// //     // } else {
// //     //     if (*it_start == '\n')
// //     //         _bits[LINE_DONE] = true;
// //     //     else
// //     //         _firstLine.push_back(*it_start);
// //     //     _bits[CR_FOUND] = false;
// //     // }

// //     // for (; it_start != it_end; ++it_end) {
// //     //     if (!_bits[CR_FOUND] && *it_start == '\r')
// //     //         _bits[CR_FOUND] = true;
// //     //     else {
// //     //         if (!_bits[LINE_DONE] && *it_start == '\n') {
// //     //             _bits[LINE_DONE] = true;

// //     //         } else if (!_bits[LINE_DONE] && *it_start == '\n') {
// //     //             parseReqline(*it_start);
// //     //         } else if (!_bits[LINE_DONE]) {
// //     //             parseReqline(*it_start);
// //     //         } else if (!_bits[LINE_DONE]) {
// //     //             parseReqline(*it_start);
// //     //         }

// //     //         _bits[CR_FOUND] = false;
// //     //     }
// //     // }
// //         // if (!_bits[CR_FOUND] && *it_start == '\r') {
            
// //         // } else 

// // }


// ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (_bits[HEADER_DONE]) {
//         this->status = this->parseBody(it_start, it_end);
//         return (this->status);
//     }
//     bool res = true;
//     for (; res && !_bits[HEADER_DONE] && it_start != it_end; ++it_start) {
//         if (_bits[CR_FOUND] && *it_start == '\n') {
//             if (!_bits[LINE_DONE])
//                 _bits[LINE_DONE] = true;
//             if (_bits[HEADER_DONE]) {
//                 this->status = ws_http::STATUS_200_OK;
//                 break ;
//             }
//         } else if (!_bits[LINE_DONE]) {
//             res = parseReqline(*it_start);
//         } else if (_bits[LINE_DONE]) {
//             res = parseHeaderLine(*it_start);
//         } else {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//             break ;
//         }
//         if (*it_start != '\r')
//             _bits[CR_FOUND] = false;
//     }
//     if (status == ws_http::STATUS_UNDEFINED) {
//         if (getHeader("host").empty()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//         } else {
//             _bits.flip(HEADER_DONE);
//             // this->_headerDone = true;
//             if (this->_maxBodySizeGetter) {
//                 // this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
//                 this->_contentLength = getHeader("transfer-encoding") != ""
//                     ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
//                 this->status = this->parseBody(it_start, it_end);
//             }
//         }
//     }
//     return (this->status);
// }


// ws_http::statuscodes_t  HttpMessage::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         if (static_cast<long>(_buffer2.size()) + buffDist > _contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (static_cast<long>(_buffer2.size()) + buffDist > _maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(_buffer2));
//         if (static_cast<long>(_buffer2.size()) == _contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     } else {
//         return (parseBodyChunked(it_start, it_end));
//     }
// }

// ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (_nbrHelper == 0 && c == '\r') {
//             _bits.flip(CR_FOUND);
//             // _crFound = true;
//             continue;
//         } 
//         if (!_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = true;
//             if (_bits[BODY_DONE])
//                 return (ws_http::STATUS_200_OK);
//         } else if (_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _nbrHelper = strtol(_buffer1.c_str(), NULL, 16);
//             _buffer1.clear();
//             if (_nbrHelper == 0)
//                 _bits.flip(BODY_DONE);
//                 // _bodyDone = true;
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = false;
//         } else if ((unsigned long)_nbrHelper > 0 && _buffer2.size() + 1 > (unsigned long)_maxBodySize) {
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         } else if (!_bits[PARSE_SIZE] && _nbrHelper > 0) {
//             _buffer2.push_back(c);
//             _nbrHelper--;
//         } else if (_bits[PARSE_SIZE] && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
//             _buffer1.push_back(c);
//         } else {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         _bits.flip(CR_FOUND);
//         // _crFound = false;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }
































































// HttpRequestLine::HttpRequestLine( void ) : status(ws_http::STATUS_UNDEFINED)
// { }

// HttpRequestLine::HttpRequestLine( ws_http::statuscodes_t statusCode ) : status(statusCode)
// { }

// HttpRequestLine::~HttpRequestLine( void )
// { }

// HttpMessage::HttpMessage( void )
//  :  status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// { }

// HttpMessage::HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter )
//  :  status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _contentLength(0), _maxBodySize(4096), _nbrHelper(0)
//     // _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// { }

// HttpMessage::~HttpMessage( void )
// { }

// void HttpMessage::printMessage( void ) const
// {
//     std::cout << " --- REQUEST HEADER --- " << std::endl;
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
//     }
//     std::cout << std::endl;
//     std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
//     std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
//     std::cout << " ---------------------------------------------------- " << std::endl;
// }

// std::string const & HttpMessage::getHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key) {
//             return (_headerMap[i].second);
//         }
//     }
//     return (ws_http::dummyValue);
// }

// bool    HttpMessage::hasHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key)
//             return (true);
//     }
//     return (false);
// }

// buff_t const &                  HttpMessage::getBody( void ) const          { return (this->_bodyBuffer); }
// HttpRequest::headers_t const &  HttpMessage::getHeaderVector( void ) const  { return (this->_headerMap); }
// ws_http::statuscodes_t          HttpMessage::getStatus( void ) const        { return (this->status); }


// // bool    HttpMessage::addHeader(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //    const buff_t::const_iterator itStartKey = it_start;
// //     const buff_t::const_iterator itEndKey = std::find(it_start, it_end, ':');
// //     if (itEndKey == it_end)
// //         return (false);
// //     for (; it_start != itEndKey; ++it_start) {
// //         if (!std::isalnum(*it_start) && ws_http::headerTchar.find(*it_start) == std::string::npos)
// //             return (false);
// //     }
// //     it_start++;
// //     while (*it_start == ' ' || *it_start == '\t')
// //         it_start++;
// //     while (*it_end == ' ' || *it_end == '\t')
// //         it_end--;
// //     for (buff_t::const_iterator it = it_start; it != it_end; ++it) {
// //         unsigned char c = *it;
// //         if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
// //             continue ;
// //         return (false);
// //     }
// //     std::string key = std::string(itStartKey, itEndKey);
// //     std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //     if (hasHeader(key))
// //         return (false);
// //     _headerMap.push_back(std::make_pair(key, std::string(it_start, it_end)));
// //     return (true);
// // }


// // ws_http::statuscodes_t    HttpMessage::parseHeaders( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// // {
// //     while (itStart < itEnd) {
// //         if (!_reqLineDone) {
// //             _firstLine.push_back(*itStart);
// //             continue ;
// //         }
// //         const buff_t::const_iterator itStartKey = itStart;
// //         while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
// //             itStart++;
// //         if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
// //             return (ws_http::STATUS_400_BAD_REQUEST);
// //         const buff_t::const_iterator itEndKey = itStart;
// //         itStart++;
// //         while (*itStart == ' ' || *itStart == '\t')
// //             itStart++;
// //         const buff_t::const_iterator itStartValue = itStart;
// //         buff_t::const_iterator lastNonWhitespace = itStart;
// //         bool crFound = false;
// //         for (;itStart != itEnd; ++itStart) {
// //             unsigned char c = *itStart;
// //             if (c == '\n' && crFound) {
// //                 _reqLineDone = true;
// //                 // _nbrHelper++;
// //                 std::string key = std::string(itStartKey, itEndKey);
// //                 std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //                 if (hasHeader(key))
// //                     return (ws_http::STATUS_400_BAD_REQUEST);
// //                 this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //                 itStart++;
// //                 break ;
// //             }
// //             if (c == '\r' && !crFound) {
// //                 crFound = true;
// //             } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
// //                 lastNonWhitespace = itStart;
// //                 crFound = false;
// //             } else if ((c == ' ' || c == '\t') && !crFound) {
// //                 crFound = false;
// //             } else {
// //                 return (ws_http::STATUS_400_BAD_REQUEST);
// //             }
// //         }
// //     }
// //     return (ws_http::STATUS_UNDEFINED);
// // }


// // ws_http::statuscodes_t  HttpMessage::parseHeadersBuf(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     buff_t::const_iterator itStartKey, itEndKey, itStartValue, lastNonWhitespace;
// //     itStartKey = itEndKey = itStartValue = lastNonWhitespace = it_start;
// //     // bool parseKey = true, valueStartFound = false, crFound = false;


// //     for (; it_start != it_end; ++it_start) {
// //         unsigned char c = *it_start;
// //         if (!_bits[LINE_DONE]) {
// //             _firstLine.push_back(c);
// //         } else if (_bits[HEADER_PARSE_KEY]) {
// //             if (c == ':') {
// //                 _bits.flip(HEADER_PARSE_KEY);
// //                 // parseKey = false;
// //                 itEndKey = it_start;
// //             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
// //                 return (false);
// //             }
// //         } else if (_bits[CR_FOUND] && c == '\n') {
// //             std::string key = std::string(itStartKey, itEndKey);
// //             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //             if (hasHeader(key))
// //                 return (false);
// //             this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //             _bits.flip(HEADER_PARSE_KEY);
// //             // // parseKey = true;
// //             _bits.flip(HEADER_VALUE_START_FOUND);
// //             // valueStartFound = false;
// //             _bits.flip(CR_FOUND);
// //             // crFound = false;
// //             itStartKey = it_start + 1;
// //         } else if (c >= 128 || std::isprint(c)) {
// //             _bits.flip(CR_FOUND);
// //             // crFound = false;
// //             if (!_bits[HEADER_VALUE_START_FOUND]) {
// //                 itStartValue = it_start;
// //                 _bits.flip(HEADER_VALUE_START_FOUND);
// //                 // valueStartFound = true;
// //             } else {
// //                 lastNonWhitespace = it_start;
// //             }
// //         } else if(c == '\r') {
// //             _bits.flip(CR_FOUND);
// //             // crFound = true;
// //         } else if (c == ' ' || c == '\t') {
// //             _bits.flip(CR_FOUND);
// //             // crFound = false;
// //         } else {
// //             return (false);
// //         }
// //     }
// //     return (true);
// //     // buff_t::const_iterator itStartKey, itEndKey, itStartValue, lastNonWhitespace;
// //     // itStartKey = itEndKey = itStartValue = lastNonWhitespace = it_start;
// //     // bool parseKey = true, valueStartFound = false, crFound = false;


// //     // for (; it_start != it_end; ++it_start) {
// //     //     unsigned char c = *it_start;
// //     //     if (!_reqLineDone) {
// //     //         _firstLine.push_back(c);
// //     //     } else if (parseKey) {
// //     //         if (c == ':') {
// //     //             _bits.flip(HEADER_PARSE_KEY);
// //     //             parseKey = false;
// //     //             itEndKey = it_start;
// //     //         } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
// //     //             return (false);
// //     //         }
// //     //     } else if (crFound && c == '\n') {
// //     //         std::string key = std::string(itStartKey, itEndKey);
// //     //         std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //     //         if (hasHeader(key))
// //     //             return (false);
// //     //         this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //     //         parseKey = true;
// //     //         valueStartFound = false;
// //     //         crFound = false;
// //     //         itStartKey = it_start + 1;
// //     //     } else if (c >= 128 || std::isprint(c)) {
// //     //         crFound = false;
// //     //         if (!valueStartFound) {
// //     //             itStartValue = it_start;
// //     //             valueStartFound = true;
// //     //         } else {
// //     //             lastNonWhitespace = it_start;
// //     //         }
// //     //     } else if(c == '\r') {
// //     //         crFound = true;
// //     //     } else if (c == ' ' || c == '\t') {
// //     //         crFound = false;
// //     //     } else {
// //     //         return (false);
// //     //     }
// //     // }
// //     // return (true);
// // }



// /*  OK - prev version - needs testing and storing of prev data */
// // ws_http::statuscodes_t  HttpMessage::parseHeadersBuf(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     buff_t::const_iterator itStartKey, itEndKey, itStartValue, lastNonWhitespace;
// //     itStartKey = itEndKey = itStartValue = lastNonWhitespace = it_start;
// //     for (; it_start != it_end; ++it_start) {
// //         unsigned char c = *it_start;
// //         if (c == '\r') {
// //             if (_bits[CR_FOUND])
// //                 return (ws_http::STATUS_400_BAD_REQUEST);
// //             if (!_bits[LINE_DONE])
// //                 _bits[LINE_DONE] = true;
// //             if (itStartKey == it_start)
// //                 _bits[HEADER_DONE] = true;
// //             _bits[CR_FOUND] = true;
// //         } else {
// //             if (!_bits[LINE_DONE]) {
// //                 _firstLine.push_back(c);
// //             } else if (_bits[HEADER_PARSE_KEY] && c == ':') {
// //                 _bits[HEADER_PARSE_KEY] = false;
// //                 itEndKey = it_start;
// //                 itStartValue = itEndKey + 1;
// //             } else if (_bits[HEADER_PARSE_KEY] && !std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
// //                 return (ws_http::STATUS_400_BAD_REQUEST);
// //             } else if (c == '\n') {
// //                 if (_bits[HEADER_DONE])
// //                     return (ws_http::STATUS_200_OK);
// //                 std::string key = std::string(itStartKey, itEndKey);
// //                 std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //                 if (hasHeader(key))
// //                     return (ws_http::STATUS_400_BAD_REQUEST);
// //                 this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //                 _bits[HEADER_PARSE_KEY] = true;
// //                 itStartKey = it_start + 1;
// //             } else if (c >= 128 || std::isprint(c)) {
// //                 if (itStartValue == itEndKey + 1)
// //                     itStartValue = it_start;
// //                 lastNonWhitespace = it_start;
// //             } else if (!(c == ' ' || c == '\t')) {
// //                 return (ws_http::STATUS_400_BAD_REQUEST);
// //             }
// //             _bits[CR_FOUND] = false;
// //         }
// //     }
// //     std::copy()
// //     return (ws_http::STATUS_UNDEFINED);
// //     // if (!_bits[HEADER_VALUE_START_FOUND]) {
// //     //     if (itStartValue == itEndKey + 1)
// //     //         itStartValue = it_start;
// //     //     _bits[HEADER_VALUE_START_FOUND] = true;            
// //     // } else {
// //     //     lastNonWhitespace = it_start;
// //     // }
// // }

// ws_http::statuscodes_t  HttpMessage::parseHeadersBuf(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator itStartKey, itEndKey, itStartValue, lastNonWhitespace;
//     itStartKey = itEndKey = itStartValue = lastNonWhitespace = it_start;
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (c == '\r') {
//             if (_bits[CR_FOUND])
//                 return (ws_http::STATUS_400_BAD_REQUEST);
//             if (!_bits[LINE_DONE])
//                 _bits[LINE_DONE] = true;
//             if (itStartKey == it_start)
//                 _bits[HEADER_DONE] = true;
//             _bits[CR_FOUND] = true;
//         } else {
//             if (!_bits[LINE_DONE]) {
//                 _firstLine.push_back(c);
//             } else if (_bits[HEADER_PARSE_KEY] && c == ':') {
//                 _bits[HEADER_PARSE_KEY] = false;
//                 itEndKey = it_start;
//                 itStartValue = itEndKey + 1;
//             } else if (_bits[HEADER_PARSE_KEY] && !std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 return (ws_http::STATUS_400_BAD_REQUEST);
//             } else if (c == '\n') {
//                 if (_bits[HEADER_DONE])
//                     return (ws_http::STATUS_200_OK);
//                 std::string key = std::string(itStartKey, itEndKey);
//                 std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//                 if (hasHeader(key))
//                     return (ws_http::STATUS_400_BAD_REQUEST);
//                 this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
//                 _bits[HEADER_PARSE_KEY] = true;
//                 itStartKey = it_start + 1;
//             } else if (c >= 128 || std::isprint(c)) {
//                 if (itStartValue == itEndKey + 1)
//                     itStartValue = it_start;
//                 lastNonWhitespace = it_start;
//             } else if (!(c == ' ' || c == '\t')) {
//                 return (ws_http::STATUS_400_BAD_REQUEST);
//             }
//             _bits[CR_FOUND] = false;
//         }
//     }
//     std::copy()
//     return (ws_http::STATUS_UNDEFINED);
// }



// HttpRequestLine HttpMessage::getRequestLine( void ) const
// {
//     HttpRequestLine reqLine;
//     buff_t::const_iterator itSep1 = _firstLine.end(), itSep2 = _firstLine.end();
//     itSep1 = std::find(_firstLine.begin(), _firstLine.end(), ' ');
//     if (itSep1 != _firstLine.end())
//         itSep2 = std::find(itSep1 + 1, _firstLine.end(), ' ');
//     if (itSep1 == _firstLine.end() || itSep2 == _firstLine.end())
//         return (HttpRequestLine(ws_http::STATUS_400_BAD_REQUEST));
//     reqLine.method = std::string(_firstLine.begin(), itSep1);
//     reqLine.fullUri = std::string(itSep1 + 1, itSep2);
//     reqLine.version = std::string(itSep2 + 1, _firstLine.end());
//     if (ws_http::versions.find(reqLine.version) == ws_http::versions.end()
//         || ws_http::methods.find(reqLine.method) == ws_http::methods.end())
//         return (HttpRequestLine(ws_http::STATUS_400_BAD_REQUEST));
//     std::size_t queryPos = reqLine.fullUri.find_first_of('?');
//     reqLine.path = reqLine.fullUri.substr(0, queryPos);
//     if (queryPos != std::string::npos)
//         reqLine.query = reqLine.fullUri.substr(queryPos + 1, std::string::npos);
//     std::string const & str = reqLine.path;
//     reqLine.pathDecoded.reserve(reqLine.path.size());
//     std::string & res = reqLine.pathDecoded;
//     for (std::size_t pos = 0, k = 0; pos < str.size(); pos++, k++) {
//         if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
//             || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
//             int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
//             int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
//             res.push_back(a + b);
//             pos += 2;
//         } else if (str[pos]) {
//             res.push_back(str[pos]);
//         }
//     }
//     reqLine.status = ws_http::STATUS_200_OK;
//     return (reqLine);
// }

// int getCharFromHex(unsigned char a)
// {
//     if (std::isdigit(a))
//         return (a - '0');
//     if (a >= 'A' && a <= 'F')
//         return (a - ('A' + 10));
//     if (a >= 'a' && a <= 'f')
//         return (a - ('a' + 10));
//     return (std::numeric_limits<unsigned char>::max());
// }

// bool    HttpMessage::parseReqline(char c, std::string & str, std::string & str2)
// {
//     if (_bits[REQLINE_PARSE_PATH] && !_buffer.empty() && c == '?') {
//         this->_headerMap.push_back( std::make_pair("@version", _buffer));
//     } else if (!_buffer.empty() && c == ' ') {
//         if (_bits[REQLINE_PARSE_VERSION]) {
//             this->_headerMap.push_back( std::make_pair("@version", _buffer));
//         } else if (_bits[REQLINE_PARSE_QUERY]) {
//             _bits[REQLINE_PARSE_VERSION] = true;
//             this->_headerMap.push_back( std::make_pair("@query", _buffer));
//         } else if (_bits[REQLINE_PARSE_PATH]) {
//             this->_headerMap.push_back( std::make_pair("@path", _buffer));
//         } else if (_bits[REQLINE_PARSE_METHOD]) {
//             _bits[REQLINE_PARSE_PATH] = true;
//             this->_headerMap.push_back( std::make_pair("@method", _buffer));
//         }
//         _buffer.clear();
//     } else if (_bits[REQLINE_PARSE_VERSION] && isalpha(c)) {
//         _buffer.push_back(c);
//     } else if (_bits[REQLINE_PARSE_QUERY] && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         _buffer.push_back(c);
//     } else if (_bits[REQLINE_PARSE_PATH] && (isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos)) {
//         if (_buffer.empty() && c != '/')
//             return (false);
//         _buffer.push_back(c);
//         if (_buffer.size() > 2 && _buffer[_buffer.size() - 3] == '%') {
//             int decoded = 16 * getCharFromHex(_buffer[_buffer.size() - 2]) + getCharFromHex(_buffer[_buffer.size() - 1]);
//             if (decoded <= std::numeric_limits<unsigned char>::max())
//                 _bodyBuffer.push_back(decoded);
//         }
//     } else if (_bits[REQLINE_PARSE_METHOD] && isalpha(c)) {
//         _buffer.push_back(c);
//     } else {
//         return (false);
//     }
// }

// // bool    HttpMessage::parseReqline(char c, std::string & str, std::string & str2)
// // {
// //     if (!_bits[SPACE_FOUND] && c == ' ') {
// //         _bits[SPACE_FOUND] = true;
// //     } else if (_bits[REQLINE_PARSE_VERSION]) {

// //     } else if (_bits[REQLINE_PARSE_QUERY]) {
// //         if (!isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) == std::string::npos)
// //             return (false);
// //         _buffer.push_back(c);
// //     } else if (_bits[REQLINE_PARSE_PATH]) {
// //         if ((_buffer.empty() && c != '/') || !isalnum(c) || ws_http::uriPathOrQueryAllowed.find(c) == std::string::npos)
// //             return (false);
// //         _buffer.push_back(c);
// //         if (_buffer.size() > 2 && _buffer[_buffer.size() - 3] == '%') {
// //             int decoded = 16 * getCharFromHex(_buffer[_buffer.size() - 2]) + getCharFromHex(_buffer[_buffer.size() - 1]);
// //             if (decoded <= std::numeric_limits<unsigned char>::max())
// //                 _bodyBuffer.push_back(decoded);
// //         }
// //     } else if (_bits[REQLINE_PARSE_METHOD]) {
// //         if (!isalpha(c))
// //             return (false);
// //         str.push_back(c);
// //     }
// // }

// ws_http::statuscodes_t    HttpMessage::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     for (; !_bits[LINE_DONE] && it_start != it_end; ++it_end) {
//         if (*it_start == '\r') {
//             _bits[CR_FOUND] = true;
//         } else {
//             if (*it_start == '\n')
//                 _bits[LINE_DONE] = true;
//             else
//                 _firstLine.push_back(*it_start);
//             _bits[CR_FOUND] = false;
//         }
//     }

//     // for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
//     //     pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     //     if (!addHeader(start, pos)) {
//     //         return (ws_http::STATUS_400_BAD_REQUEST);
//     //     }
//     // }
//     return (ws_http::STATUS_UNDEFINED);
// }


// ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (_bits[HEADER_DONE]) {
//         this->status = this->parseBody(it_start, it_end);
//         return (this->status);
//     }
//     // std::size_t buffSize = this->_buffer.size();
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::const_iterator end = std::search(it_start, it_end, ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//     // this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
//     if (end == this->_buffer.end())
//         return (ws_http::STATUS_UNDEFINED);
//     this->status = this->parseHeaders(this->_buffer.begin(), end);
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (getHeader("host").empty()) {
//         this->status = ws_http::STATUS_400_BAD_REQUEST;
//     } else {
//         _bits.flip(HEADER_DONE);
//         // this->_headerDone = true;
//         if (this->_maxBodySizeGetter) {
//             // this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
//             this->_contentLength = getHeader("transfer-encoding") != ""
//                 ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
//             this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
//         }
//         this->_buffer.clear();
//     }
//     return (this->status);
// }

// // ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     if (this->_headerDone) {
// //         this->status = this->parseBody(it_start, it_end);
// //         return (this->status);
// //     }
// //     // std::size_t buffSize = this->_buffer.size();
// //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //     buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
// //         ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// //     // this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
// //     if (end == this->_buffer.end())
// //         return (ws_http::STATUS_UNDEFINED);
// //     this->status = this->parseHeaders(this->_buffer.begin(), end);
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     if (getHeader("host").empty()) {
// //         this->status = ws_http::STATUS_400_BAD_REQUEST;
// //     } else {
// //         this->_headerDone = true;
// //         if (this->_maxBodySizeGetter) {
// //             this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
// //             this->_contentLength = getHeader("transfer-encoding") != ""
// //                 ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
// //             this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// //         }
// //         this->_buffer.clear();
// //     }
// //     return (this->status);
// // }


// ws_http::statuscodes_t  HttpMessage::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         if (static_cast<long>(_bodyBuffer.size()) + buffDist > _contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (static_cast<long>(_bodyBuffer.size()) + buffDist > _maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
//         if (static_cast<long>(_bodyBuffer.size()) == _contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     } else {
//         return (parseBodyChunked(it_start, it_end));
//     }
// }



// ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (_nbrHelper == 0 && c == '\r') {
//             _bits.flip(CR_FOUND);
//             // _crFound = true;
//             continue;
//         } 
//         if (!_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = true;
//             if (_bits[BODY_DONE])
//                 return (ws_http::STATUS_200_OK);
//         } else if (_bits[PARSE_SIZE] && _bits[CR_FOUND] && c == '\n') {
//             _nbrHelper = strtol(_buffer.c_str(), NULL, 16);
//             _buffer.clear();
//             if (_nbrHelper == 0)
//                 _bits.flip(BODY_DONE);
//                 // _bodyDone = true;
//             _bits.flip(PARSE_SIZE);
//             // _parseSize = false;
//         } else if ((unsigned long)_nbrHelper > 0 && _bodyBuffer.size() + 1 > (unsigned long)_maxBodySize) {
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         } else if (!_bits[PARSE_SIZE] && _nbrHelper > 0) {
//             _bodyBuffer.push_back(c);
//             _nbrHelper--;
//         } else if (_bits[PARSE_SIZE] && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
//             _buffer.push_back(c);
//         } else {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         _bits.flip(CR_FOUND);
//         // _crFound = false;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// // ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     for (; it_start != it_end; ++it_start) {
// //         unsigned char c = *it_start;
// //         if (_nbrHelper == 0 && c == '\r') {
// //             _crFound = true;
// //             continue;
// //         } 
// //         if (!_parseSize && _crFound && c == '\n') {
// //             _parseSize = true;
// //             if (_bodyDone)
// //                 return (ws_http::STATUS_200_OK);
// //         } else if (_parseSize && _crFound && c == '\n') {
// //             _nbrHelper = strtol(_buffer.c_str(), NULL, 16);
// //             _buffer.clear();
// //             if (_nbrHelper == 0)
// //                 _bodyDone = true;
// //             _parseSize = false;
// //         } else if ((unsigned long)_nbrHelper > 0 && _bodyBuffer.size() + 1 > (unsigned long)_maxBodySize) {
// //             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
// //         } else if (!_parseSize && _nbrHelper > 0) {
// //             _bodyBuffer.push_back(c);
// //             _nbrHelper--;
// //         } else if (_parseSize && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
// //             _buffer.push_back(c);
// //         } else {
// //             return (ws_http::STATUS_400_BAD_REQUEST);
// //         }
// //         _crFound = false;
// //     }
// //     return (ws_http::STATUS_UNDEFINED);
// // }













































// HttpRequestLine::HttpRequestLine( void ) : status(ws_http::STATUS_UNDEFINED)
// { }

// HttpRequestLine::HttpRequestLine( ws_http::statuscodes_t statusCode ) : status(statusCode)
// { }

// HttpRequestLine::~HttpRequestLine( void )
// { }

// HttpMessage::HttpMessage( void )
//  :  status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _contentLength(0), _maxBodySize(4096), _nbrHelper(0),
//     _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// { }

// HttpMessage::HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter )
//  :  status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _contentLength(0), _maxBodySize(4096), _nbrHelper(0),
//     _headerDone(false), _crFound(false), _parseSize(true), _bodyDone(false)
// { }

// HttpMessage::~HttpMessage( void )
// { }

// void HttpMessage::printMessage( void ) const
// {
//     std::cout << " --- REQUEST HEADER --- " << std::endl;
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         std::cout << _headerMap[i].first << ": " << _headerMap[i].second << " | size header-value: " << _headerMap[i].second.size() << std::endl;
//     }
//     std::cout << std::endl;
//     std::cout << " --- REQUEST BODY Size: " << getBody().size() << "--- " << std::endl;
//     std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
//     std::cout << " ---------------------------------------------------- " << std::endl;
// }

// std::string const & HttpMessage::getHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key) {
//             return (_headerMap[i].second);
//         }
//     }
//     return (ws_http::dummyValue);
// }

// bool    HttpMessage::hasHeader( std::string const & key ) const
// {
//     for (std::size_t i = 0; i != _headerMap.size(); ++i) {
//         if (_headerMap[i].first == key)
//             return (true);
//     }
//     return (false);
// }

// buff_t const &                  HttpMessage::getBody( void ) const          { return (this->_bodyBuffer); }
// HttpRequest::headers_t const &  HttpMessage::getHeaderVector( void ) const  { return (this->_headerMap); }
// ws_http::statuscodes_t          HttpMessage::getStatus( void ) const        { return (this->status); }


// // bool    HttpMessage::addHeader(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //    const buff_t::const_iterator itStartKey = it_start;
// //     const buff_t::const_iterator itEndKey = std::find(it_start, it_end, ':');
// //     if (itEndKey == it_end)
// //         return (false);
// //     for (; it_start != itEndKey; ++it_start) {
// //         if (!std::isalnum(*it_start) && ws_http::headerTchar.find(*it_start) == std::string::npos)
// //             return (false);
// //     }
// //     it_start++;
// //     while (*it_start == ' ' || *it_start == '\t')
// //         it_start++;
// //     while (*it_end == ' ' || *it_end == '\t')
// //         it_end--;
// //     for (buff_t::const_iterator it = it_start; it != it_end; ++it) {
// //         unsigned char c = *it;
// //         if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
// //             continue ;
// //         return (false);
// //     }
// //     std::string key = std::string(itStartKey, itEndKey);
// //     std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //     if (hasHeader(key))
// //         return (false);
// //     _headerMap.push_back(std::make_pair(key, std::string(it_start, it_end)));
// //     return (true);
// // }


// // ws_http::statuscodes_t    HttpMessage::parseHeaders( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// // {
// //     while (itStart < itEnd) {
// //         if (!_reqLineDone) {
// //             _firstLine.push_back(*itStart);
// //             continue ;
// //         }
// //         const buff_t::const_iterator itStartKey = itStart;
// //         while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
// //             itStart++;
// //         if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
// //             return (ws_http::STATUS_400_BAD_REQUEST);
// //         const buff_t::const_iterator itEndKey = itStart;
// //         itStart++;
// //         while (*itStart == ' ' || *itStart == '\t')
// //             itStart++;
// //         const buff_t::const_iterator itStartValue = itStart;
// //         buff_t::const_iterator lastNonWhitespace = itStart;
// //         bool crFound = false;
// //         for (;itStart != itEnd; ++itStart) {
// //             unsigned char c = *itStart;
// //             if (c == '\n' && crFound) {
// //                 _reqLineDone = true;
// //                 // _nbrHelper++;
// //                 std::string key = std::string(itStartKey, itEndKey);
// //                 std::transform(key.begin(), key.end(), key.begin(), ::tolower);
// //                 if (hasHeader(key))
// //                     return (ws_http::STATUS_400_BAD_REQUEST);
// //                 this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
// //                 itStart++;
// //                 break ;
// //             }
// //             if (c == '\r' && !crFound) {
// //                 crFound = true;
// //             } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
// //                 lastNonWhitespace = itStart;
// //                 crFound = false;
// //             } else if ((c == ' ' || c == '\t') && !crFound) {
// //                 crFound = false;
// //             } else {
// //                 return (ws_http::STATUS_400_BAD_REQUEST);
// //             }
// //         }
// //     }
// //     return (ws_http::STATUS_UNDEFINED);
// // }


// bool    HttpMessage::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator itStartKey, itEndKey, itStartValue, lastNonWhitespace;
//     itStartKey = itEndKey = itStartValue = lastNonWhitespace = it_start;
//     bool parseKey = true, valueStartFound = false, crFound = false;


//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (!_reqLineDone) {
//             _firstLine.push_back(c);
//         } else if (parseKey) {
//             if (c == ':') {
//                 parseKey = false;
//                 itEndKey = it_start;
//             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 return (false);
//             }
//         } else if (crFound && c == '\n') {
//             std::string key = std::string(itStartKey, itEndKey);
//             std::transform(key.begin(), key.end(), key.begin(), ::tolower);
//             if (hasHeader(key))
//                 return (false);
//             this->_headerMap.push_back( std::make_pair(key, std::string(itStartValue, lastNonWhitespace + 1)));
//             parseKey = true;
//             valueStartFound = false;
//             crFound = false;
//             itStartKey = it_start + 1;
//         } else if (c >= 128 || std::isprint(c)) {
//             crFound = false;
//             if (!valueStartFound) {
//                 itStartValue = it_start;
//                 valueStartFound = true;
//             } else {
//                 lastNonWhitespace = it_start;
//             }
//         } else if(c == '\r') {
//             crFound = true;
//         } else if (c == ' ' || c == '\t') {
//             crFound = false;
//         } else {
//             return (false);
//         }
//     }
//     return (true);
// }



// HttpRequestLine HttpMessage::getRequestLine( void ) const
// {
//     HttpRequestLine reqLine;
//     buff_t::const_iterator itSep1 = _firstLine.end(), itSep2 = _firstLine.end();
//     itSep1 = std::find(_firstLine.begin(), _firstLine.end(), ' ');
//     if (itSep1 != _firstLine.end())
//         itSep2 = std::find(itSep1 + 1, _firstLine.end(), ' ');
//     if (itSep1 == _firstLine.end() || itSep2 == _firstLine.end())
//         return (HttpRequestLine(ws_http::STATUS_400_BAD_REQUEST));
//     reqLine.method = std::string(_firstLine.begin(), itSep1);
//     reqLine.fullUri = std::string(itSep1 + 1, itSep2);
//     reqLine.version = std::string(itSep2 + 1, _firstLine.end());
//     if (ws_http::versions.find(reqLine.version) == ws_http::versions.end()
//         || ws_http::methods.find(reqLine.method) == ws_http::methods.end())
//         return (HttpRequestLine(ws_http::STATUS_400_BAD_REQUEST));
//     std::size_t queryPos = reqLine.fullUri.find_first_of('?');
//     reqLine.path = reqLine.fullUri.substr(0, queryPos);
//     if (queryPos != std::string::npos)
//         reqLine.query = reqLine.fullUri.substr(queryPos + 1, std::string::npos);
//     std::string const & str = reqLine.path;
//     reqLine.pathDecoded.reserve(reqLine.path.size());
//     std::string & res = reqLine.pathDecoded;
//     for (std::size_t pos = 0, k = 0; pos < str.size(); pos++, k++) {
//         if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
//             || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
//             int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
//             int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
//             res.push_back(a + b);
//             pos += 2;
//         } else if (str[pos]) {
//             res.push_back(str[pos]);
//         }
//     }
//     reqLine.status = ws_http::STATUS_200_OK;
//     return (reqLine);
// }

// ws_http::statuscodes_t    HttpMessage::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
    

//     for (buff_t::const_iterator start = pos; start < it_end; start = pos + ws_http::crlf.size()) {
//         pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (!addHeader(start, pos)) {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


// ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (this->_headerDone) {
//         this->status = this->parseBody(it_start, it_end);
//         return (this->status);
//     }
//     // std::size_t buffSize = this->_buffer.size();
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::const_iterator end = std::search(it_start, it_end, ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//     // this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
//     if (end == this->_buffer.end())
//         return (ws_http::STATUS_UNDEFINED);
//     this->status = this->parseHeaders(this->_buffer.begin(), end);
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (this->status);
//     if (getHeader("host").empty()) {
//         this->status = ws_http::STATUS_400_BAD_REQUEST;
//     } else {
//         this->_headerDone = true;
//         if (this->_maxBodySizeGetter) {
//             this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
//             this->_contentLength = getHeader("transfer-encoding") != ""
//                 ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
//             this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
//         }
//         this->_buffer.clear();
//     }
//     return (this->status);
// }

// // ws_http::statuscodes_t  HttpMessage::parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     if (this->_headerDone) {
// //         this->status = this->parseBody(it_start, it_end);
// //         return (this->status);
// //     }
// //     // std::size_t buffSize = this->_buffer.size();
// //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //     buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
// //         ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// //     // this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
// //     if (end == this->_buffer.end())
// //         return (ws_http::STATUS_UNDEFINED);
// //     this->status = this->parseHeaders(this->_buffer.begin(), end);
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     if (getHeader("host").empty()) {
// //         this->status = ws_http::STATUS_400_BAD_REQUEST;
// //     } else {
// //         this->_headerDone = true;
// //         if (this->_maxBodySizeGetter) {
// //             this->_maxBodySize = (*_maxBodySizeGetter)(getHeader(pathDecoded), getHeader("host"));
// //             this->_contentLength = getHeader("transfer-encoding") != ""
// //                 ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
// //             this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// //         }
// //         this->_buffer.clear();
// //     }
// //     return (this->status);
// // }


// ws_http::statuscodes_t  HttpMessage::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         if (static_cast<long>(_bodyBuffer.size()) + buffDist > _contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (static_cast<long>(_bodyBuffer.size()) + buffDist > _maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
//         if (static_cast<long>(_bodyBuffer.size()) == _contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     } else {
//         return (parseBodyChunked(it_start, it_end));
//     }
// }

// ws_http::statuscodes_t  HttpMessage::parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     for (; it_start != it_end; ++it_start) {
//         unsigned char c = *it_start;
//         if (_nbrHelper == 0 && c == '\r') {
//             _crFound = true;
//             continue;
//         } 
//         if (!_parseSize && _crFound && c == '\n') {
//             _parseSize = true;
//             if (_bodyDone)
//                 return (ws_http::STATUS_200_OK);
//         } else if (_parseSize && _crFound && c == '\n') {
//             _nbrHelper = strtol(_buffer.c_str(), NULL, 16);
//             _buffer.clear();
//             if (_nbrHelper == 0)
//                 _bodyDone = true;
//             _parseSize = false;
//         } else if ((unsigned long)_nbrHelper > 0 && _bodyBuffer.size() + 1 > (unsigned long)_maxBodySize) {
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         } else if (!_parseSize && _nbrHelper > 0) {
//             _bodyBuffer.push_back(c);
//             _nbrHelper--;
//         } else if (_parseSize && (std::isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
//             _buffer.push_back(c);
//         } else {
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         }
//         _crFound = false;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }


