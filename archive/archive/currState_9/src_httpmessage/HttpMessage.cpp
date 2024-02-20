/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 08:54:28 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 21:50:00 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

const std::size_t         HttpMessage::readBufferSize = 4096;
std::vector<char>   HttpMessage::readBuffer = std::vector<char>(HttpMessage::readBufferSize);
const std::string         HttpMessage::method = "@method";
const std::string         HttpMessage::path = "@path";
const std::string         HttpMessage::pathDecoded = "@pathdecoded";
const std::string         HttpMessage::query = "@query";
const std::string         HttpMessage::fullUri = "@fulluri";
const std::string         HttpMessage::version = "@version";

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
 :  _status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(NULL), _bits("011000000"),
    _contentLength(0), _maxBodySize(4096), _chunkSize(0), _reqLinePos(REQLINE_PARSE_METHOD), headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
{ }

HttpMessage::HttpMessage( IOnHttpMessageBodySize* maxBodySizeGetter )
 :  _status(ws_http::STATUS_UNDEFINED), _maxBodySizeGetter(maxBodySizeGetter), _bits("011000000"),
    _contentLength(0), _maxBodySize(4096), _chunkSize(0), _reqLinePos(REQLINE_PARSE_METHOD), headerPtr(NULL), headerSize(0), bodyPtr(NULL), bodySize(0)
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
    std::cout << " --- REQUEST BODY Size: " << getBody().size() << " --- " << std::endl;
    // std::cout << std::string(getBody().begin(), getBody().begin() + getBody().size()) << std::endl;
    // std::cout << getBody() << std::endl;
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
            insertHeaderClearKeyValue(_buffer1, _buffer2);
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

void    HttpMessage::insertHeaderClearValue(std::string const & key, buff_t & buffer)
{
    if (hasHeader(key))
        return ;
    _headerMap.push_back(std::make_pair(key, std::string(buffer.begin(), buffer.end())));
    buffer.clear();

    // if (!hasHeader(key)) {
    //     setHeader(key, buffer);
    //     buffer.clear();
    // }
}

void    HttpMessage::insertHeaderClearKeyValue(buff_t & key, buff_t & buffer)
{
    std::string keyStr = std::string(key.begin(), key.end());
    if (hasHeader(keyStr))
        return ;
    _headerMap.push_back(std::make_pair(keyStr, std::string(buffer.begin(), buffer.end())));
    buffer.clear();
    key.clear();

    // if (!hasHeader(key)) {
    //     setHeader(key, buffer);
    //     buffer.clear();
    //     key.clear();
    // }
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
        // if (ws_http::methods_rev.find(_buffer1) == ws_http::methods_rev.end())
        if (ws_http::methods_rev.find(std::string(_buffer1.begin(), _buffer1.end())) == ws_http::methods_rev.end())
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        insertHeaderClearValue("@method", _buffer1);
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
        // _headerMap.push_back(std::make_pair("@fulluri", _buffer1));
        _headerMap.push_back(std::make_pair("@fulluri", std::string(_buffer1.begin(), _buffer1.end())));
        insertHeaderClearValue("@path", _buffer1);
        insertHeaderClearValue("@pathdecoded", _buffer2);
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
                std::copy(_buffer1.begin(), _buffer1.end(), std::back_inserter(_headerMap[i].second));
        }
        insertHeaderClearValue("@query", _buffer1);
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
        // if (ws_http::versions_rev.find(_buffer1) == ws_http::versions_rev.end())
        if (ws_http::versions_rev.find(std::string(_buffer1.begin(), _buffer1.end())) == ws_http::versions_rev.end())
            return (setStatus(ws_http::STATUS_400_BAD_REQUEST));
        insertHeaderClearValue("@version", _buffer1);
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
        if (_maxBodySizeGetter)
            _maxBodySize = (*_maxBodySizeGetter)(getHeader("@pathdecoded"), getHeader("host"));
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
            _chunkSize = strtol(std::string(_buffer1.begin(), _buffer1.end()).c_str(), NULL, 16);
            // _chunkSize = strtol(_buffer1.c_str(), NULL, 16);
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



void    HttpMessage::setMaxBodySize( long maxBodySize )
{
    _maxBodySize = maxBodySize;
}

bool    HttpMessage::headerDone( void ) const
{
    return (_bits[HEADER_DONE]);
}

HttpMessage&    HttpMessage::setHeader( std::string const & key, std::string const & value )
{
    if (!hasHeader(key))
        _headerMap.push_back(std::make_pair(key, value));
    return (*this);
}
HttpMessage&    HttpMessage::clearHeader( void )
{
    _headerMap.clear();
    return (*this);
}

HttpMessage&    HttpMessage::clearBody( void )
{
    _buffer2.clear();
    return (*this);
}

HttpMessage&    HttpMessage::setBody( std::istream & data, std::string const & mimeType )
{
    std::size_t bodySize = getStreamSize(data);
    std::string bodySizeStr = toStr(bodySize);
    bool cTypeFound = false, cLengthFound = false;
    for (headers_t::iterator it = _headerMap.begin(); it != _headerMap.end(); ++it) {
        if (it->first == "content-type") {
            cTypeFound = true;
            it->second = mimeType;
        }
        if (it->first == "content-length") {
            cLengthFound = true;
            it->second = bodySizeStr;
        }
    }
    if (!cTypeFound)
        _headerMap.push_back(std::make_pair("content-type", mimeType));
    if (!cLengthFound)
        _headerMap.push_back(std::make_pair("content-length", bodySizeStr));
    // _buffer2 = data.
    _buffer2.clear();
    addStreamToBuff(_buffer2, data, bodySize);
    if (mimeType =="text/html")
        std::cout << "set body content: " << std::string(_buffer2.begin(), _buffer2.end()) << std::endl;
        // std::cout << "set body content: " << _buffer2 << std::endl;
    return (*this);
}

HttpMessage&    HttpMessage::setBodyError( ws_http::statuscodes_t errorStatus )
{
    std::stringstream ss;
    ss << "<!doctype html>" "<html lang=\"en\">" "<head>" "<meta charset=\"utf-8\">"
        << "<title>" << ws_http::statuscodes.at(errorStatus) << "</title>" "</head>" 
        << "<body>" "<div style=\"text-align: center;\">" 
        << "<h1>" << ws_http::statuscodes.at(errorStatus) << "</h1>"
        << "<hr>" "<p>" << ws_http::webservVersion << "</p>" "</div>"
        << "</body>" "</html>";
    return (setBody(ss, "text/html"));
}

void    insertStrToBuff(std::string const & str, buff_t& buffer)
{
    std::copy(str.begin(), str.end(), std::back_inserter(buffer));
}

HttpMessage&    HttpMessage::setStatusLine( ws_http::version_t httpVersion, ws_http::statuscodes_t statusCode )
{
    // if (_status == ws_http::STATUS_UNDEFINED)
    //     return (*this);

    _buffer1.clear();
    insertStrToBuff(ws_http::versions.at(httpVersion), _buffer1);
    _buffer1.push_back(' ');
    insertStrToBuff(ws_http::statuscodes.at(statusCode), _buffer1);
    insertStrToBuff(ws_http::crlf, _buffer1);

    // _buffer1.clear();
    // _buffer1 = ws_http::versions.at(httpVersion) + " " + ws_http::statuscodes.at(statusCode) + ws_http::crlf;

    return (*this);
}

HttpMessage&    HttpMessage::setStatusLine( ws_http::version_t httpVersion, std::string const & status )
{
    // if (_status == ws_http::STATUS_UNDEFINED)
    //     return (*this);

    _buffer1.clear();
    insertStrToBuff(ws_http::versions.at(httpVersion), _buffer1);
    _buffer1.push_back(' ');
    insertStrToBuff(status, _buffer1);
    insertStrToBuff(ws_http::crlf, _buffer1);

    // _buffer1.clear();
    // _buffer1 = ws_http::versions.at(httpVersion) + " " + status + ws_http::crlf;

    return (*this);
}

HttpMessage&    HttpMessage::setRequestLine( ws_http::method_t method, std::string const & requestTarget, ws_http::version_t httpVersion )
{
    // if (_status == ws_http::STATUS_UNDEFINED)
    //     return (*this);
    // if (_buffer1.empty())

    _buffer1.clear();
    insertStrToBuff(ws_http::methods.at(method), _buffer1);
    _buffer1.push_back(' ');
    insertStrToBuff(requestTarget, _buffer1);
    _buffer1.push_back(' ');
    insertStrToBuff( ws_http::versions.at(httpVersion), _buffer1);
    insertStrToBuff(ws_http::crlf, _buffer1);

    // _buffer1.clear();
    // _buffer1 = ws_http::methods.at(method) + requestTarget + ws_http::versions.at(httpVersion) + ws_http::crlf;

    return (*this);
}

bool    HttpMessage::readyToSend( void ) const
{
    return (_bits[SEND_RESPONSE]);
}

HttpMessage&    HttpMessage::flushMessage( void )
{

    if (_buffer1.empty() && _headerMap.empty()) {
        // _bits[SEND_HEADER_DONE] = true;
        // _contentLength = _buffer2.size();
        bodySize = _buffer2.size();
        bodyPtr = _buffer2.data();
        return (*this);
    }
    bool hasDate = false;
    for (headers_t::iterator it = _headerMap.begin(); it != _headerMap.end(); ++it) {
        if (it->first == "date")
            hasDate = true;
        insertStrToBuff(it->first, _buffer1);
        _buffer1.push_back(':');
        _buffer1.push_back(' ');
        insertStrToBuff(it->second, _buffer1);
        insertStrToBuff(ws_http::crlf, _buffer1);
        // _buffer1 += it->first + ": " + it->second + ws_http::crlf;
    }
    if (!hasDate)
        _headerMap.push_back(std::make_pair("date", getDateString(0, "%a, %d %b %Y %H:%M:%S GMT")));

    insertStrToBuff(ws_http::crlf, _buffer1);
    // _buffer1 += ws_http::crlf;

    // _contentLength = _buffer1.size();
    _bits[SEND_RESPONSE] = true;
    headerPtr = _buffer1.data();
    headerSize = _buffer1.size();
    bodySize = _buffer2.size();
    bodyPtr = _buffer2.data();

    std::cout << "\n\n ---- FLUSH THIS ------- size: " << _buffer1.size() << std::endl;
    std::cout << std::string(_buffer1.begin(), _buffer1.end()) << std::endl;
    // std::cout << _buffer1 << std::endl;
    // printMessage();
    return (*this);
}

int HttpMessage::readFromSocketAndParseHttp( int socketfd , int flags, bool parseReqline )
{
    if (_bits[SEND_RESPONSE])
        throw std::logic_error("unable to read before sending done");
    int readByte = recv(socketfd, HttpMessage::readBuffer.data(), HttpMessage::readBuffer.size(), flags);
    if (readByte == -1 || readByte == 0 || _status != ws_http::STATUS_UNDEFINED)
        return (readByte);

    if (!parseReqline)
        _reqLinePos = REQLINE_DONE;
    
    buff_t::const_iterator it_start = HttpMessage::readBuffer.begin();
    buff_t::const_iterator it_end = HttpMessage::readBuffer.begin() + readByte;
    if (_bits[HEADER_DONE]) {
        parseBody(it_start, it_end);
        return (readByte);
    }
    for (; _status < ws_http::STATUS_400_BAD_REQUEST && it_start != it_end; ++it_start) {
        if (*it_start == '\n') {
            if (!_bits[CR_FOUND]) {
                setStatus(ws_http::STATUS_400_BAD_REQUEST);
                return (readByte);
            }
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
    it_start++;
    if (_bits[HEADER_DONE]) {
        if (getHeader("host").empty()) {
            setStatus(ws_http::STATUS_400_BAD_REQUEST);
            return (readByte);
        }
        if (_maxBodySizeGetter)
            _maxBodySize = (*_maxBodySizeGetter)(getHeader("@pathdecoded"), getHeader("host"));
        _contentLength = getHeader("transfer-encoding") != "" ? -1 : strtol(getHeader("content-length").c_str(), NULL, 10);
        parseBody(it_start, it_end);
    }
    return (readByte);
}

int HttpMessage::sendDataToSocket( int socketfd, int flags )
{
    std::cout << "send data to socket" << std::endl;
    int writeByte = 0;
    if (this->headerSize > 0) {
        writeByte = send(socketfd, this->headerPtr, this->headerSize, flags);
        if (writeByte > 0) {
            this->headerPtr += writeByte;
            this->headerSize -= writeByte;
        }
    } else if (this->bodySize > 0) {
        writeByte = send(socketfd, this->bodyPtr, this->bodySize, flags);
        if (writeByte > 0) {
            this->bodyPtr += writeByte;
            this->bodySize -= writeByte;
        }
    }
    if (this->bodySize == 0 && this->headerSize == 0)
        _bits[SEND_DONE] = true;
    return (writeByte);
}

void    HttpMessage::resetMessage( void )
{
    _headerMap.clear();
    _buffer1.clear();
    _buffer2.clear();
    _status = ws_http::STATUS_UNDEFINED;
    _maxBodySizeGetter = NULL;
    _bits = std::bitset<BITS_MAX>("011000000");
    _contentLength = 0;
    _maxBodySize = readBufferSize;
    _chunkSize = 0;
    _reqLinePos = REQLINE_PARSE_METHOD;
    headerPtr = NULL;
    headerSize = 0;
    bodyPtr = NULL;
    bodySize = 0;
}

bool    HttpMessage::sendDone( void ) const
{
    return (_bits[SEND_DONE]);
}

// int HttpMessage::sendDataToSocket( int socketfd, int flags )
// {
//     std::cout << "\n------ sendDataToSocket ------" << std::endl;
//     std::cout << "ready to send?: " << _bits[SEND_RESPONSE] << std::endl;
//     std::cout << "send header?: " << _bits[SEND_HEADER_DONE] << std::endl;
//     std::cout << "send done?: " << _bits[SEND_DONE] << std::endl;
//     if (!_bits[SEND_RESPONSE])
//         throw std::logic_error("unable to send before flushing");
    
//     if (_bits[SEND_DONE])
//         return (0);
    
//     // buff_t& usedBuff = _buffer1;
//     // if (_buffer1.empty()) {
//     //     std::cout << "use buffer2" << std::endl;
//     //     usedBuff = _buffer2;
//     // }
//     // if (_contentLength == 0 && !_buffer1.empty()) {
//     //     std::cout << "clear buffer1" << std::endl;
//     //     _buffer1.clear();
//     //     _contentLength = _buffer2.size();
//     // }
//     const char* dataPtr;
    
//     if (_bits[!SEND_HEADER_DONE]) {
//         dataPtr = _buffer1.data() + (_buffer1.size() - _contentLength);
//         std::cout << "send header!" << std::endl;
//         std::cout << "content: " << dataPtr << std::endl;
//     } else {
//         std::cout << "send header!" << std::endl;
//         dataPtr = _buffer2.data() + (_buffer2.size() - _contentLength);
//     }


    
//     std::cout << "content length: " << _contentLength << std::endl;
//     // if (usedBuff == _buffer1)
//     int sendBytes = send(socketfd, dataPtr, _contentLength, flags);
//     std::cout << "send bytes: " << sendBytes << std::endl;
//     if (sendBytes > 0)
//         _contentLength -= sendBytes;
//     std::cout << "new content length: " << _contentLength << std::endl;

//     if (!_bits[SEND_HEADER_DONE] && _contentLength == 0) {
//         std::cout << "set send header done true" << std::endl;
//         _bits[SEND_HEADER_DONE] = true;
//         _contentLength = _buffer2.size();
//     }

//     if (_bits[SEND_HEADER_DONE] && _contentLength == 0) {
//         std::cout << "set send done true" << std::endl;
//         _bits[SEND_DONE] = true;

//     }

//     // if (_buffer1.empty() && _contentLength == 0)
//     //     _bits[SEND_DONE] = true;

//     return (sendBytes);
// }
