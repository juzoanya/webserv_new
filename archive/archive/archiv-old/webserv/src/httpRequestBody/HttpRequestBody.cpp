/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestBody.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 18:36:16 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 20:52:57 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestBody.hpp"


void    printBuffer(std::string const & msg, buff_t::iterator it_start, buff_t::iterator it_end)
{
    std::cout << " -------------------- " << msg << ":  -------------------- " <<std::endl;
    std::cout << std::string(it_start, it_end) << std::endl;
    std::cout << " ----------------------------------------------------------------- " <<std::endl;
}


HttpRequestBody::HttpRequestBody( void )
{ }

HttpRequestBody::HttpRequestBody(long contSize, long maxBody) : _contentLength(contSize), _maxBodySize(maxBody), _chunkSize(0), _splitCase(0)
{ }

HttpRequestBody::~HttpRequestBody( void )
{ }

buff_t const & HttpRequestBody::getBody( void ) const { return (this->_bodyBuffer); }


int HttpRequestBody::copyData(buff_t::iterator it_start, buff_t::iterator it_end)
{
    std::size_t dist = std::min(std::distance(it_start, it_end), this->_chunkSize);
    std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
    this->_chunkSize -= dist;
    this->_maxBodySize -= dist;
    return (1);
}

// buff_t::iterator    HttpRequestBody::setChunkSize(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     if (!this->_buffer.empty()) {
//         buff_t::iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     }
//     buff_t::iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//         return (it_end);
//     }
//     this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
// }

// buff_t::iterator    HttpRequestBody::setChunkSize(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     buff_t::iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
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


// buff_t::iterator    HttpRequestBody::setChunkSize(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//         return (it_start + 1);
//     }
//     if (this->_splitCase != 0) {
//         // std::cout << "we have a splitcase\n";
//         it_start += this->_splitCase;
//         this->_splitCase = 0;
//     }
        

//     // if (itSize != it_end && this->_buffer.empty()) {
//     //     this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//     //     return (itSize + ws_http::crlf.size());
//     // }
    
//     buff_t::iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (itSize == it_end) {
//         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
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

// ws_http::statuscodes_t    HttpRequestBody::parseChunk(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     // std::cout << "-- parse chunked --\n";
//     // printBuffer("socket buffer", it_start, it_end);

//     buff_t::iterator content_start = it_start;
//     static int k = 0;
//     while (std::distance(it_start, it_end) > 0 && k++ < 100) {
//         //  std::cout << "\n\n---LOOP--- distance it_start <-> it_end: " << std::distance(it_start, it_end) << std::endl;
//         if (this->_chunkSize == 0) {
//             // std::cout << "setChunkSize" << std::endl;
//             content_start = this->setChunkSize(it_start, it_end);
//             if (content_start == it_start) {
//                 // std::cout << "chunkSize was not inside buffer\n";
//                 return (ws_http::STATUS_UNDEFINED);
//             }
//             // std::cout << "new Chunk Size: " << this->_chunkSize << std::endl;
//         }
//         if (this->_chunkSize == 0) {
//             // std::cout << "end found!\n";
//             // std::cout << "rest distance: " << std::distance(content_start, it_end) << std::endl;
//             return (ws_http::STATUS_200_OK);
//         }
//         std::size_t dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
//         std::size_t rest_dist = std::distance(content_start + dist, it_end);
//         // std::cout << "distance content_start <-> it_end is: " << std::distance(content_start, it_end) << std::endl;
//         // std::cout << "chunkSize is: " << this->_chunkSize << std::endl;
//         std::copy(content_start, content_start + dist, std::back_inserter(this->_bodyBuffer));
//         this->_chunkSize -= dist;
//         this->_maxBodySize -= dist;
//         it_start = content_start + dist + ws_http::crlf.size();
//         // std::cout << "after: chunksize: " << this->_chunkSize << std::endl;
//         // std::cout << "after: maxBodySize: " << this->_maxBodySize << std::endl;
//         // printBuffer("new bodyBuffer", this->_bodyBuffer.begin(), this->_bodyBuffer.end());
//         if (this->_chunkSize == 0 && rest_dist == 0) {
//             // std::cout << "rest dist 0\n";
//             this->_splitCase = 2;
//         } else if (this->_chunkSize == 0 && rest_dist == 1) {
//             // std::cout << "rest dist 1\n";
//             this->_splitCase = 1;
//         }
//     }
    
//     return (ws_http::STATUS_UNDEFINED);
// }

buff_t::iterator    HttpRequestBody::setChunkSize(buff_t::iterator it_start, buff_t::iterator it_end)
{
    if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
        this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
        this->_buffer.clear();
        return (it_start + 1);
    }
    if (this->_splitCase != 0) {
        it_start += this->_splitCase;
        this->_splitCase = 0;
    }
    buff_t::iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
    if (itSize == it_end) {
        std::copy(it_start, it_end, std::back_inserter(this->_buffer));
        return (it_start);
    }
    if (!this->_buffer.empty()) {
        std::copy(it_start, itSize, std::back_inserter(this->_buffer));
        this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
        this->_buffer.clear();
    } else {
        this->_chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
    }
    return (itSize + ws_http::crlf.size());
}

ws_http::statuscodes_t    HttpRequestBody::parseChunk(buff_t::iterator it_start, buff_t::iterator it_end)
{
    buff_t::iterator content_start = it_start;
    while (std::distance(it_start, it_end) > 0) {
        if (this->_chunkSize == 0) {
            content_start = this->setChunkSize(it_start, it_end);
            if (content_start == it_start)
                return (ws_http::STATUS_UNDEFINED);
            if (this->_chunkSize == 0)
                return (ws_http::STATUS_200_OK);
        }
        std::size_t dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
        std::size_t rest_dist = std::distance(content_start + dist, it_end);
        if (this->_bodyBuffer.size() + dist > this->_maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(content_start, content_start + dist, std::back_inserter(this->_bodyBuffer));
        this->_chunkSize -= dist;
        if (this->_chunkSize == 0 && rest_dist == 0) {
            this->_splitCase = 2;
        } else if (this->_chunkSize == 0 && rest_dist == 1) {
            this->_splitCase = 1;
        }
        it_start = content_start + dist + ws_http::crlf.size();
    }
    return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t    HttpRequestBody::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
{
    // std::cout << "\n\n---- PARSE BODY ----\n";
    if (this->_contentLength >= 0) {
        std::cout << " -- parse normally --\n";
        std::size_t buffDist = std::distance(it_start, it_end);
        if (this->_bodyBuffer.size() + buffDist > this->_contentLength)
            return (ws_http::STATUS_400_BAD_REQUEST);
        if (this->_bodyBuffer.size() + buffDist > this->_maxBodySize)
            return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
        std::copy(it_start, it_end, std::back_inserter(this->_bodyBuffer));
        if (this->_bodyBuffer.size() == this->_contentLength)
            return (ws_http::STATUS_200_OK);
        return (ws_http::STATUS_UNDEFINED);
    }
    return (this->parseChunk(it_start, it_end));
}
