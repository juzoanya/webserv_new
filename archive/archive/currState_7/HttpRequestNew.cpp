/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestNew.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 19:49:42 by mberline          #+#    #+#             */
/*   Updated: 2024/02/10 20:21:40 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::string HttpHeaders::reqline_method = "@method";
std::string HttpHeaders::reqline_path = "@path";
std::string HttpHeaders::reqline_pathDecoded = "@pathdecoded";
std::string HttpHeaders::reqline_query = "@query";
std::string HttpHeaders::reqline_fullUri = "@fulluri";

HttpHeaders::HttpHeaderField HttpHeaders::dummyHeader;

HttpHeaders::HttpHeaderField::HttpHeaderField( void )
{ }

HttpHeaders::HttpHeaderField::HttpHeaderField( std::string const & token, std::string const & value )
 : _token(token), _value(value)
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
}

HttpHeaders::HttpHeaderField::HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue )
 : _token(itStartKey, itEndKey), _value(itStartValue, itEndValue), 
    _pos(itEndKey - itStartKey), _header(std::string(itStartKey, itEndKey) + std::string(itStartValue, itEndValue))
{
    std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
    std::transform(this->_header.begin(), this->_header.begin() + _pos, this->_header.begin(), tolower);
}

HttpHeaders::HttpHeaderField::~HttpHeaderField( void )
{ }

bool HttpHeaders::HttpHeaderField::operator==( std::string const & key ) const
{
    return (this->_header.compare(0, this->_pos, key));
}

bool HttpHeaders::HttpHeaderField::operator==( HttpHeaderField const & headerField ) const
{
    return (this->_header.compare());
}
bool HttpHeaders::HttpHeaderField::operator!=( HttpHeaderField const & headerField ) const { return (headerField._token != this->_token); }
bool HttpHeaders::HttpHeaderField::operator!=( std::string const & key ) const { return (key != this->_token); }
std::string const & HttpHeaders::HttpHeaderField::getField( void ) const
{
    // return (this->_token);
    return (this->_header.substr(0, _pos));
}
std::string const & HttpHeaders::HttpHeaderField::getValue( void ) const
{
    // return (this->_value);
    return (this->_header.substr(_pos, std::string::npos));
}

HttpHeaders::HttpHeaders( void )
 : _parseReqLine(false), _reqLineDone(false), _done(false)
{
    (void)_parseReqLine;
    (void)_reqLineDone;
    (void)_done;
}

HttpHeaders::~HttpHeaders( void )
{ }

bool    HttpHeaders::addHeader( std::string const & line ) { return (this->addHeader(line.begin(), line.end())); }


bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    buff_t::const_iterator itStartKey = itStart;
    buff_t::const_iterator itEndKey = itStart;
    buff_t::const_iterator itStartValue = itStart;
    buff_t::const_iterator lastNonWhitespace = itStart;
    bool parseKey = true;
    bool valueStartFound = false;
    bool crFound = false;

    for (; itStart != itEnd; ++itStart)
    {
        unsigned char c = *itStart;
        // if (std::isprint(c))
        //     std::cout << "c: " << c << " -> ";
        // else
        //     std::cout << "c: " << (int)c << " -> ";
        if (parseKey) {
            // std::cout << "parseKey";
            if (c == ':') {
                // std::cout << " - : found\n";
                parseKey = false;
                itEndKey = itStart;
            } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
                // std::cout << " - not ok char in key\n";
                return (false);
            } else {
                // std::cout << "\n";
            }
        } else if (crFound && c == '\n') {
            // std::cout << "crFound - and is \\n\n";
            // std::cout << "itStartKey: " << *itStartKey << std::endl; 
            // std::cout << "itEndKey: " << *itEndKey << std::endl; 
            // std::cout << "itStartValue: " << *itStartValue << std::endl; 
            // std::cout << "lastNonWhitespace: " << *lastNonWhitespace << std::endl;
            
            HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
            if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
                return (false);
            this->_headers.push_back(headerField);
            parseKey = true;
            valueStartFound = false;
            crFound = false;
            itStartKey = itStart + 1;
        } else if (c >= 128 || std::isprint(c)) {
            // std::cout << "isPrintable\n";
            crFound = false;
            if (!valueStartFound) {
                itStartValue = itStart;
                valueStartFound = true;
            } else {
                lastNonWhitespace = itStart;
            }
        } else if(c == '\r') {
            // std::cout << "crFound\n";
            crFound = true;
        } else if (c == ' ' || c == '\t') {
            // std::cout << "whitaspace\n";
            crFound = false;
        } else {
            // std::cout << "other - false\n";
            return (false);
        }
    }

    return (true);
}

bool    HttpHeaders::addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
{
    const buff_t::const_iterator itStartKey = itStart;
    const buff_t::const_iterator itEndKey = std::find(itStart, itEnd, ':');
    if (itEndKey == itEnd)
        return (false);
    for (; itStart != itEndKey; ++itStart) {
        if (!std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
            return (false);
    }
    itStart++;
    while (*itStart == ' ' || *itStart == '\t')
        itStart++;
    while (*itEnd == ' ' || *itEnd == '\t')
        itEnd--;
    for (buff_t::const_iterator it = itStart; it != itEnd; ++it) {
        unsigned char c = *it;
        if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
            continue ;
        return (false);
    }
    HttpHeaderField headerField(itStartKey, itEndKey, itStart, itEnd);
    if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
        return (false);
    this->_headers.push_back(headerField);
    return (true);
}


// bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// {
//     buff_t::const_iterator itStartKey = itStart;
//     buff_t::const_iterator itEndKey = itStart;
//     buff_t::const_iterator itStartValue = itStart;
//     buff_t::const_iterator lastNonWhitespace = itStart;
//     bool parseKey = true;
//     bool valueStartFound = false;
//     bool crFound = false;

//     for (; itStart != itEnd; ++itStart)
//     {
//         unsigned char c = *itStart;
//         // if (std::isprint(c))
//         //     std::cout << "c: " << c << " -> ";
//         // else
//         //     std::cout << "c: " << (int)c << " -> ";
//         if (parseKey) {
//             // std::cout << "parseKey";
//             if (c == ':') {
//                 // std::cout << " - : found\n";
//                 parseKey = false;
//                 itEndKey = itStart;
//             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 // std::cout << " - not ok char in key\n";
//                 return (false);
//             } else {
//                 // std::cout << "\n";
//             }
//         } else if (crFound && c == '\n') {
//             // std::cout << "crFound - and is \\n\n";
//             // std::cout << "itStartKey: " << *itStartKey << std::endl; 
//             // std::cout << "itEndKey: " << *itEndKey << std::endl; 
//             // std::cout << "itStartValue: " << *itStartValue << std::endl; 
//             // std::cout << "lastNonWhitespace: " << *lastNonWhitespace << std::endl; 
//             HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//             if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//                 return (false);
//             this->_headers.push_back(headerField);
//             parseKey = true;
//             valueStartFound = false;
//             crFound = false;
//             itStartKey = itStart + 1;
//         } else if (c >= 128 || std::isprint(c)) {
//             // std::cout << "isPrintable\n";
//             crFound = false;
//             if (!valueStartFound) {
//                 itStartValue = itStart;
//                 valueStartFound = true;
//             } else {
//                 lastNonWhitespace = itStart;
//             }
//         } else if(c == '\r') {
//             // std::cout << "crFound\n";
//             crFound = true;
//         } else if (c == ' ' || c == '\t') {
//             // std::cout << "whitaspace\n";
//             crFound = false;
//         } else {
//             // std::cout << "other - false\n";
//             return (false);
//         }
//     }

//     return (true);
// }


std::string const & HttpHeaders::getHeaderValue( std::string const & key ) const { return (this->getHeader(key).getValue()); }
HttpHeaders::HttpHeaderField const & HttpHeaders::getHeader( std::string const & key ) const
{
    std::vector<HttpHeaderField>::const_iterator it = std::find(this->_headers.begin(), this->_headers.end(), key);
    if (it != this->_headers.end())
        return (*it);
    return (HttpHeaders::dummyHeader);
}

std::vector<HttpHeaders::HttpHeaderField> const & HttpHeaders::getHeadersVec( void ) const { return (this->_headers); }



std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs )
{
    if (!rhs.getField().empty())
        os << rhs.getField() << ": " << rhs.getValue() << "$" << "\r\n";
    return (os);
}

std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs )
{
    std::vector<HttpHeaders::HttpHeaderField> const & headers = rhs.getHeadersVec();
    for (std::vector<HttpHeaders::HttpHeaderField>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        os << *it;
    return (os);
}

int main( void )
{
    // "GET / HTTP/1.1" + ws_http::crlf +
    std::string request = 
    "Host: example.com:8080" + ws_http::crlf +
    "Connection: keep-alive" + ws_http::crlf +
    "Cache-Control: max-age=0" + ws_http::crlf +
    "Upgrade-Insecure-Requests: 1" + ws_http::crlf +
    "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit\n/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36" + ws_http::crlf +
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8" + ws_http::crlf +
    "Sec-GPC: 1" + ws_http::crlf +
    "Accept-Encoding: gzip, deflate" + ws_http::crlf +
    "Accept-Language: de-DE,de;q=0.9";

    HttpHeaders headers;

    std::cout << "return : " << headers.addHeaderBuf(request.begin(), request.end()) << std::endl;
    std::cout << headers << std::endl;
    return (0);
}

// // ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     if (this->_server && this->_headerDone) {
// //         this->status = this->parseBody(it_start, it_end);
// //     } else {
// //         std::size_t buffSize = this->_buffer.size();
// //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //         buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
// //             ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// //         this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
// //         if (end == this->_buffer.end())
// //             return (ws_http::STATUS_UNDEFINED);
// //         this->status = this->parseHeaders(this->_buffer.begin(), end);
// //         if (this->status == ws_http::STATUS_UNDEFINED) {
// //             std::string const & hostHeaderValue = this->headers.getHeaderValue("host");
// //             if (hostHeaderValue.empty()) {
// //                 this->status = ws_http::STATUS_400_BAD_REQUEST;
// //             } else {
// //                 this->_headerDone = true;
// //                 if (this->_server) {
// //                     this->_maxBodySize = this->_server->getHttpConfig(this->pathDecoded, headers.getHeaderValue("host")).getMaxBodySize();
// //                     this->_contentLength = strtol(this->headers.getHeaderValue("content-length").c_str(), NULL, 10);
// //                     this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// //                 }
// //                 this->_buffer.clear();
// //                 if (!this->_server)
// //                     return (ws_http::STATUS_200_OK);
// //             }
// //         }
// //     }
// //     return (this->status);
// // }




// ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     // if (this->status != ws_http::STATUS_UNDEFINED)
//     //     return (this->status);
//     // if (this->_server && this->_headerDone) {
//     //     this->status = this->parseBody(it_start, it_end);
//     // } else {
//     std::size_t buffSize = this->_buffer.size();
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
//         ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//     this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
//     if (end == this->_buffer.end())
//         return (ws_http::STATUS_UNDEFINED);
//     this->status = this->parseHeaders(this->_buffer.begin(), end);
//     if (this->status == ws_http::STATUS_UNDEFINED) {
//         std::string const & hostHeaderValue = this->headers.getHeaderValue("host");
//         if (hostHeaderValue.empty()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//         } else {
//             this->_headerDone = true;
//             if (this->_server) {
//                 this->_maxBodySize = this->_server->getHttpConfig(this->pathDecoded, headers.getHeaderValue("host")).getMaxBodySize();
//                 this->_contentLength = strtol(this->headers.getHeaderValue("content-length").c_str(), NULL, 10);
//                 this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
//             }
//             this->_buffer.clear();
//             if (!this->_server)
//                 return (ws_http::STATUS_200_OK);
//         }
//     }
//     // }
//     return (this->status);
// }



// buff_t::const_iterator    HttpRequest::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_splitCase > 2 && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//         this->_splitCase = 0;
//         return (it_start + 1);
//     }
//     if (this->_splitCase == 1 || this->_splitCase == 2) {
//         it_start += this->_splitCase;
//         this->_splitCase = 0;
//     }
//     buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         this->_splitCase = ;
//         return (it_start);
//     }
//     if (!this->_buffer.empty()) {
//         std::copy(it_start, itSize, std::back_inserter(this->_buffer));
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//     } else {
//         this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//     }
//     return (itSize + ws_http::crlf.size());
// }

// ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         long buffDist = std::distance(it_start, it_end);
//         if (static_cast<long>(this->_buffer.size()) + buffDist > this->_contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (static_cast<long>(this->_buffer.size()) + buffDist > this->_maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         if (static_cast<long>(this->_buffer.size()) == this->_contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     }
//     buff_t::const_iterator content_start = it_start;
//     while (std::distance(it_start, it_end) > 0) {
//         if (this->_chunkSize == 0) {
//             content_start = this->setChunkSize(it_start, it_end);
//             if (content_start == it_start)
//                 return (ws_http::STATUS_UNDEFINED);
//             if (this->_chunkSize == 0)
//                 return (ws_http::STATUS_200_OK);
//         }
//         long dist = std::min(static_cast<int>(std::distance(content_start, it_end)), this->_chunkSize);
//         long rest_dist = std::distance(content_start + dist, it_end);
//         if (static_cast<long>(this->_buffer.size()) + dist > this->_maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(content_start, content_start + dist, std::back_inserter(this->_buffer));
//         this->_chunkSize -= dist;
//         if (this->_chunkSize == 0 && rest_dist == 0) {
//             this->_splitCase = 2;
//         } else if (this->_chunkSize == 0 && rest_dist == 1) {
//             this->_splitCase = 1;
//         }
//         it_start = content_start + dist + ws_http::crlf.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }






































// bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// {
//     while (itStart < itEnd) {
//         const buff_t::const_iterator itStartKey = itStart;
//         while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
//             itStart++;
//         if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
//             return (false);
//         const buff_t::const_iterator itEndKey = itStart;
//         itStart++;
//         while (*itStart == ' ' || *itStart == '\t')
//             itStart++;
//         const buff_t::const_iterator itStartValue = itStart;
//         buff_t::const_iterator lastNonWhitespace = itStart;
//         bool crFound = false;
//         for (;itStart != itEnd; ++itStart) {
//             unsigned char c = *itStart;
//             if (c == '\n' && crFound) {
//                 HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//                 if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//                     return (false);
//                 this->_headers.push_back(headerField);
//                 itStart++;
//                 break ;
//             }
//             if (c == '\r' && !crFound) {
//                 crFound = true;
//             } else if (((c >= 33 && c <= 126) || c >= 128) && !crFound) {
//                 lastNonWhitespace = itStart;
//                 crFound = false;
//             } else if ((c == ' ' || c == '\t') && !crFound) {
//                 crFound = false;
//             } else {
//                 return (false);
//             }
//         }
//     }
//     return (true);
// }

    // for (; itStart != itEnd; ++itStart)
    // {
    //     unsigned char c = *itStart;
    //     if (parseKey) {
    //         if (!(std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos)) {
    //             return (false);
    //         } else if (c == ':') {
    //             parseKey = false;
    //             itEndKey = itStart;
    //         }
    //     } else if (crFound) {
    //         if (c == '\n') {
    //             HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
    //             if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
    //                 return (false);
    //             this->_headers.push_back(headerField);
    //             parseKey = true;
    //             ValueStartFound = false;
    //             crFound = false;
    //         } else {
    //             return (false);
    //         }
    //     } else if ((c >= 33 && c <= 126) || c >= 128) {
    //         crFound = false;
    //         if (!ValueStartFound)
    //             itStartValue = itStart;
    //         else
    //             lastNonWhitespace = itStart;
    //     } else if(c == '\r') {
    //         crFound = true;
    //     } else if (c == ' ' || c == '\t') {
            
    //     }
    // }

    // for (; itStart != itEnd; ++itStart)
    // {
    //     if (parseKey && !(std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)) {
    //         return (false);
    //     } else if (parseKey && *itStart == ':') {
    //         parseKey = false;
    //         itEndKey = itStart;
    //     } else if (!parseKey && !crFound && c == '\r') {
    //         crFound = true;
    //     } else if (!parseKey && !crFound && ((c >= 33 && c <= 126) || c >= 128)) {
    //         lastNonWhitespace = itStart;
    //         crFound = false;
    //     } else if (!parseKey && !crFound && (c == ' ' || c == '\t')) {
    //         crFound = false;
    //     } else {
    //         return (false);
    //     }
    //     if (c == '\n' && crFound) {
    //         HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
    //         if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
    //             return (false);
    //         this->_headers.push_back(headerField);
    //         itStart++;
    //         break ;
    //     }
    // }

    // while (itStart < itEnd)
    // {
    //     if (parseKey && !(std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos))
    //         return (false);
    //     if (parseKey && *itStart == ':') {
    //         parseKey = false;
    //         itEndKey = itStart;
    //     }
    //     if (!parseKey && !crFound && c == '\r') {
    //         crFound = true;
    //     } else if (!parseKey && !crFound && ((c >= 33 && c <= 126) || c >= 128)) {
    //         lastNonWhitespace = itStart;
    //         crFound = false;
    //     } else if (!parseKey && !crFound && (c == ' ' || c == '\t')) {
    //         crFound = false;
    //     } else {
    //         return (false);
    //     }
    //     if (c == '\n' && crFound) {
    //         HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
    //         if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
    //             return (false);
    //         this->_headers.push_back(headerField);
    //         itStart++;
    //         break ;
    //     }
    // }

    

// std::string      HttpHeaders::reqline_method = "reqline_method";
// std::string      HttpHeaders::reqline_path = "reqline_path";
// std::string      HttpHeaders::reqline_pathDecoded = "reqline_pathdecoded";
// std::string      HttpHeaders::reqline_query = "reqline_query";
// std::string      HttpHeaders::reqline_fullUri = "reqline_fulluri";

// HttpHeaders::HttpHeaderField HttpHeaders::dummyHeader;

// HttpHeaders::HttpHeaderField::HttpHeaderField( void )
// { }

// HttpHeaders::HttpHeaderField::HttpHeaderField( std::string const & token, std::string const & value )
//  : _token(token), _value(value)
// {
//     std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
// }

// HttpHeaders::HttpHeaderField::HttpHeaderField( buff_t::const_iterator itStartKey, buff_t::const_iterator itEndKey, buff_t::const_iterator itStartValue, buff_t::const_iterator itEndValue )
//  : _token(itStartKey, itEndKey), _value(itStartValue, itEndValue)
// {
//     std::transform(this->_token.begin(), this->_token.end(), this->_token.begin(), tolower);
// }

// HttpHeaders::HttpHeaderField::~HttpHeaderField( void )
// { }

// bool HttpHeaders::HttpHeaderField::operator==( HttpHeaderField const & headerField ) const { return (headerField._token == this->_token); }
// bool HttpHeaders::HttpHeaderField::operator==( std::string const & key ) const { return (key == this->_token); }
// bool HttpHeaders::HttpHeaderField::operator!=( HttpHeaderField const & headerField ) const { return (headerField._token != this->_token); }
// bool HttpHeaders::HttpHeaderField::operator!=( std::string const & key ) const { return (key != this->_token); }
// std::string const & HttpHeaders::HttpHeaderField::getField( void ) const { return (this->_token); }
// std::string const & HttpHeaders::HttpHeaderField::getValue( void ) const { return (this->_value); }

// HttpHeaders::HttpHeaders( void )
//  : _parseReqLine(false), _reqLineDone(false), _done(false)
// {
//     (void)_parseReqLine;
//     (void)_reqLineDone;
//     (void)_done;
// }

// HttpHeaders::~HttpHeaders( void )
// { }

// bool    HttpHeaders::addHeader( std::string const & line ) { return (this->addHeader(line.begin(), line.end())); }

// bool    HttpHeaders::addHeader( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// {
//     const buff_t::const_iterator itStartKey = itStart;
//     const buff_t::const_iterator itEndKey = std::find(itStart, itEnd, ':');
//     if (itEndKey == itEnd)
//         return (false);
//     for (; itStart != itEndKey; ++itStart) {
//         if (!std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
//             return (false);
//     }
//     itStart++;
//     while (*itStart == ' ' || *itStart == '\t')
//         itStart++;
//     while (*itEnd == ' ' || *itEnd == '\t')
//         itEnd--;
//     for (buff_t::const_iterator it = itStart; it != itEnd; ++it) {
//         unsigned char c = *it;
//         if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
//             continue ;
//         return (false);
//     }
//     HttpHeaderField headerField(itStartKey, itEndKey, itStart, itEnd);
//     if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//         return (false);
//     this->_headers.push_back(headerField);
//     return (true);
// }


// // bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// // {
// //     buff_t::const_iterator itStartKey = itStart;
// //     buff_t::const_iterator itEndKey = itStart;
// //     buff_t::const_iterator itStartValue = itStart;
// //     buff_t::const_iterator lastNonWhitespace = itStart;
// //     bool parseKey = true;
// //     bool valueStartFound = false;
// //     bool crFound = false;

// //     for (; itStart != itEnd; ++itStart)
// //     {
// //         unsigned char c = *itStart;
// //         // if (std::isprint(c))
// //         //     std::cout << "c: " << c << " -> ";
// //         // else
// //         //     std::cout << "c: " << (int)c << " -> ";
// //         if (parseKey) {
// //             // std::cout << "parseKey";
// //             if (c == ':') {
// //                 // std::cout << " - : found\n";
// //                 parseKey = false;
// //                 itEndKey = itStart;
// //             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
// //                 // std::cout << " - not ok char in key\n";
// //                 return (false);
// //             } else {
// //                 // std::cout << "\n";
// //             }
// //         } else if (crFound && c == '\n') {
// //             // std::cout << "crFound - and is \\n\n";
// //             // std::cout << "itStartKey: " << *itStartKey << std::endl; 
// //             // std::cout << "itEndKey: " << *itEndKey << std::endl; 
// //             // std::cout << "itStartValue: " << *itStartValue << std::endl; 
// //             // std::cout << "lastNonWhitespace: " << *lastNonWhitespace << std::endl; 
// //             HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
// //             if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
// //                 return (false);
// //             this->_headers.push_back(headerField);
// //             parseKey = true;
// //             valueStartFound = false;
// //             crFound = false;
// //             itStartKey = itStart + 1;
// //         } else if (c >= 128 || std::isprint(c)) {
// //             // std::cout << "isPrintable\n";
// //             crFound = false;
// //             if (!valueStartFound) {
// //                 itStartValue = itStart;
// //                 valueStartFound = true;
// //             } else {
// //                 lastNonWhitespace = itStart;
// //             }
// //         } else if(c == '\r') {
// //             // std::cout << "crFound\n";
// //             crFound = true;
// //         } else if (c == ' ' || c == '\t') {
// //             // std::cout << "whitaspace\n";
// //             crFound = false;
// //         } else {
// //             // std::cout << "other - false\n";
// //             return (false);
// //         }
// //     }

// //     return (true);
// // }

// bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// {
//     buff_t::const_iterator itStartKey = itStart;
//     buff_t::const_iterator itEndKey = itStart;
//     buff_t::const_iterator itStartValue = itStart;
//     buff_t::const_iterator lastNonWhitespace = itStart;
//     bool parseKey = true;
//     bool valueStartFound = false;
//     bool crFound = false;

//     for (; itStart != itEnd; ++itStart)
//     {
//         unsigned char c = *itStart;
//         // if (std::isprint(c))
//         //     std::cout << "c: " << c << " -> ";
//         // else
//         //     std::cout << "c: " << (int)c << " -> ";
//         if (parseKey) {
//             // std::cout << "parseKey";
//             if (c == ':') {
//                 // std::cout << " - : found\n";
//                 parseKey = false;
//                 itEndKey = itStart;
//             } else if (!std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos) {
//                 // std::cout << " - not ok char in key\n";
//                 return (false);
//             } else {
//                 // std::cout << "\n";
//             }
//         } else if (crFound && c == '\n') {
//             // std::cout << "crFound - and is \\n\n";
//             // std::cout << "itStartKey: " << *itStartKey << std::endl; 
//             // std::cout << "itEndKey: " << *itEndKey << std::endl; 
//             // std::cout << "itStartValue: " << *itStartValue << std::endl; 
//             // std::cout << "lastNonWhitespace: " << *lastNonWhitespace << std::endl; 
//             HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//             if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//                 return (false);
//             this->_headers.push_back(headerField);
//             parseKey = true;
//             valueStartFound = false;
//             crFound = false;
//             itStartKey = itStart + 1;
//         } else if (c >= 128 || std::isprint(c)) {
//             // std::cout << "isPrintable\n";
//             crFound = false;
//             if (!valueStartFound) {
//                 itStartValue = itStart;
//                 valueStartFound = true;
//             } else {
//                 lastNonWhitespace = itStart;
//             }
//         } else if(c == '\r') {
//             // std::cout << "crFound\n";
//             crFound = true;
//         } else if (c == ' ' || c == '\t') {
//             // std::cout << "whitaspace\n";
//             crFound = false;
//         } else {
//             // std::cout << "other - false\n";
//             return (false);
//         }
//     }

//     return (true);
// }

// std::string const & HttpHeaders::getHeaderValue( std::string const & key ) const { return (this->getHeader(key).getValue()); }

// HttpHeaders::HttpHeaderField const & HttpHeaders::getHeader( std::string const & key ) const
// {
//     std::vector<HttpHeaderField>::const_iterator it = std::find(this->_headers.begin(), this->_headers.end(), key);
//     if (it != this->_headers.end())
//         return (*it);
//     return (HttpHeaders::dummyHeader);
// }

// std::vector<HttpHeaders::HttpHeaderField> const & HttpHeaders::getHeadersVec( void ) const { return (this->_headers); }

// std::ostream& operator<<( std::ostream & os, HttpHeaders::HttpHeaderField const & rhs )
// {
//     if (!rhs.getField().empty())
//         os << rhs.getField() << ": " << rhs.getValue() << "$" << "\r\n";
//     return (os);
// }

// std::ostream& operator<<( std::ostream & os, HttpHeaders const & rhs )
// {
//     std::vector<HttpHeaders::HttpHeaderField> const & headers = rhs.getHeadersVec();
//     for (std::vector<HttpHeaders::HttpHeaderField>::const_iterator it = headers.begin(); it != headers.end(); ++it)
//         os << *it;
//     return (os);
// }

// int main( void )
// {
//     // "GET / HTTP/1.1" + ws_http::crlf +
//     std::string request = 
//     "Host: example.com:8080" + ws_http::crlf +
//     "Connection: keep-alive" + ws_http::crlf +
//     "Cache-Control: max-age=0" + ws_http::crlf +
//     "Upgrade-Insecure-Requests: 1" + ws_http::crlf +
//     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit\n/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36" + ws_http::crlf +
//     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8" + ws_http::crlf +
//     "Sec-GPC: 1" + ws_http::crlf +
//     "Accept-Encoding: gzip, deflate" + ws_http::crlf +
//     "Accept-Language: de-DE,de;q=0.9";

//     HttpHeaders headers;

//     std::cout << "return : " << headers.addHeaderBuf(request.begin(), request.end()) << std::endl;
//     std::cout << headers << std::endl;
//     return (0);
// }

// // // ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // // {
// // //     if (this->status != ws_http::STATUS_UNDEFINED)
// // //         return (this->status);
// // //     if (this->_server && this->_headerDone) {
// // //         this->status = this->parseBody(it_start, it_end);
// // //     } else {
// // //         std::size_t buffSize = this->_buffer.size();
// // //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// // //         buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
// // //             ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// // //         this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
// // //         if (end == this->_buffer.end())
// // //             return (ws_http::STATUS_UNDEFINED);
// // //         this->status = this->parseHeaders(this->_buffer.begin(), end);
// // //         if (this->status == ws_http::STATUS_UNDEFINED) {
// // //             std::string const & hostHeaderValue = this->headers.getHeaderValue("host");
// // //             if (hostHeaderValue.empty()) {
// // //                 this->status = ws_http::STATUS_400_BAD_REQUEST;
// // //             } else {
// // //                 this->_headerDone = true;
// // //                 if (this->_server) {
// // //                     this->_maxBodySize = this->_server->getHttpConfig(this->pathDecoded, headers.getHeaderValue("host")).getMaxBodySize();
// // //                     this->_contentLength = strtol(this->headers.getHeaderValue("content-length").c_str(), NULL, 10);
// // //                     this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// // //                 }
// // //                 this->_buffer.clear();
// // //                 if (!this->_server)
// // //                     return (ws_http::STATUS_200_OK);
// // //             }
// // //         }
// // //     }
// // //     return (this->status);
// // // }




// // ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     // if (this->status != ws_http::STATUS_UNDEFINED)
// //     //     return (this->status);
// //     // if (this->_server && this->_headerDone) {
// //     //     this->status = this->parseBody(it_start, it_end);
// //     // } else {
// //     std::size_t buffSize = this->_buffer.size();
// //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //     buff_t::const_iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
// //         ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// //     this->endPos = it_start + ((end - this->_buffer.begin()) - buffSize);
// //     if (end == this->_buffer.end())
// //         return (ws_http::STATUS_UNDEFINED);
// //     this->status = this->parseHeaders(this->_buffer.begin(), end);
// //     if (this->status == ws_http::STATUS_UNDEFINED) {
// //         std::string const & hostHeaderValue = this->headers.getHeaderValue("host");
// //         if (hostHeaderValue.empty()) {
// //             this->status = ws_http::STATUS_400_BAD_REQUEST;
// //         } else {
// //             this->_headerDone = true;
// //             if (this->_server) {
// //                 this->_maxBodySize = this->_server->getHttpConfig(this->pathDecoded, headers.getHeaderValue("host")).getMaxBodySize();
// //                 this->_contentLength = strtol(this->headers.getHeaderValue("content-length").c_str(), NULL, 10);
// //                 this->status = this->parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// //             }
// //             this->_buffer.clear();
// //             if (!this->_server)
// //                 return (ws_http::STATUS_200_OK);
// //         }
// //     }
// //     // }
// //     return (this->status);
// // }



// // buff_t::const_iterator    HttpRequest::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->_splitCase > 2 && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
// //         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
// //         this->_buffer.clear();
// //         this->_splitCase = 0;
// //         return (it_start + 1);
// //     }
// //     if (this->_splitCase == 1 || this->_splitCase == 2) {
// //         it_start += this->_splitCase;
// //         this->_splitCase = 0;
// //     }
// //     buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
// //     if (itSize == it_end) {
// //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //         this->_splitCase = ;
// //         return (it_start);
// //     }
// //     if (!this->_buffer.empty()) {
// //         std::copy(it_start, itSize, std::back_inserter(this->_buffer));
// //         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
// //         this->_buffer.clear();
// //     } else {
// //         this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
// //     }
// //     return (itSize + ws_http::crlf.size());
// // }

// // ws_http::statuscodes_t  HttpRequest::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->_contentLength >= 0) {
// //         long buffDist = std::distance(it_start, it_end);
// //         if (static_cast<long>(this->_buffer.size()) + buffDist > this->_contentLength)
// //             return (ws_http::STATUS_400_BAD_REQUEST);
// //         if (static_cast<long>(this->_buffer.size()) + buffDist > this->_maxBodySize)
// //             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
// //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //         if (static_cast<long>(this->_buffer.size()) == this->_contentLength)
// //             return (ws_http::STATUS_200_OK);
// //         return (ws_http::STATUS_UNDEFINED);
// //     }
// //     buff_t::const_iterator content_start = it_start;
// //     while (std::distance(it_start, it_end) > 0) {
// //         if (this->_chunkSize == 0) {
// //             content_start = this->setChunkSize(it_start, it_end);
// //             if (content_start == it_start)
// //                 return (ws_http::STATUS_UNDEFINED);
// //             if (this->_chunkSize == 0)
// //                 return (ws_http::STATUS_200_OK);
// //         }
// //         long dist = std::min(static_cast<int>(std::distance(content_start, it_end)), this->_chunkSize);
// //         long rest_dist = std::distance(content_start + dist, it_end);
// //         if (static_cast<long>(this->_buffer.size()) + dist > this->_maxBodySize)
// //             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
// //         std::copy(content_start, content_start + dist, std::back_inserter(this->_buffer));
// //         this->_chunkSize -= dist;
// //         if (this->_chunkSize == 0 && rest_dist == 0) {
// //             this->_splitCase = 2;
// //         } else if (this->_chunkSize == 0 && rest_dist == 1) {
// //             this->_splitCase = 1;
// //         }
// //         it_start = content_start + dist + ws_http::crlf.size();
// //     }
// //     return (ws_http::STATUS_UNDEFINED);
// // }






































// // bool    HttpHeaders::addHeaderBuf( buff_t::const_iterator itStart, buff_t::const_iterator itEnd )
// // {
// //     while (itStart < itEnd) {
// //         const buff_t::const_iterator itStartKey = itStart;
// //         while (itStart != itEnd && *itStart != ':' && std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)
// //             itStart++;
// //         if (itStart == itEnd || itStart + 1 == itEnd || *itStart != ':')
// //             return (false);
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
// //                 HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
// //                 if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
// //                     return (false);
// //                 this->_headers.push_back(headerField);
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
// //                 return (false);
// //             }
// //         }
// //     }
// //     return (true);
// // }

//     // for (; itStart != itEnd; ++itStart)
//     // {
//     //     unsigned char c = *itStart;
//     //     if (parseKey) {
//     //         if (!(std::isalnum(c) && ws_http::headerTchar.find(c) == std::string::npos)) {
//     //             return (false);
//     //         } else if (c == ':') {
//     //             parseKey = false;
//     //             itEndKey = itStart;
//     //         }
//     //     } else if (crFound) {
//     //         if (c == '\n') {
//     //             HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//     //             if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//     //                 return (false);
//     //             this->_headers.push_back(headerField);
//     //             parseKey = true;
//     //             ValueStartFound = false;
//     //             crFound = false;
//     //         } else {
//     //             return (false);
//     //         }
//     //     } else if ((c >= 33 && c <= 126) || c >= 128) {
//     //         crFound = false;
//     //         if (!ValueStartFound)
//     //             itStartValue = itStart;
//     //         else
//     //             lastNonWhitespace = itStart;
//     //     } else if(c == '\r') {
//     //         crFound = true;
//     //     } else if (c == ' ' || c == '\t') {
            
//     //     }
//     // }

//     // for (; itStart != itEnd; ++itStart)
//     // {
//     //     if (parseKey && !(std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos)) {
//     //         return (false);
//     //     } else if (parseKey && *itStart == ':') {
//     //         parseKey = false;
//     //         itEndKey = itStart;
//     //     } else if (!parseKey && !crFound && c == '\r') {
//     //         crFound = true;
//     //     } else if (!parseKey && !crFound && ((c >= 33 && c <= 126) || c >= 128)) {
//     //         lastNonWhitespace = itStart;
//     //         crFound = false;
//     //     } else if (!parseKey && !crFound && (c == ' ' || c == '\t')) {
//     //         crFound = false;
//     //     } else {
//     //         return (false);
//     //     }
//     //     if (c == '\n' && crFound) {
//     //         HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//     //         if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//     //             return (false);
//     //         this->_headers.push_back(headerField);
//     //         itStart++;
//     //         break ;
//     //     }
//     // }

//     // while (itStart < itEnd)
//     // {
//     //     if (parseKey && !(std::isalnum(*itStart) && ws_http::headerTchar.find(*itStart) == std::string::npos))
//     //         return (false);
//     //     if (parseKey && *itStart == ':') {
//     //         parseKey = false;
//     //         itEndKey = itStart;
//     //     }
//     //     if (!parseKey && !crFound && c == '\r') {
//     //         crFound = true;
//     //     } else if (!parseKey && !crFound && ((c >= 33 && c <= 126) || c >= 128)) {
//     //         lastNonWhitespace = itStart;
//     //         crFound = false;
//     //     } else if (!parseKey && !crFound && (c == ' ' || c == '\t')) {
//     //         crFound = false;
//     //     } else {
//     //         return (false);
//     //     }
//     //     if (c == '\n' && crFound) {
//     //         HttpHeaderField headerField(itStartKey, itEndKey, itStartValue, lastNonWhitespace + 1);
//     //         if (std::find(this->_headers.begin(), this->_headers.end(), headerField) != this->_headers.end())
//     //             return (false);
//     //         this->_headers.push_back(headerField);
//     //         itStart++;
//     //         break ;
//     //     }
//     // }