/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBodyParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:17:01 by mberline          #+#    #+#             */
/*   Updated: 2023/12/15 10:24:43 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_BODY_PARSER
#define HTTP_BODY_PARSER

#include "HttpHeaderDef.hpp"
#include "HttpParserStatus.hpp"

#define WS_HTTP_DEFAULT_MAX_BODY_SIZE 4096

class Chunk {
public:
    Chunk(buff_t& bufferData, long remainSize);
    buff_t::const_iterator parse(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    bool    isDone( void ) const;
    ws_http_parser_status_t    getStatus( void ) const;
private:
    buff_t::const_iterator parseSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    ws_http_parser_status_t status;
    bool                    _done;
    buff_t*                 _bufferData;
    std::string             _chunkSizedata;
    long                    _size;
    long                    _maxRemainSize;
    bool                    _parseChunk;
    bool                    _splitted;
};


class HttpBody {
public:
    HttpBody(std::size_t contentLength=0, std::size_t maxBodySize = WS_HTTP_DEFAULT_MAX_BODY_SIZE );
    ~HttpBody( void );
    ws_http_parser_status_t parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    buff_t const & getBody( void ) const;
private:
    buff_t      _bodyData;
    std::size_t _maxBodySize;
    bool        _isChunked;
    std::size_t _contentLength;
    Chunk       _chunk;
};


#endif
