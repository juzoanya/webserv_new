/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 11:32:31 by mberline          #+#    #+#             */
/*   Updated: 2024/01/05 09:39:33 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONSTANTS_HPP
#define HTTP_CONSTANTS_HPP

#include "headers.hpp"

#define CRLF "\r\n"
#define WEBSERV_VERSION "webserv/0.0.0"

namespace ws_http {

    typedef enum {
        STATUS_UNDEFINED,
        STATUS_200_OK = 200,
        STATUS_301_MOVED_PERMANENTLY = 301,
        STATUS_302_FOUND = 302,
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
        STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505
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

    extern const std::string header_charstr_forbidden;
    extern const std::string crlf;
    extern const std::string httpHeaderEnd;

    extern const std::map<const std::string, method_t> methods;
    extern const std::map<method_t, const std::string> methods_rev;
    extern const std::map<const std::string, version_t> versions;
    extern const std::map<version_t, const std::string> versions_rev;
    extern const std::map<const std::string, const std::string> mimetypes;
    extern const std::map<statuscodes_t, const std::string>     statuscodes;
    extern const std::map<const std::string, statuscodes_t>     statuscodes_rev;
    extern const std::map<statuscodes_t, const std::string> defaultErrorPages;

    std::string getFileExtension( const std::string& filePath );
}

#endif