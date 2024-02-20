/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 11:58:13 by mberline          #+#    #+#             */
/*   Updated: 2024/02/17 10:56:57 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include "headers.hpp"

// class HttpHeader {
//     public:
//         typedef std::vector< std::pair<std::string, std::string> >    headers_t;
//         HttpHeader( void );
//         ~HttpHeader( void );
//         bool                setHeader( std::string const & key, std::string const & value );
//         std::string const & getHeader( std::string const & key ) const;
//         bool                hasHeader( std::string const & key ) const;
//         headers_t const &   getHeaderVector( void ) const;

//         int                parseHeader(const char* start, const char* end, bool parseReqline);
//     private:
//         enum HeaderBools { HEADER_DONE = 0x1, CR_FOUND = 0x2, HEADER_VALUE_START_FOUND = 0x4 };
//         enum HeaderPos { METHOD, PATH, PATH_DECODED, QUERY, FULLURI, VERSION, HEADER_KEY, HEADER_VALUE };

//         typedef	int (*parsing_func_t)(unsigned char c);
// 		static parsing_func_t parsingFuncs[];
        
//         int  parseHeaderLine(unsigned char c, HttpHeader& header);
//         int  parseHeaderKey(unsigned char c, HttpHeader& header);
//         int  parseHeaderValue(unsigned char c, HttpHeader& header);
//         int  parseMethod(unsigned char c, HttpHeader& header);
//         int  parsePath(unsigned char c, HttpHeader& header);
//         int  parseQuery(unsigned char c, HttpHeader& header);
//         int  parseVersion(unsigned char c, HttpHeader& header);

//         headers_t       _headers;
//         char            _state;
//         unsigned char   _pos;
//         unsigned char   _len;


// };

// int getCharFromHex(unsigned char a);

// struct HttpRequestInfos {
//     HttpRequestInfos(ws_http::method_t reqMethod, ws_http::version_t reqVersion, 
//         std::string & reqpPathDecoded, std::string & reqQuery, std::string & reqFullUri);
//     ~HttpRequestInfos( void );
//     ws_http::method_t   method;
//     ws_http::version_t  version;
//     std::string &       pathDecoded;
//     std::string &       query;
//     std::string &       fullUri;
//     std::string &       host;
//     std::string &       contentType;
// };

// class HttpRequestLine {
//     HttpRequestLine(ws_http::method_t reqMethod, ws_http::version_t reqVersion, 
//         std::string & reqpPathDecoded, std::string & reqQuery, std::string & reqFullUri);
//     ~HttpRequestLine( void );
//     ws_http::method_t   method;
//     ws_http::version_t  version;
//     std::string &       pathDecoded;
//     std::string &       query;
//     std::string &       fullUri;
//     std::string &       host;
//     std::string &       contentType;
// };

class HttpHeader {
    public:
        typedef std::vector< std::pair<std::string, std::string> >    headers_t;
        HttpHeader( void );
        ~HttpHeader( void );
        bool                setHeader( std::string const & key, std::string const & value );
        std::string const & getHeader( std::string const & key ) const;
        bool                hasHeader( std::string const & key ) const;
        headers_t const &   getHeaderVector( void ) const;
        // HttpRequestInfos    getRequestInfos( void );
        int                 reparseRequestLine( ws_http::method_t method, std::string const & requestTarget );

        int                 parseHeader(const char* start, const char* end, bool parseReqline);
    private:
        enum HeaderBools { HEADER_DONE = 0x1, CR_FOUND = 0x2, HEADER_VALUE_START_FOUND = 0x4 };
        enum HeaderPos { PATH_DECODED, FULLURI, METHOD, PATH, QUERY, VERSION, HEADER_KEY, HEADER_VALUE };

        // typedef	int (*parsing_func_t)(HttpHeader* inst, unsigned char c);
		// static parsing_func_t parsingFuncs[];
        
        int  parseHeaderLine(unsigned char c);
        int  parseHeaderKey(unsigned char c);
        int  parseHeaderValue(unsigned char c);
        int  parseMethod(unsigned char c);
        int  parsePath(unsigned char c);
        int  parseQuery(unsigned char c);
        int  parseVersion(unsigned char c);

        headers_t       _headers;
        char            _state;
        unsigned char   _pos;
        unsigned char   _len;


};

int getCharFromHex(unsigned char a);

#endif
