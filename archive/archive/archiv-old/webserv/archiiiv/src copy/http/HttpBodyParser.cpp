/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBodyParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:15:28 by mberline          #+#    #+#             */
/*   Updated: 2023/11/15 12:55:31 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "HttpBodyParser.hpp"

Chunk::Chunk(buff_t& bufferData, long remainSize)
 :  status(WS_HTTP_PARSE_PARSING), _done(false), _bufferData(&bufferData),
    _size(0), _maxRemainSize(remainSize), _parseChunk(false), _splitted(false)
{ }

bool    Chunk::isDone( void ) const { return (this->_done); }
ws_http_parser_status_t    Chunk::getStatus( void ) const { return (this->status); }

long getChunkSize(std::string const & str)
{
    char *rest;
    const char *strPtr = str.c_str();
    long size = strtol(strPtr, &rest, 16);
    if (str.empty() || *rest != 0 || size < 0)
        return (-1);
    return (size);
}

buff_t::const_iterator Chunk::parseSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    buff_t::const_iterator it_pos = it_start;
    if (this->_splitted && *it_start != '\n') {
        this->status = WS_HTTP_PARSE_ERROR_BODY_SYNTAX;
        return (it_start);
    } else if (this->_splitted && *it_start == '\n') {
        it_pos++;
        this->_parseChunk = true;
        this->_splitted = false;
    } else {
        it_pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
        std::copy(it_start, it_pos, std::back_inserter(this->_chunkSizedata));
        if (it_pos != it_end) {
            it_pos += ws_http::crlf.size();
            this->_parseChunk = true;
        } else if (this->_chunkSizedata.back() == '\r') {
            this->_splitted = true;
            this->_chunkSizedata.pop_back();
        }
    }
    if (this->_parseChunk) {
        this->_size = getChunkSize(this->_chunkSizedata);
    }
    if (this->_parseChunk && this->_size == 0) {
        this->_done = true;
        this->status = WS_HTTP_PARSE_BODY_DONE;
        return (it_end);
    }
    return (it_pos);
}

buff_t::const_iterator Chunk::parse(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    if (this->_done)
        return (it_end);
    buff_t::const_iterator it_pos = it_start;
    if (!this->_parseChunk) {
        it_pos = this->parseSize(it_start, it_end);
        if (it_pos == it_start || it_pos == it_end) {
            return (it_pos);
    }
    }
    it_start = it_pos;
    long minDist = std::min(it_end - it_start, this->_size);
    long restDist = it_end - (it_start + minDist);
    if (this->_size > 0) {
        if (minDist > this->_maxRemainSize) {
            this->status = WS_HTTP_PARSE_ERROR_BODY_OVERSIZE;
            return (it_start);
        }
        std::copy(it_start, it_start + minDist, std::back_inserter(*this->_bufferData));
        this->_size -= minDist;
        this->_maxRemainSize -= minDist;
        return (it_start + minDist);
    }
    
    if (this->_splitted && it_start[minDist] == '\n') {
        this->_done = true;
        return (it_start + minDist + 1);
    } else if (restDist >= 2 && it_start[minDist] == '\r' && it_start[minDist + 1] == '\n') {
        this->_done = true;
        return (it_start + minDist + 2);
    } else if (restDist == 1 && it_start[minDist] == '\r') {
        this->_splitted = true;
        return (it_end);
    }
    this->status = WS_HTTP_PARSE_ERROR_BODY_SYNTAX;
    return (it_start);
}

HttpBody::HttpBody( std::size_t contentLength, std::size_t maxBodySize )
 :  _maxBodySize(maxBodySize), _isChunked(contentLength == 0 ? true : false),
    _contentLength(contentLength), _chunk(this->_bodyData, this->_maxBodySize)
{ }

HttpBody::~HttpBody( void )
{ }

buff_t const & HttpBody::getBody( void ) const
{
    return (this->_bodyData);
}

ws_http_parser_status_t HttpBody::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
    std::cout << "------> parseBody\n";
    std::cout << "max body size: " << this->_maxBodySize << std::endl;
    std::cout << "curr body size: " << this->_bodyData.size() << std::endl;
    if (it_end - it_start == 0) {
        std::cout << "empty body" << std::endl;
        return (WS_HTTP_PARSE_PARSING);
    }
    if (this->_bodyData.capacity() == 0) {
        this->_bodyData.reserve(WS_HTTP_DEFAULT_MAX_BODY_SIZE);
    }
    if (!this->_isChunked) {
        std::cout << "Parse not chunked body\n";
        std::size_t dist = (it_end - it_start);
        if ( dist > this->_maxBodySize || dist > this->_contentLength) {
            return (WS_HTTP_PARSE_ERROR_BODY_OVERSIZE);
        }
        std::copy(it_start, it_end, std::back_inserter(this->_bodyData));
        this->_maxBodySize -= dist;
        this->_contentLength -= dist;
        if (this->_contentLength == 0)
            return (WS_HTTP_PARSE_BODY_DONE);
        return (WS_HTTP_PARSE_PARSING);
    } else {
        std::cout << "Parse chunked body\n";
        buff_t::const_iterator it_pos;
        while (it_start != it_end) {
            it_pos = this->_chunk.parse(it_start, it_end);
            if (this->_chunk.isDone() && this->_chunk.getStatus() == WS_HTTP_PARSE_BODY_DONE) {
                return (WS_HTTP_PARSE_BODY_DONE);
            } else if (this->_chunk.isDone()) {
                this->_chunk = Chunk(this->_bodyData, this->_maxBodySize - this->_bodyData.size());
            }
            if (this->_chunk.getStatus() != WS_HTTP_PARSE_PARSING) {
                return (this->_chunk.getStatus());
            }
            it_start = it_pos;
        }
        return (WS_HTTP_PARSE_PARSING);
    }
}
