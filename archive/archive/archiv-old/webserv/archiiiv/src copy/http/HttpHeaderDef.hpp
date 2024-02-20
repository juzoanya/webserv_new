/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHeaderDef.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 09:37:11 by mberline          #+#    #+#             */
/*   Updated: 2023/11/15 08:57:32 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HTTP_HEADER_DEF_HPP
#define WEBSERV_HTTP_HEADER_DEF_HPP

#include <map>
#include <string>
#include <vector>

#define CRLF "\r\n"
#define WEBSERV_VERSION "webserv/0.0.0"

typedef std::vector<char> buff_t;

namespace ws_http {


    typedef enum {

        /* Successful responses */

        /**
         * 200 OK
         * The request succeeded. The result meaning of "success" depends on the HTTP method:
         * GET: The resource has been fetched and transmitted in the message body.
         * HEAD: The representation headers are included in the response without any message body.
         * PUT or POST: The resource describing the result of the action is transmitted in the message body.
         * TRACE: The message body contains the request message as received by the server.
         */
        STATUS_200_OK = 200,

        /* Redirection messages */

        /**
         * 301 Moved Permanently
         * The URL of the requested resource has been changed permanently.
         * The new URL is given in the response.
         */
        STATUS_301_MOVED_PERMANENTLY = 301,

        /**
         * 302 Found
         * This response code means that the URI of the requested resource has
         * been changed temporarily. Further changes in the URI might be made
         * in the future. Therefore, this same URI should be used by the
         * client in future requests.
         */
        STATUS_302_FOUND = 302,

        /* Client error responses */

        /**
         * 400 Bad Request
         * The server cannot or will not process the request due to something that is 
         * perceived to be a client error (e.g., malformed request syntax, 
         * invalid request message framing, or deceptive request routing).
         */
        STATUS_400_BAD_REQUEST = 400,

        /**
         * 403 Forbidden
         * The client does not have access rights to the content; that is, 
         * it is unauthorized, so the server is refusing to give the requested resource. 
         * Unlike 401 Unauthorized, the client's identity is known to the server.
         */
        STATUS_403_FORBIDDEN = 403,

        /**
         * 404 Not Found
         * The server cannot find the requested resource. In the browser,
         * this means the URL is not recognized. In an API, this can also mean that the endpoint
         * is valid but the resource itself does not exist. Servers may also send this response instead
         * of 403 Forbidden to hide the existence of a resource from an unauthorized client. 
         * This response code is probably the most well known due to its frequent occurrence on the web.
         */
        STATUS_404_NOT_FOUND = 404,

        /**
         * 405 Method Not Allowed
         * The request method is known by the server but is not supported
         * by the target resource. For example, an API may not allow calling
         * DELETE to remove a resource.
         */
        STATUS_405_METHOD_NOT_ALLOWED = 405,

        /**
         * 408 Request Timeout
         * This response is sent on an idle connection by some servers,
         * even without any previous request by the client.
         * It means that the server would like to shut down this unused connection.
         * This response is used much more since some browsers, like Chrome,
         * Firefox 27+, or IE9, use HTTP pre-connection mechanisms to speed up surfing.
         * Also note that some servers merely shut down the connection without sending this message.
         */
        STATUS_408_REQUEST_TIMEOUT = 408,

        /**
         * 411 Length Required
         * Server rejected the request because the Content-Length header
         * field is not defined and the server requires it.       
         */
        STATUS_411_LENGTH_REQUIRED = 411,

        /**
         * 413 Payload Too Large
         * Request entity is larger than limits defined by server.
         * The server might close the connection or return an Retry-After header field.
         */
        STATUS_413_PAYLOAD_TOO_LARGE = 413,

        /**
         * 414 URI Too Long
         * The URI requested by the client is longer than the server is willing to interpret.
         */
        STATUS_414_URI_TOO_LONG = 414,

        /**
         * 415 Unsupported Media Type
         * The media format of the requested data is not supported by the
         * server, so the server is rejecting the request.
         */
        STATUS_415_UNSUPPORTED_MEDIA_TYPE = 415,

        /**
         * 429 Too Many Requests
         * The user has sent too many requests in a given amount of time ("rate limiting").
         */
        STATUS_429_TOO_MANY_REQUESTS = 429,

        /**
         * 431 Request Header Fields Too Large
         * The server is unwilling to process the request because its header fields are too large.
         * The request may be resubmitted after reducing the size of the request header fields.     
         */
        STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,

        /* Server error responses */

        /**
         * 500 Internal Server Error
         * The server has encountered a situation it does not know how to handle.
         */
        STATUS_500_INTERNAL_SERVER_ERROR = 500,

        /**
         * 501 Not Implemented
         * The request method is not supported by the server and cannot be handled.
         * The only methods that servers are required to support
         * (and therefore that must not return this code) are GET and HEAD.
         */
        STATUS_501_NOT_IMPLEMENTED = 501,

        /**
         * 503 Service Unavailable
         * The server is not ready to handle the request.
         * Common causes are a server that is down for maintenance or that is overloaded.
         * Note that together with this response, a user-friendly page explaining the problem should be sent.
         * This response should be used for temporary conditions and the Retry-After HTTP header should,
         * if possible, contain the estimated time before the recovery of the service.
         * The webmaster must also take care about the caching-related headers that are sent along with this response,
         * as these temporary condition responses should usually not be cached.
         */
        STATUS_503_SERVICE_UNAVAILABLE = 503,

        /**
         * 505 HTTP Version Not Supported
         * The HTTP version used in the request is not supported by the server.
         */
        STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505
    } statuscodes_t;

    typedef enum {
        /**
         * @brief Represents an invalid or unsupported HTTP method.
         * This method is used to indicate that the HTTP request contains an unknown or unsupported method.
         */
        METHOD_INVALID = -1,

        /**
         * @brief Represents the HTTP GET method.
         * The GET method is used to retrieve data from the server. It requests a representation of the specified resource.
         */
        METHOD_GET,

        /**
         * @brief Represents the HTTP HEAD method.
         * The HEAD method is identical to GET, but it only retrieves the headers of the response without the message body.
         */
        METHOD_HEAD,

        /**
         * @brief Represents the HTTP POST method.
         * The POST method is used to submit data to be processed to a specified resource, often causing a change in server state or side effects on the server.
         */
        METHOD_POST,

        /**
         * @brief Represents the HTTP PUT method.
         * The PUT method is used to update or create a resource on the server. It replaces the current representation of the target resource.
         */
        METHOD_PUT,

        /**
         * @brief Represents the HTTP DELETE method.
         * The DELETE method is used to request the removal of a resource from the server.
         */
        METHOD_DELETE,

        /**
         * @brief Represents the HTTP CONNECT method.
         * The CONNECT method establishes a network connection to a resource, usually for use with a proxy.
         */
        METHOD_CONNECT,

        /**
         * @brief Represents the HTTP OPTIONS method.
         * The OPTIONS method is used to describe the communication options for the target resource, including allowed methods, supported media types, and more.
         */
        METHOD_OPTIONS,

        /**
         * @brief Represents the HTTP TRACE method.
         * The TRACE method echoes the received request so that the client can see what changes or additions have been made by intermediate servers.
         */
        METHOD_TRACE,

        /**
         * @brief Represents the HTTP PATCH method.
         * The PATCH method is used to apply partial modifications to a resource, allowing it to be updated with smaller, more efficient requests.
         */
        METHOD_PATCH
    } method_t;

    typedef enum {
        /**
         * @brief Represents an invalid or unsupported HTTP version.
         * This version is used to indicate that the HTTP request specifies an unknown or unsupported HTTP protocol version.
         */
        VERSION_INVALID = -1,

        /**
         * @brief Represents the HTTP/1.0 version.
         * HTTP/1.0 is the original version of the HTTP protocol and provides basic functionality for web communications.
         */
        VERSION_1_0,

        /**
         * @brief Represents the HTTP/1.1 version.
         * HTTP/1.1 is an updated version of the HTTP protocol, introducing several improvements and features for enhanced performance and functionality.
         */
        VERSION_1_1,

        /**
         * @brief Represents the HTTP/2.0 version.
         * HTTP/2.0 is a major revision of the HTTP protocol, designed to provide improved speed and efficiency for web communications.
         */
        VERSION_2_0,

        /**
         * @brief Represents the HTTP/3.0 version.
         * HTTP/3.0 is the latest version of the HTTP protocol, further optimizing web communication by using a new transport protocol.
         */
        VERSION_3_0
    } version_t;

    typedef enum {
        HEADER_INVALID,
        /* General Header Fields */

        /**
         * @brief Represents the Cache-Control header field.
         * The Cache-Control header field is used to specify caching directives in requests and responses. It provides control over how and for how long a response is cached.
         */
        HEADER_CACHE_CONTROL,

        /**
         * @brief Represents the Connection header field.
         * The Connection header field allows the sender to specify options that are desired for that particular connection and must not be communicated by proxies over further connections.
         */
        HEADER_CONNECTION,

        /**
         * @brief Represents the Date header field.
         * The Date header field represents the date and time when the message was sent. It is used to indicate the age and validity of the message.
         */
        HEADER_DATE,

        /**
         * @brief Represents the Pragma header field.
         * The Pragma header field is used for backward compatibility with HTTP/1.0 and allows clients to include implementation-specific directives that might apply to any recipient, along the request-response chain.
         */
        HEADER_PRAGMA,

        /**
         * @brief Represents the Trailer header field.
         * The Trailer header field indicates that the given set of header fields is present in the trailer of a message encoded with chunked transfer coding.
         */
        HEADER_TRAILER,

        /**
         * @brief Represents the Transfer-Encoding header field.
         * The Transfer-Encoding header field specifies the form of encoding used to safely transfer the entity to the user. It can indicate that the message is chunked or compressed, for example.
         */
        HEADER_TRANSFER_ENCODING,

        /**
         * @brief Represents the Upgrade header field.
         * The Upgrade header field is used to specify additional communication protocols that the sender would like to switch to for the current connection.
         */
        HEADER_UPGRADE,

        /**
         * @brief Represents the Via header field.
         * The Via header field indicates the network path taken by the request between the client and the server. It is typically added by intermediaries such as proxies.
         */
        HEADER_VIA,

        /**
         * @brief Represents the Warning header field.
         * The Warning header field is used to carry additional information about the status or transformation of a message. It is often included in cache-related responses.
         */
        HEADER_WARNING,

        /* Request Header Fields */
        
        /**
         * @brief Represents the Accept request header field.
         * The Accept request header field indicates what media types, or range of media types, the client can process. It is used by the client to communicate its preferences for the response.
         */
        HEADER_ACCEPT,

        /**
         * @brief Represents the Accept-Encoding request header field.
         * The Accept-Encoding request header field specifies the client's preferred media types for the response, allowing the server to choose the most appropriate content encoding.
         */
        HEADER_ACCEPT_ENCODING,

        /**
         * @brief Represents the Accept-Language request header field.
         * The Accept-Language request header field indicates the natural languages that are preferred for the response. It helps the server determine the best content localization.
         */
        HEADER_ACCEPT_LANGUAGE,

        /**
         * @brief Represents the Authorization request header field.
         * The Authorization request header field contains credentials for authenticating the client with the server. It is typically used for HTTP basic authentication.
         */
        HEADER_AUTHORIZATION,

        /**
         * @brief Represents the Expect request header field.
         * The Expect request header field indicates client expectations for the server's behavior. It is used to indicate whether certain conditions must be met before the request is processed.
         */
        HEADER_EXPECT,

        /**
         * @brief Represents the From request header field.
         * The From request header field provides the email address of the user making the request. It can be used for informational or error reporting purposes.
         */
        HEADER_FROM,

        /**
         * @brief Represents the Host request header field.
         * The Host request header field specifies the domain name and port number of the server. It is mandatory in HTTP/1.1 requests and helps the server identify the intended destination.
         */
        HEADER_HOST,

        /**
         * @brief Represents the If-Match request header field.
         * The If-Match request header field is used to make a conditional request based on the ETag values of one or more entities. It allows the request to proceed only if the ETags match.
         */
        HEADER_IF_MATCH,

        /**
         * @brief Represents the If-Modified-Since request header field.
         * The If-Modified-Since request header field is used for conditional requests. It specifies that the request should only be processed if the resource has been modified since the given date.
         */
        HEADER_IF_MODIFIED_SINCE,

        /**
         * @brief Represents the If-None-Match request header field.
         * The If-None-Match request header field is used for conditional requests. It specifies that the request should only be processed if the resource has not been modified since the given ETag values.
         */
        HEADER_IF_NONE_MATCH,

        /**
         * @brief Represents the If-Range request header field.
         * The If-Range request header field is used in conditional requests. It allows a client to request only the portion of the resource that has changed since a given ETag or modification date.
         */
        HEADER_IF_RANGE,

        /**
         * @brief Represents the If-Unmodified-Since request header field.
         * The If-Unmodified-Since request header field is used for conditional requests. It specifies that the request should only be processed if the resource has not been modified since the given date.
         */
        HEADER_IF_UNMODIFIED_SINCE,

        /**
         * @brief Represents the Max-Forwards request header field.
         * The Max-Forwards request header field limits the number of times a request can be forwarded by proxies. It helps prevent request loops in proxy chains.
         */
        HEADER_MAX_FORWARDS,

        /**
         * @brief Represents the Proxy-Authorization request header field.
         * The Proxy-Authorization request header field contains credentials for authenticating the client with a proxy server. It is similar to the Authorization header but is used for proxy authentication.
         */
        HEADER_PROXY_AUTHORIZATION,

        /**
         * @brief Represents the Range request header field.
         * The Range request header field is used to request only a portion of a resource, specified by a byte range. It allows clients to retrieve partial content, such as video segments.
         */
        HEADER_RANGE,

        /**
         * @brief Represents the Referer request header field.
         * The Referer request header field indicates the address of the web page that linked to the current page. It helps servers track the source of incoming requests.
         */
        HEADER_REFERER,

        /**
         * @brief Represents the TE request header field.
         * The TE request header field specifies the transfer codings that the client is willing to accept. It allows the client to indicate its preferences for content encoding.
         */
        HEADER_TE,

        /**
         * @brief Represents the User-Agent request header field.
         * The User-Agent request header field provides information about the user agent (client) making the request. It helps servers tailor responses based on the client's capabilities and characteristics.
         */
        HEADER_USER_AGENT,

        /* Response Header Fields */
        
        /**
         * @brief Represents the Accept-Ranges response header field.
         * The Accept-Ranges response header field indicates what byte ranges are supported by the server for a particular resource. It helps clients determine if they can make partial content requests.
         */
        HEADER_ACCEPT_RANGES,

        /**
         * @brief Represents the Age response header field.
         * The Age response header field conveys the time a response has been held in a proxy cache. It helps clients understand the age of the cached response.
         */
        HEADER_AGE,

        /**
         * @brief Represents the ETag response header field.
         * The ETag response header field provides a unique identifier for a resource's version. Clients can use it to perform conditional requests and check for resource modifications.
         */
        HEADER_ETAG,

        /**
         * @brief Represents the Location response header field.
         * The Location response header field is used in redirection responses (e.g., 301, 302) to indicate the new URL where the client should continue the request. It helps clients navigate to the new resource.
         */
        HEADER_LOCATION,

        /**
         * @brief Represents the Proxy-Authenticate response header field.
         * The Proxy-Authenticate response header field is used by a proxy server to challenge the client's credentials. It indicates the authentication method required for accessing the resource.
         */
        HEADER_PROXY_AUTHENTICATE,

        /**
         * @brief Represents the Retry-After response header field.
         * The Retry-After response header field is used to specify when the client should retry its request after receiving a 503 (Service Unavailable) response. It can indicate a date or a time delay.
         */
        HEADER_RETRY_AFTER,

        /**
         * @brief Represents the Server response header field.
         * The Server response header field provides information about the software used by the origin server. It helps clients and operators identify the server software.
         */
        HEADER_SERVER,

        /**
         * @brief Represents the Vary response header field.
         * The Vary response header field specifies the request headers that were used to determine the cache's content selection. It informs clients about the request headers that can affect cache selection.
         */
        HEADER_VARY,

        /**
         * @brief Represents the WWW-Authenticate response header field.
         * The WWW-Authenticate response header field challenges the client to provide credentials for accessing the resource. It indicates the authentication method required by the server.
         */
        HEADER_WWW_AUTHENTICATE,

        /**
         * @brief Represents the Content-Encoding response header field.
         * The Content-Encoding response header field specifies the encoding transformations applied to the resource. It helps clients decode the response correctly.
         */
        HEADER_CONTENT_ENCODING,

        /**
         * @brief Represents the Content-Length response header field.
         * The Content-Length response header field indicates the size of the message body in octets (8-bit bytes). It helps clients process and display the response content.
         */
        HEADER_CONTENT_LENGTH,

        /**
         * @brief Represents the Content-Type response header field.
         * The Content-Type response header field specifies the media type of the response content. It helps clients determine how to interpret and render the content.
         */
        HEADER_CONTENT_TYPE,

        /**
         * @brief Represents the Content-Language response header field.
         * The Content-Language response header field specifies the natural language(s) of the response content. It helps clients select the appropriate language for rendering the content.
         */
        HEADER_CONTENT_LANGUAGE,

        /**
         * @brief Represents the Content-Location response header field.
         * The Content-Location response header field provides the URI of the response's preferred representation. It is used to identify the location of the resource.
         */
        HEADER_CONTENT_LOCATION,

        /**
         * @brief Represents the Content-MD5 response header field.
         * The Content-MD5 response header field contains the MD5 digest of the response content. It can be used for integrity checks and ensuring that the content has not been altered in transit.
         */
        HEADER_CONTENT_MD5,

        /**
         * @brief Represents the Content-Range response header field.
         * The Content-Range response header field is used to indicate the range of content enclosed within the message body. It is often used for serving partial content.
         */
        HEADER_CONTENT_RANGE,

        /**
         * @brief Represents the Last-Modified response header field.
         * The Last-Modified response header field specifies the date and time when the resource was last modified. It helps clients determine if the resource has changed.
         */
        HEADER_LAST_MODIFIED,

        /**
         * @brief Represents the Set-Cookie response header field.
         * The Set-Cookie response header field is used to send a cookie from the server to the client. It allows the server to maintain session information and user preferences.
         */
        HEADER_SET_COOKIE

    } header_field_t;

    struct CaInCmp {
        bool operator() ( const std::string& str1, const std::string& str2 ) const;
    };

    /**
     * @brief A string containing the forbidden characters for HTTP headers.
     * This string contains characters that are not allowed in HTTP header fields.
     */
    extern const std::string header_charstr_forbidden;

    /**
     * @brief A string representing the CRLF (Carriage Return and Line Feed) sequence.
     * This string contains the CRLF sequence used in HTTP for line termination.
     */
    extern const std::string crlf;

    extern const std::string httpHeaderEnd;

    /**
     * @brief A mapping of HTTP methods to their string representations.
     * This map associates HTTP methods (e.g., GET, POST) with their string representations.
     */
    extern const std::map<const std::string, method_t> methods;

    /**
     * @brief A mapping of string representations of HTTP methods to their corresponding methods.
     * This map provides a reverse lookup for HTTP methods, allowing you to find the method from its string representation.
     */
    extern const std::map<method_t, const std::string> methods_rev;

    /**
     * @brief A mapping of HTTP protocol versions to their string representations.
     * This map associates HTTP protocol versions (e.g., HTTP/1.1) with their string representations.
     */
    extern const std::map<const std::string, version_t> versions;

    /**
     * @brief A mapping of string representations of HTTP protocol versions to their corresponding versions.
     * This map provides a reverse lookup for HTTP protocol versions, allowing you to find the version from its string representation.
     */
    extern const std::map<version_t, const std::string> versions_rev;

    /**
     * @brief A mapping of HTTP header field strings to their corresponding enumeration values.
     * This map associates HTTP header field strings (e.g., "Content-Type") with their enumeration values.
     */
    extern const std::map<const std::string, header_field_t, CaInCmp> headers;

    /**
     * @brief A mapping of HTTP header field enumeration values to their string representations.
     * This map provides a reverse lookup for HTTP header field values, allowing you to find the string representation of a field.
     */
    extern const std::map<header_field_t, const std::string> headers_rev;

    /**
     * @brief A mapping of file extensions to MIME types.
     * This map associates file extensions (e.g., ".html") with their corresponding MIME types (e.g., "text/html").
     */
    extern const std::map<const std::string, const std::string> mimetypes;

    /**
     * @brief A mapping of HTTP status codes to their string representations.
     * This map associates HTTP status codes of the type statuscodes_t (e.g., STATUS_200_OK (200) ) with their string representations (e.g., "200 OK").
     */
    extern const std::map<statuscodes_t, const std::string>     statuscodes;

    /**
     * @brief A mapping of HTTP status codes string representations to their enummeration value.
     * reverse loopup to get the statuscodes_t enummeration value of a statuscode string (e.g. check lowerbound)
     */
    extern const std::map<const std::string, statuscodes_t>     statuscodes_rev;

    /**
     * @brief A mapping of default error pages for HTTP status codes.
     * This map associates HTTP status codes with default error page HTML contents.
     */
    extern const std::map<statuscodes_t, const std::string> defaultErrorPages;

    typedef std::map<statuscodes_t, const std::string>              mp_status_string_t;
    typedef std::map<const std::string, header_field_t, CaInCmp>    mp_string_header_t;
    typedef std::map<header_field_t, const std::string>             mp_header_string_t;
    typedef std::map<const std::string, method_t>                   mp_string_method_t;
    typedef std::map<method_t, const std::string>                   mp_method_string_t;
    typedef std::map<const std::string, version_t>                  mp_string_version_t;
    typedef std::map<version_t, const std::string>                  mp_version_string_t;
    typedef std::map<const std::string, const std::string>          mp_string_t;
    typedef std::map<std::string, std::string, ws_http::CaInCmp>    mp_string_ci_t;

    void writeString( std::string const & str, buff_t& buffer );
    void writeStatusLine( std::string const & version, std::string const & status, buff_t& buffer );
    void writeHeader( std::string const & key, std::string const & value, buff_t& buffer );
    std::string getFileExtension( const std::string& filePath );
}


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


#endif
