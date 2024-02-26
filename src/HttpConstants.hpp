/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 11:32:31 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 18:43:45 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTP_CONSTANTS_HPP
#define HTTP_CONSTANTS_HPP

#include "headers.hpp"

namespace ws_http {

	typedef enum {
		STATUS_UNDEFINED,
		STATUS_200_OK = 200,
		STATUS_201_CREATED = 201,
		STATUS_204_NO_CONTENT = 204,
		STATUS_300_MULTIPLE_CHOICES = 300,
		STATUS_301_MOVED_PERMANENTLY = 301,
		STATUS_302_FOUND = 302,
		STATUS_303_SEE_OTHER = 303,
		STATUS_304_NOT_MODIFIED = 304,
		STATUS_305_USE_PROXY = 305,
		STATUS_307_TEMPORARY_REDIRECT = 307,
		STATUS_308_PERMANENT_REDIRECT = 308,
		STATUS_400_BAD_REQUEST = 400,
		STATUS_403_FORBIDDEN = 403,
		STATUS_404_NOT_FOUND = 404,
		STATUS_405_METHOD_NOT_ALLOWED = 405,
		STATUS_408_REQUEST_TIMEOUT = 408,
		STATUS_411_LENGTH_REQUIRED = 411,
		STATUS_413_PAYLOAD_TOO_LARGE = 413,
		STATUS_414_URI_TOO_LONG = 414,
		STATUS_415_UNSUPPORTED_MEDIA_TYPE = 415,
		STATUS_429_TOO_MANY_REQUESTS = 429,
		STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
		STATUS_500_INTERNAL_SERVER_ERROR = 500,
		STATUS_501_NOT_IMPLEMENTED = 501,
		STATUS_503_SERVICE_UNAVAILABLE = 503,
		STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505,
		STATUS_1001_INTERNAL_REDIRECT = 1001
	} statuscodes_t;

	typedef enum {
		METHOD_GET,
		METHOD_HEAD,
		METHOD_POST,
		METHOD_PUT,
		METHOD_DELETE,
		METHOD_CONNECT,
		METHOD_OPTIONS,
		METHOD_TRACE,
		METHOD_PATCH
	} method_t;

	typedef enum {
		VERSION_1_0,
		VERSION_1_1,
		VERSION_2_0,
		VERSION_3_0
	} version_t;

	struct CaInCmp {
		bool operator() ( const std::string& str1, const std::string& str2 ) const;
	};

	extern const std::string webservVersion;
	extern const std::string headerTchar;
	extern const std::string uriUnreserved;
	extern const std::string uriReservedDelims;
	extern const std::string uriReservedSubDelims;
	extern const std::string uriUnreserved;
	extern const std::string uriPathOrQueryAllowed;
	extern const std::string crlf;
	extern const std::string httpHeaderEnd;
	extern const std::string dummyValue;

	extern const std::map<method_t, const std::string>      methods;
	extern const std::map<version_t, const std::string>     versions;
	extern const std::map<statuscodes_t, const std::string> statuscodes;
	extern const std::map<const std::string, method_t>      methods_rev;
	extern const std::map<const std::string, version_t>     versions_rev;
	extern const std::map<const std::string, statuscodes_t> statuscodes_rev;
	extern const std::map<const std::string, const std::string> mimetypes;
	extern const std::map<statuscodes_t, const std::string> defaultErrorPages;

	template< typename T, typename U, typename C >
	std::map<U, const T>   wsReverseMap( std::map<const T, U, C> const & ref )
	{
		std::map<U, const T>   mapRev;

		typename std::map<const T, U, C>::const_iterator  it;
		for (it = ref.begin(); it != ref.end(); ++it) {
			mapRev.insert(std::make_pair(it->second, it->first));
		}
		return (mapRev);
	}

}

#endif
