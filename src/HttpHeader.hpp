/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 11:58:13 by mberline          #+#    #+#             */
/*   Updated: 2024/02/21 10:21:25 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include "headers.hpp"

class HttpHeader {
	public:
		typedef std::vector< std::pair<std::string, std::string> >    headers_t;
		HttpHeader( void );
		~HttpHeader( void );
		bool                setHeader( std::string const & key, std::string const & value );
		std::string const & getHeader( std::string const & key ) const;
		bool                hasHeader( std::string const & key ) const;
		headers_t const &   getHeaderVector( void ) const;
		int                 reparseRequestLine( ws_http::method_t method, std::string const & requestTarget );
		int                 parseHeader(const char* start, const char* end, bool parseReqline);

	private:
		enum HeaderBools { HEADER_DONE = 0x1, CR_FOUND = 0x2, HEADER_VALUE_START_FOUND = 0x4 };
		enum HeaderPos { PATH_DECODED, FULLURI, METHOD, PATH, QUERY, VERSION, HEADER_KEY, HEADER_VALUE };
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
