/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 12:03:38 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 22:54:39 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

HttpHeader::HttpHeader( void ) : _state(0), _pos(METHOD), _len(0)
{
	_headers.resize(VERSION + 1);
	_headers[PATH_DECODED].first = "@pathdecoded";
	_headers[QUERY_DECODED].first = "@querydecoded";
	_headers[FULLURI].first = "@fullUri";
	_headers[METHOD].first = "@method";
	_headers[PATH].first = "@path";
	_headers[QUERY].first = "@query";
	_headers[VERSION].first = "@version";
}

HttpHeader::~HttpHeader( void )
{ }

std::string const & HttpHeader::getHeader( std::string const & key ) const
{
	for (std::size_t i = 0; i != _headers.size(); ++i) {
		if (_headers[i].first == key) {
			return (_headers[i].second);
		}
	}
	return (ws_http::dummyValue);
}

bool    HttpHeader::hasHeader( std::string const & key ) const
{
	for (std::size_t i = 0; i != _headers.size(); ++i) {
		if (_headers[i].first == key)
			return (true);
	}
	return (false);
}

bool	HttpHeader::setHeader( std::string const & key, std::string const & value )
{
	if (hasHeader(key))
		return (false);
	_headers.push_back(std::make_pair(key, value));
	return (true);
}


HttpHeader::headers_t const &   HttpHeader::getHeaderVector( void ) const
{
	return (_headers);
}

int HttpHeader::parseHeaderKey(unsigned char c)
{
	if (c == '\r' && _headers.back().first.empty()) {
		_state |= HEADER_DONE;
		return (0);
	}
	if (c == ':') {
		_pos = HEADER_VALUE;
		return (0);
	}
	if (!(std::isalnum(c) || ws_http::headerTchar.find(c) != std::string::npos))
		return (-1);
	_headers.back().first.push_back(std::tolower(c));
	return (0);
}

int HttpHeader::parseHeaderValue(unsigned char c)
{
	buff_t& buffer = _headers.back().second;
	if (c == '\r') {
		if (_state & CR_FOUND)
			return (-1);
		buffer.erase(buffer.end() - _len, buffer.end());
		_len = 0;
		_pos = HEADER_KEY;
		_state &= ~HEADER_VALUE_START_FOUND;
		return (0);
	}
	if (c == ' ' || c == '\t') {
		if (_state & HEADER_VALUE_START_FOUND) {
			_len++;
			buffer.push_back(c);
		}
		return (0);
	}
	if (!(c >= 128 || (c >= 33 && c <= 126)))
		return (-1);
	if (!(_state & HEADER_VALUE_START_FOUND))
		_state |= HEADER_VALUE_START_FOUND;
	_len = 0;
	buffer.push_back(c);
	return (0);
}

int HttpHeader::parseMethod(unsigned char c)
{
	if (c == ' ') {
		if (ws_http::methods_rev.find(_headers[METHOD].second) == ws_http::methods_rev.end())
			return (-1);
		return (1);
	}
	if (!isalpha(c))
		return (-1);
	_headers[METHOD].second.push_back(c);
	return (0);
}

int HttpHeader::parseVersion(unsigned char c)
{
	if (c == '\r') {
		if (_state & CR_FOUND || ws_http::versions_rev.find(_headers[VERSION].second) == ws_http::versions_rev.end())
			return (-1);
		return (1);
	}
	if (!(isalpha(c) || !isdigit(c) || c != '.' || c != '\\'))
		return (-1);
	_headers[VERSION].second.push_back(c);
	return (0);
}

int getCharFromHex(unsigned char a)
{
	if (std::isdigit(a))
		return (a - '0');
	if (a >= 'A' && a <= 'F')
		return (a + 10 - 'A');
	if (a >= 'a' && a <= 'f')
		return (a + 10 - 'a');
	return (std::numeric_limits<unsigned char>::max());
}

int HttpHeader::parsePath(unsigned char c)
{
	if (c == '?')
		return (1);
	if (c == ' ')
		return (2);
	if (!(isalnum(c) || c == '%' || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos))
		return (-1);
	if (_headers[PATH].second.empty() && c != '/')
		return (-1);
	_headers[PATH].second.push_back(c);
	_headers[FULLURI].second.push_back(c);
	if (c == '%')
		return (0);
	if (_headers[PATH].second.size() < 3) {
		_headers[PATH_DECODED].second.push_back(c);
	} else {
		unsigned char a = _headers[PATH].second[_headers[PATH].second.size() - 3];
		unsigned char b = _headers[PATH].second[_headers[PATH].second.size() - 2];
		if (a == '%') {
			int decoded = 16 * getCharFromHex(b) + getCharFromHex(c);
			if (decoded <= std::numeric_limits<unsigned char>::max())
				_headers[PATH_DECODED].second.push_back(decoded);
			else 
				return (-1);
		} else if (b != '%') {
			_headers[PATH_DECODED].second.push_back(c);
		}
	}
	return (0);
}

int HttpHeader::parseQuery(unsigned char c)
{
	if (c == ' ')
		return (1);
	if (!(isalnum(c) || c == '%' || ws_http::uriPathOrQueryAllowed.find(c) != std::string::npos))
		return (-1);
	if(_headers[QUERY].second.empty())
		_headers[FULLURI].second.push_back('?');
	_headers[QUERY].second.push_back(c);
	_headers[FULLURI].second.push_back(c);
	if (c == '%')
		return (0);
	if (_headers[QUERY].second.size() < 3) {
		_headers[QUERY_DECODED].second.push_back(c);
	} else {
		unsigned char a = _headers[QUERY].second[_headers[QUERY].second.size() - 3];
		unsigned char b = _headers[QUERY].second[_headers[QUERY].second.size() - 2];
		if (a == '%') {
			int decoded = 16 * getCharFromHex(b) + getCharFromHex(c);
			if (decoded <= std::numeric_limits<unsigned char>::max())
				_headers[QUERY_DECODED].second.push_back(decoded);
			else 
				return (-1);
		} else if (b != '%') {
			_headers[QUERY_DECODED].second.push_back(c);
		}
	}
	return (0);
}

int  HttpHeader::parseHeader(const char *start, const char *end, bool parseReqline)
{
	if (_state == -1)
		return (-1);
	if (_state & HEADER_DONE)
		return (1);
	if (!parseReqline) {
		_pos = HEADER_KEY;
		_headers.push_back(std::pair<std::string, std::string>());
	}
	int ret = 0;
	for (const char*posPtr = start ; ret != -1 && posPtr != end; ++posPtr) {
		if (_state & CR_FOUND && *posPtr != '\n') {
			ret = -1;
		} else if (_state & CR_FOUND) {
			_state &= ~CR_FOUND;
			if (_state & HEADER_DONE) {
				_headers.pop_back();
				return (posPtr + 1 - start);
			} else if (_pos >= HEADER_KEY) {
				_headers.push_back(std::pair<std::string, std::string>());
			}
		} else {
			if (_pos == METHOD)
				ret = parseMethod(*posPtr);
			else if (_pos == PATH)
				ret = parsePath(*posPtr);
			else if (_pos == QUERY)
				ret = parseQuery(*posPtr);
			else if (_pos == VERSION)
				ret = parseVersion(*posPtr);
			else if (_pos == HEADER_KEY)
				ret = parseHeaderKey(*posPtr);
			else
				ret = parseHeaderValue(*posPtr);
			_state = *posPtr == '\r' ? _state | CR_FOUND : _state & ~CR_FOUND;
			_pos += ret;
		}
	}
	if (ret == -1) {
		_headers.clear();
		_state = -1;
		return (1);
	}
	return (0);
}

int HttpHeader::reparseRequestLine( std::string const & method, std::string const & requestTarget )
{
	if (!(_state & HEADER_DONE) || _state == -1)
		return (0);
	_headers[METHOD].second = method;
	_headers[PATH].second.clear();
	_headers[PATH_DECODED].second.clear();
	_headers[QUERY].second.clear();
	_headers[FULLURI].second.clear();
	int ret = 0;
	_pos = PATH;
	for (std::string::const_iterator pos = requestTarget.begin() ; ret != -1 && pos != requestTarget.end(); ++pos) {
		if (_pos == PATH)
			ret = parsePath(*pos);
		else if (_pos == QUERY)
			ret = parseQuery(*pos);
		else if (_pos == VERSION)
			break ;
	}
	return (ret == -1 ? -1 : 1);
}
