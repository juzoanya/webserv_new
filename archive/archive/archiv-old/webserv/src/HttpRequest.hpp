/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:26:49 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 21:52:05 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <map>
#include <vector>
#include <string>
#include "HttpConstants.hpp"

typedef std::vector<char>   buff_t;

class HttpRequest {
    public:
        HttpRequest( void );
        ~HttpRequest( void );

        static const std::string crlf;
        static const std::string headEnd;

        ws_http::statuscodes_t    parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        std::map<std::string, std::string, ws_http::CaInCmp>  headerMap;
        std::string             method;
        std::string             path;
        std::string             pathDecoded;
        std::string             query;
        std::string             fullUri;
        std::string             hostHeader;
        ws_http::statuscodes_t  status;
        bool                    headerDone;
        buff_t                  bodyBuffer;
        long                    maxBodySize;

    private:
        buff_t::const_iterator  setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        buff_t  _buffer;
        long    _contentLength;
        long    _chunkSize;
        int     _splitCase;
};

#endif