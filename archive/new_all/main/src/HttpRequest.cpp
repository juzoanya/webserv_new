/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 12:39:08 by mberline          #+#    #+#             */
/*   Updated: 2024/01/23 04:15:16 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::string getStringUrlDecoded(std::string const & str)
{
	char *res = new char[str.size()];
	std::size_t k = 0;
	for (std::size_t pos = 0; pos < str.size(); pos++, k++) {
		if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
			|| isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
			int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
			int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
			res[k] = a + b;
			pos += 2;
		} else if (str[pos]) {
			res[k] = str[pos];
		}
	}
	std::string result = std::string(res, k);
	delete[] res;
	return (result);
}

// HttpRequest::HttpRequest( void )
//  : status(ws_http::STATUS_UNDEFINED)
// { }

HttpRequest::HttpRequest( OnHttpRequestHeaderDoneFunctor& headerDoneFunc )
 : status(ws_http::STATUS_UNDEFINED), _headerDoneFunc(&headerDoneFunc), _headerDone(false)
{ }

HttpRequest::~HttpRequest( void )
{ }


void HttpRequest::printRequest( void ) const
{
	std::cout << " --- REQUEST HEADER --- " << std::endl;
	std::cout << " --- Request-Line --- " << std::endl;
	std::cout << "Request Method: " << this->method << std::endl;
	std::cout << "Request Target PathEncoded: " << this->path << std::endl;
	std::cout << "Request Target PathDecoded: " << this->pathDecoded << std::endl;
	std::cout << "Request Target Query: " << this->query << std::endl;
	std::cout << "Request Target FullUri: " << this->fullUri << std::endl;
	std::cout << " --- Header-Fields --- " << std::endl;
	for (http_header_map_t::const_iterator it = this->headerMap.begin(); it != this->headerMap.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << std::endl;
	std::cout << " --- REQUEST BODY Size: " << this->requestBody.bodyBuffer.size() << "--- " << std::endl;
	// std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + std::min(1000ul, this->requestBody.bodyBuffer.size())) << std::endl;
	std::cout << std::string(this->requestBody.bodyBuffer.begin(), this->requestBody.bodyBuffer.begin() + this->requestBody.bodyBuffer.size()) << std::endl;
}

// bool    addHeader(std::string const & key, std::string const & value, http_header_map_t& headers)
// {
//     // if (" \"(),/:;<=>?@[\\]{}")
//     // if (key.find_first_not_of("!#$%&'*+-.^_`|~") != std::string::npos) {
//     //     if (std::isalnum())
//     // }
//     std::size_t pos = 
//     while (pos != std::string::npos) {
//         pos = key.find_first_not_of("!#$%&'*+-.^_`|~");
//         if (pos != std::string::npos && !std::isalnum(key[pos]))
//             return (false);
//     }
//     std::size_t valueStart = value.find_first_not_of(" \t");
//     std::size_t valueEnd = value.find_last_not_of(" \t");
// }

/*

header-field    =   field-name ":" OWS field-value OWS
-> field-name consists of of any number TCHAR characters, minimum 1 character
-> colon has to be directly after the field-name
-> before and after the field-value can be any number of SPACE or HORIZONTAL TABS
-> the field-value consists of VCHAR and obs-text chracters, can be separated by SPACE or HORIZONTAL TABS
TCHAR       =   "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
				"^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
VCHAR       =   ASCII von x20 - x7E (visible / diplayable characters) / 33 - 126 
obs-text    =   ASCII von x80- xFF (all after 127) / 128 - 255

OWS             =   *( SP / HTAB )
field-name      =   token
	token       =   1*TCHAR
field-value     =   *( field-content )
field-content   =   field-vchar [ 1*( SP / HTAB ) field-vchar ]
field-vchar     =   VCHAR / obs-text

*/

bool    addHeader(std::string const & line, http_header_map_t& headers)
{
	std::size_t keyEnd = line.find(':');
	if (keyEnd == std::string::npos || keyEnd == 0)
		return (false);
	for (std::size_t pos = 0; pos != keyEnd; ++pos) {
		// std::cout << "pos: " << pos << " | value: " << line[pos] << std::endl;
		if (!std::isalnum(line[pos]) && ws_http::headerTchar.find(line[pos]) == std::string::npos)
			return (false);
	}
	std::string token = line.substr(0, keyEnd);
	if (headers.find(token) != headers.end())
		return (false);
	std::size_t valueStart = line.find_first_not_of(" \t", keyEnd + 1);
	std::size_t valueEnd = line.find_last_not_of(" \t") + 1;
	for (std::size_t pos = valueStart; pos != valueEnd; ++pos) {
		// std::cout << "pos: " << pos << " | value: " << line[pos]  << std::endl;
		unsigned char c = line[valueStart];
		if (c == ' ' || c == '\t' || (c >= 33 && c <= 126) || c >= 128)
			continue ;
		return (false);
	}
	// std::cout << token << ": " << line.substr(valueStart, valueEnd - valueStart) << std::endl;
	headers.insert(std::make_pair(token, line.substr(valueStart, valueEnd - valueStart)));
	return (true);
}

ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
	// std::cout << "----- Request: parse Headers - headerBuffer: -----\n" << std::string(it_start, it_end) << std::endl;
	// std::cout << "--------------------------------------------------" << std::endl;
	buff_t::const_iterator pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
	std::string         version;
	std::stringstream   ss(std::string(it_start, pos));
	ss >> this->method;
	ss >> this->fullUri;
	ss >> version;
	// std::cout << "requestLine: " << this->method << " | " << this->fullUri << " | " << version << std::endl;
	if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end()) {
		// std::cout << "bad request from requestline\n";
		return (ws_http::STATUS_400_BAD_REQUEST);
	}
	std::size_t queryPos = this->fullUri.find_first_of('?');
	this->path = this->fullUri.substr(0, queryPos);
	this->pathDecoded = getStringUrlDecoded(this->path);
	if (queryPos != std::string::npos)
		this->query = this->fullUri.substr(queryPos + 1, std::string::npos);
	// std::cout << "path: " << this->path << " | decoded: " << this->pathDecoded << " | query: " << this->query << std::endl;

	for (buff_t::const_iterator start = pos + ws_http::crlf.size(); start < it_end; start = pos + ws_http::crlf.size()) {
		pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
		// std::cout << "\n---- addHeader ----\n";
		if (!addHeader(std::string(start, pos), this->headerMap)) {
			// std::cout << "----------------\n";
			return (ws_http::STATUS_400_BAD_REQUEST);
		}
		// std::cout << "----------------\n";
	}
	return (ws_http::STATUS_UNDEFINED);
}

ws_http::statuscodes_t  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
	if (this->status != ws_http::STATUS_UNDEFINED) {
		// std::cout << "request: already done | status: " << this->status << std::endl;   
		return (this->status);
	}
	if (this->_headerDone) {
		this->status = this->requestBody.parseBody(it_start, it_end);
		// std::cout << "request: headerDone | status from body: " << this->status << std::endl;
	} else {
		std::copy(it_start, it_end, std::back_inserter(this->_buffer));
		buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
			ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
		if (end == this->_buffer.end())
			return (ws_http::STATUS_UNDEFINED);
		this->status = this->parseHeaders(this->_buffer.begin(), end);
		// std::cout << "request: header parsed, status: " << this->status << std::endl;
		if (this->status == ws_http::STATUS_UNDEFINED) {
			std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itHostHeader = this->headerMap.find("host");
			if (itHostHeader == this->headerMap.end()) {
				this->status = ws_http::STATUS_400_BAD_REQUEST;
				// std::cout << "request: no host header | status: " << this->status << std::endl;
			} else {
				this->_headerDone = true;
				this->hostHeader = itHostHeader->second;
				this->requestBody = HttpRequestBody(*this, this->_headerDoneFunc->operator()(this->pathDecoded, this->hostHeader));
				this->status = this->requestBody.parseBody(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
				// std::cout << "request: headerDone | status from body(after parsing Header): " << this->status << std::endl;
				this->_buffer.clear();
			}
		}
	}
	// std::cout << "request: status returning: " << this->status << std::endl;
	return (this->status);
}


HttpRequestBody::HttpRequestBody( void )
 : maxBodySize(0), _contentLength(0), _chunkSize(0), _splitCase(0)
{ }

HttpRequestBody::HttpRequestBody( HttpRequest const & request, long maxRequestBodySize )
 : maxBodySize(maxRequestBodySize), _contentLength(0), _chunkSize(0), _splitCase(0)
{
	std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itContLen = request.headerMap.find("content-length");
	std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itTransEnc = request.headerMap.find("transfer-encoding");
	if (itContLen != request.headerMap.end())
		this->_contentLength = strtol(itContLen->second.c_str(), NULL, 10);
	else if (itTransEnc != request.headerMap.end())
		this->_contentLength = -1;
}

HttpRequestBody::~HttpRequestBody( void )
{ }

buff_t::const_iterator    HttpRequestBody::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
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
	buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
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

ws_http::statuscodes_t  HttpRequestBody::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
{
	// std::cout << "request, parse Body: maxBodyLen: " << this->maxBodySize << " | contentLen: " << this->_contentLength << std::endl;
	if (this->_contentLength >= 0) {
		long buffDist = std::distance(it_start, it_end);
		// std::cout << "buffDist: " << buffDist << std::endl;
		if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->_contentLength)
			return (ws_http::STATUS_400_BAD_REQUEST);
		if (static_cast<long>(this->bodyBuffer.size()) + buffDist > this->maxBodySize)
			return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
		std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
		if (static_cast<long>(this->bodyBuffer.size()) == this->_contentLength)
			return (ws_http::STATUS_200_OK);
		return (ws_http::STATUS_UNDEFINED);
	}
	buff_t::const_iterator content_start = it_start;
	while (std::distance(it_start, it_end) > 0) {
		if (this->_chunkSize == 0) {
			content_start = this->setChunkSize(it_start, it_end);
			if (content_start == it_start)
				return (ws_http::STATUS_UNDEFINED);
			if (this->_chunkSize == 0)
				return (ws_http::STATUS_200_OK);
		}
		long dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
		long rest_dist = std::distance(content_start + dist, it_end);
		if (static_cast<long>(this->bodyBuffer.size()) + dist > this->maxBodySize)
			return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
		std::copy(content_start, content_start + dist, std::back_inserter(this->bodyBuffer));
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





// std::string getStringUrlDecoded(std::string const & str)
// {
//     char *res = new char[str.size()];
//     std::size_t k = 0;
//     for (std::size_t pos = 0; pos < str.size(); pos++, k++) {
//         if (str[pos] == '%' && str[pos + 1] && str[pos + 2] && (isdigit(str[pos + 1])
//             || isupper(str[pos + 1])) && (isdigit(str[pos + 2]) || isupper(str[pos + 2]))) {
//             int a = 16 * (str[pos + 1] < 'A' ? str[pos + 1] - 48 : str[pos + 1] - 55);
//             int b =      (str[pos + 2] < 'A' ? str[pos + 2] - 48 : str[pos + 2] - 55);
//             res[k] = a + b;
//             pos += 2;
//         } else if (str[pos]) {
//             res[k] = str[pos];
//         }
//     }
//     std::string result = std::string(res, k);
//     delete res;
//     return (result);
// }

// HttpRequest::HttpRequest( void )
//  : status(ws_http::STATUS_UNDEFINED)
// { }

// HttpRequest::~HttpRequest( void )
// { }

// ws_http::statuscodes_t    HttpRequest::parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     buff_t::const_iterator pos = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     std::string         version;
//     std::stringstream   ss(std::string(it_start, pos));
//     std::getline(ss, this->method, ' ');
//     std::getline(ss, this->fullUri, ' ');
//     std::getline(ss, version);

//     if (!ss.eof() || ws_http::versions.find(version) == ws_http::versions.end() || ws_http::methods.find(this->method) == ws_http::methods.end())
//         return (ws_http::STATUS_400_BAD_REQUEST);
//     std::size_t queryPos = this->fullUri.find_first_of('?');
//     this->path = this->fullUri.substr(0, queryPos);
//     this->pathDecoded = getStringUrlDecoded(this->path);
//     if (queryPos != std::string::npos)
//         this->query = this->fullUri.substr(queryPos + 1, std::string::npos);

//     for (buff_t::const_iterator start = pos + ws_http::crlf.size(); start < it_end; start = pos + ws_http::crlf.size()) {
//         pos = std::search(start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         std::stringstream   headerFieldStream(std::string(start, pos));
//         std::string token, value;
//         std::getline(headerFieldStream, token, ':');
//         std::getline(headerFieldStream, value);

//         if (value.empty() || !headerFieldStream.eof())
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (value.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos
//         || token.find_first_of(ws_http::header_charstr_forbidden) != std::string::npos
//         || this->headerMap.find(token) != this->headerMap.end())
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         this->headerMap[token] = value;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// // buff_t::const_iterator  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// // {
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (it_end);
// //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //     buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(), ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
// //     if (end != this->_buffer.end()) {
// //         this->status = ws_http::STATUS_UNDEFINED;
// //         return (it_end);
// //     }
// //     this->status = this->parseHeaders(this->_buffer.begin(), end);
// //     if (this->status != ws_http::STATUS_UNDEFINED)
// //         return (this->status);
// //     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itHostHeader = this->headerMap.find("host");
// //     if (itHostHeader == this->headerMap.end())
// //         return (this->status = ws_http::STATUS_400_BAD_REQUEST);
// //     this->hostHeader = itHostHeader->second;

// //     buff_t tmpBuffer = std::vector<char>(end + ws_http::httpHeaderEnd.size(), this->_buffer.end());
// //     this->_buffer.clear();
// //     // this->status = this->parseBody(tmpBuffer.begin(), tmpBuffer.end());
// //     // return (this->status);
// //     return (this->status = this->parseBody(tmpBuffer.begin(), tmpBuffer.end()));
// // }

// buff_t::const_iterator  HttpRequest::parseRequest(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->status != ws_http::STATUS_UNDEFINED)
//         return (it_end);
//     std::size_t tmpBufferSize = this->_buffer.size();
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     buff_t::iterator end = std::search(this->_buffer.begin(), this->_buffer.end(),
//         ws_http::httpHeaderEnd.begin(), ws_http::httpHeaderEnd.end());
//     if (end == this->_buffer.end())
//         return (it_end);
//     this->status = this->parseHeaders(this->_buffer.begin(), end);
//     if (this->status == ws_http::STATUS_UNDEFINED) {
//         std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itHostHeader = this->headerMap.find("host");
//         if (itHostHeader == this->headerMap.end()) {
//             this->status = ws_http::STATUS_400_BAD_REQUEST;
//         } else {
//             this->hostHeader = itHostHeader->second;
//             std::size_t headerEndPos = (end - this->_buffer.begin()) - tmpBufferSize;
//             this->_buffer.clear();
//             return (it_start + headerEndPos + ws_http::httpHeaderEnd.size());
//         }
//     }
//     return (it_end);
// }


// HttpRequestBody::HttpRequestBody( void )
//  : maxBodySize(0), _contentLength(0), _chunkSize(0), _splitCase(0)
// { }

// HttpRequestBody::HttpRequestBody( HttpRequest const & request, long maxRequestBodySize )
//  : maxBodySize(maxRequestBodySize), _contentLength(0), _chunkSize(0), _splitCase(0)
// {
//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itContLen = request.headerMap.find("content-length");
//     std::map<std::string, std::string, ws_http::CaInCmp>::const_iterator itTransEnc = request.headerMap.find("transfer-encoding");
//     if (itContLen != request.headerMap.end())
//         this->_contentLength = strtol(itContLen->second.c_str(), NULL, 10);
//     else if (itTransEnc != request.headerMap.end())
//         this->_contentLength = -1;
// }

// HttpRequestBody::~HttpRequestBody( void )
// { }

// buff_t::const_iterator    HttpRequestBody::setChunkSize(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (!this->_buffer.empty() && this->_buffer[this->_buffer.size() - 1] == '\r' && *it_start == '\n') {
//         this->_chunkSize = strtol(std::string(this->_buffer.begin(), this->_buffer.end()).c_str(), NULL, 16);
//         this->_buffer.clear();
//         return (it_start + 1);
//     }
//     if (this->_splitCase != 0) {
//         it_start += this->_splitCase;
//         this->_splitCase = 0;
//     }
//     buff_t::const_iterator itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
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

// ws_http::statuscodes_t  HttpRequestBody::parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end)
// {
//     if (this->_contentLength >= 0) {
//         std::size_t buffDist = std::distance(it_start, it_end);
//         if (this->bodyBuffer.size() + buffDist > this->_contentLength)
//             return (ws_http::STATUS_400_BAD_REQUEST);
//         if (this->bodyBuffer.size() + buffDist > this->maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
//         if (this->bodyBuffer.size() == this->_contentLength)
//             return (ws_http::STATUS_200_OK);
//         return (ws_http::STATUS_UNDEFINED);
//     }
//     buff_t::const_iterator content_start = it_start;
//     while (std::distance(it_start, it_end) > 0) {
//         if (this->_chunkSize == 0) {
//             content_start = this->setChunkSize(it_start, it_end);
//             if (content_start == it_start)
//                 return (ws_http::STATUS_UNDEFINED);
//             if (this->_chunkSize == 0)
//                 return (ws_http::STATUS_200_OK);
//         }
//         std::size_t dist = std::min(std::distance(content_start, it_end), this->_chunkSize);
//         std::size_t rest_dist = std::distance(content_start + dist, it_end);
//         if (this->bodyBuffer.size() + dist > this->maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(content_start, content_start + dist, std::back_inserter(this->bodyBuffer));
//         this->_chunkSize -= dist;
//         if (this->_chunkSize == 0 && rest_dist == 0) {
//             this->_splitCase = 2;
//         } else if (this->_chunkSize == 0 && rest_dist == 1) {
//             this->_splitCase = 1;
//         }
//         it_start = content_start + dist + ws_http::crlf.size();
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }
