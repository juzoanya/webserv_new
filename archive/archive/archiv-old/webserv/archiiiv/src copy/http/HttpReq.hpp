/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:26:49 by mberline          #+#    #+#             */
/*   Updated: 2023/12/18 13:39:30 by mberline         ###   ########.fr       */
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

        int parseRequestLine(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        int parseReq(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        std::map<std::string, std::string, CaInCmp>  headerMap;

    private:
        std::string         _method;
        std::string         _path;
        std::string         _pathDecoded;
        std::string         _query;
        std::string         _fullUri;

        buff_t  _headerBuffer;
};

class HttpRes {
    public:
        HttpRes( void );
        ~HttpRes( void );
        
        int sendFile(std::string const & path);

    private:

};

#endif