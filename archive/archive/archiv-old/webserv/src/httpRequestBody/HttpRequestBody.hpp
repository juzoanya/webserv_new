/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttprequestBody.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 18:36:41 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 21:34:41 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_BODY_HPP
#define HTTP_REQUEST_BODY_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>
#include "../HttpConstants.hpp"

typedef std::vector<char>    buff_t;

class   BodyChunk {
    public:
        BodyChunk( buff_t& body );
        ~BodyChunk( void );
        int parseChunk(buff_t::iterator it_start, buff_t::iterator it_end);
    private:
        int     chunkSize;
        buff_t  tmpBuffer;
        buff_t& bodyBuffer;
};

class   HttpRequestBody {
    public:
        HttpRequestBody( void );
        HttpRequestBody(long contSize, long maxBody);
        ~HttpRequestBody( void );

        buff_t::iterator        setChunkSize(buff_t::iterator it_start, buff_t::iterator it_end);
        int                     copyData(buff_t::iterator it_start, buff_t::iterator it_end);
        ws_http::statuscodes_t  parseChunk(buff_t::iterator it_start, buff_t::iterator it_end);
        ws_http::statuscodes_t  parseBody(buff_t::iterator it_start, buff_t::iterator it_end);
        buff_t const &          getBody( void ) const;
    private:
        long    _contentLength;
        long    _maxBodySize;
        long    _chunkSize;
        int     _splitCase;
        buff_t  _buffer;
        buff_t  _bodyBuffer;
};

#endif