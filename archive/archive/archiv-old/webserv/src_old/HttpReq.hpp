/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:26:49 by mberline          #+#    #+#             */
/*   Updated: 2024/01/03 11:25:26 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQ_HPP
#define HTTPREQ_HPP

#include <map>
#include <string>
#include "HttpHeaderDef.hpp"

struct CaInCmp {
    bool operator() ( const std::string& str1, const std::string& str2 ) const;
};

class HttpMsg {
    public:
        HttpMsg( void );
        ~HttpMsg( void );

        static const std::string crlf;
        static const std::string headEnd;

        void    parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

        std::map<std::string, std::string, CaInCmp>  headerMap;
        std::string _method;
        std::string _path;
        std::string _pathDecoded;
        std::string _query;
        std::string _fullUri;
        buff_t      _buffer;
        ws_http::statuscodes_t  _status;

    private:
        void    parseRequestLine(buff_t::iterator it_start, buff_t::iterator it_end);
        void    parseHeader(buff_t::iterator it_start, buff_t::iterator it_end);
};

#endif