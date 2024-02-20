/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/26 10:51:07 by mberline          #+#    #+#             */
/*   Updated: 2023/12/07 11:49:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include "HttpHeaderDef.hpp"
#include "parser/HttpHeaderParser.hpp"
#include "../socketapi/include/ws_sockapi.hpp"
#include "directoryListing/FileInfo.hpp"

#define WS_HTTP_RES_DEFAULT_HEADER_SIZE 4096


class HttpResponse {
public:
    HttpResponse( void );
    HttpResponse(ClientSock& client, std::size_t timeoutS, std::size_t maxrequests);
    ~HttpResponse(void);
    
    HttpHeaderParser&   getHeaders( void );
    int     sendFile(std::string const & filePath);
    int     sendDirectory(std::string const & rootPath, std::string const & dirPath);
    int     sendError(ws_http::statuscodes_t errorCode, std::string const & errorPagePath = "");
    void    sendText(std::string const & str);
    void    sendBufferRange( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    void    sendRedirect(ws_http::statuscodes_t redirectCode, std::string const & newLocation );

    ws_http::statuscodes_t                                  statuscode;             // Status code of the response.
private:
    int     flushResponse(void);
    void    findSetContentTypeLength( std::string const & filePath, std::size_t len );
    void    setContentTypeLength( std::string const & mimeType, std::size_t len );
    buff_t                                                  _resHeader;             // Buffer for storing response headers.
    buff_t                                                  _resBody;               // Buffer for storing the response body.
   
    HttpHeaderParser    _headers;

    ws_http::version_t                                      _httpVersion;           // HTTP version for the response.
    ClientSock*                                             _client;                // Pointer to the client socket.
    
    bool                                                    _isFlushed;             // Flag to indicate if the response has been flushed.
};



// typedef enum {
//     WS_HTTP_RESPONSE_ERROR_FILE_OK,
//     WS_HTTP_RESPONSE_ERROR_FILE_PERMISSION,
//     WS_HTTP_RESPONSE_ERROR_FILE_NOT_FOUND,
//     WS_HTTP_RESPONSE_ERROR_IS_DIR
// } ws_http_response_status_t;


// class HttpResponse {
// public:
//     HttpResponse(ClientSock& client, std::size_t timeoutS, std::size_t maxrequests);
//     ~HttpResponse(void);
    
//     HttpHeaderParser&   getHeaders( void );
//     int sendFile(std::string const & filePath);
//     int sendDirectory(std::string const & rootPath, std::string const & dirPath);
//     int sendDefaultError(ws_http::statuscodes_t errorCode);
//     int sendError(ws_http::statuscodes_t errorCode, std::string const & errorPagePath = "");
//     void    sendText(std::string const & str);
//     void    sendBufferRange( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
//     void    sendRedirect(ws_http::statuscodes_t redirectCode, std::string const & newLocation );

// private:
//     int     flushResponse(void);
//     void    findSetContentTypeLength( std::string const & filePath, std::size_t len );
//     void    setContentTypeLength( std::string const & mimeType, std::size_t len );
//     ws_http::statuscodes_t                                  statuscode;             // Status code of the response.
//     buff_t                                                  _resHeader;             // Buffer for storing response headers.
//     buff_t                                                  _resBody;               // Buffer for storing the response body.
   
//     HttpHeaderParser    _headers;

//     ws_http::version_t                                      _httpVersion;           // HTTP version for the response.
//     ClientSock*                                             _client;                // Pointer to the client socket.
    
//     bool                                                    _isFlushed;             // Flag to indicate if the response has been flushed.
// };





// /**
//  * @class HttpResponse
//  * @brief Responsible for generating HTTP responses in an HTTP server.
//  *
//  * This class provides functionality for constructing and sending HTTP responses to clients
//  * in response to incoming HTTP requests. 
//  *
//  */
// class HttpResponse {
// public:
//     /**
//      * @brief Constructor for HttpResponse.
//      *
//      * Initializes an instance of the HttpResponse class for handling HTTP responses.
//      *
//      * @param client A reference to the client socket for sending the response.
//      */
//     HttpResponse(ClientSock& client);

//     /**
//      * @brief Destructor for HttpResponse.
//      */
//     ~HttpResponse(void);

//     /**
//      * @brief Set an HTTP header field with the specified field type and value.
//      *
//      * This method allows you to set an HTTP header field with a predefined field type and the corresponding value.
//      * Use this function to add standard HTTP header fields to the response.
//      *
//      * @param headerField The type of HTTP header field to set, represented by the `ws_http::header_field_t` enum.
//      * @param headerValue The value to set for the specified header field.
//      */
//     void setHeader(ws_http::header_field_t headerField, std::string headerValue);

//     /**
//      * @brief Set a custom HTTP header field with a custom field name and value.
//      *
//      * This method allows you to set a custom HTTP header field with a custom field name and the corresponding value.
//      * Use this function to add non-standard or custom header fields to the response.
//      *
//      * @param headerField The name of the custom HTTP header field.
//      * @param headerValue The value to set for the custom header field.
//      */
//     void setHeader(std::string headerField, std::string headerValue);

//     /**
//      * @brief Remove an HTTP header field with the specified field type.
//      *
//      * Remove an HTTP header field by its predefined field type. This function can be used to remove previously set standard HTTP header fields.
//      *
//      * @param headerField The type of HTTP header field to remove, represented by the `ws_http::header_field_t` enum.
//      * @return true if the field was successfully removed; false if the field does not exist.
//      */
//     bool removeHeader(ws_http::header_field_t headerField);

//     /**
//      * @brief Remove a custom HTTP header field with a custom field name.
//      *
//      * Remove a custom HTTP header field by its custom field name. This function can be used to remove previously set custom header fields.
//      *
//      * @param headerField The name of the custom HTTP header field to remove.
//      * @return true if the field was successfully removed; false if the field does not exist.
//      */
//     bool removeHeader(std::string headerField);

//     /**
//      * @brief Get the value of an HTTP header field by its specified field type.
//      *
//      * Retrieve the value of a predefined HTTP header field by its field type.
//      *
//      * @param headerField The type of HTTP header field to retrieve, represented by the `ws_http::header_field_t` enum.
//      * @return The value of the specified header field. If the field does not exist, an empty string is returned.
//      */
//     const std::string& getHeader(ws_http::header_field_t headerField) const;

//     /**
//      * @brief Get the value of a custom HTTP header field by its custom field name.
//      *
//      * Retrieve the value of a custom HTTP header field by its custom field name.
//      *
//      * @param headerField The name of the custom HTTP header field to retrieve.
//      * @return The value of the specified custom header field. If the field does not exist, an empty string is returned.
//      */
//     const std::string& getHeader(std::string headerField) const;

//     /**
//      * @brief Check if an HTTP header field is set by its specified field type.
//      *
//      * Check whether a predefined HTTP header field is set by its field type.
//      *
//      * @param headerField The type of HTTP header field to check, represented by the `ws_http::header_field_t` enum.
//      * @return true if the field is set; false if the field is not set.
//      */
//     bool headerIsSet(ws_http::header_field_t headerField) const;

//     /**
//      * @brief Check if a custom HTTP header field is set by its custom field name.
//      *
//      * Check whether a custom HTTP header field is set by its custom field name.
//      *
//      * @param headerField The name of the custom HTTP header field to check.
//      * @return true if the field is set; false if the field is not set.
//      */
//     bool headerIsSet(std::string headerField) const;

//     /**
//      * @brief Set the response body by reading a file from the specified file path.
//      *
//      * Reads the contents of the file specified by the file path and sets it as the response body.
//      * The function also sets the appropriate content type based on the file's extension.
//      *
//      * @param filePath The path to the file to be sent as the response body.
//      * @return 0 if the file was successfully set; otherwise, an error code indicating the issue.
//      */
//     int setFileToSend(std::string const & filePath);

//     /**
//      * @brief Flush the HTTP response to the client.
//      *
//      * Constructs the final HTTP response by combining the response headers and body.
//      * Then, the response is sent to the client through the associated socket.
//      *
//      * @return 0 if the response was successfully sent; -1 if the response has already been flushed.
//      */
//     int flushResponse(void);

//     /**
//      * @brief Send the contents of a file as the response body.
//      *
//      * Sends the contents of a file specified by the file path as the response body.
//      * It sets default headers, like the appropriate content type based on the file's extension and will call flushResponse to submit the Response
//      *
//      * @param filePath The path to the file to be sent as the response body.
//      * @return 0 if the file was successfully sent; otherwise, an error code indicating the issue.
//      */
//     int sendFile(std::string const & rootPath, std::string const & fileName);

//     /**
//      * @brief Send a default error response based on the specified HTTP status code.
//      *
//      * Sends a default error response with the provided HTTP status code. The response includes an error page corresponding to the status code.
//      *
//      * @param errorCode The HTTP status code for the error response.
//      * @return 0 if the error response was successfully sent; -1 if the response has already been flushed.
//      */
//     int sendDefaultError(ws_http::statuscodes_t errorCode);

//     /**
//      * @brief Send an error response based on the specified HTTP status code.
//      *
//      * Sends an error response with the provided HTTP status code. If an error page path is provided, it is used for the response body; otherwise, a default error page is used.
//      *
//      * @param errorCode The HTTP status code for the error response.
//      * @param errorPagePath The path to a custom error page to use as the response body (optional).
//      * @return 0 if the error response was successfully sent; -1 if the response has already been flushed.
//      */
//     int sendError(ws_http::statuscodes_t errorCode, std::string const & errorPagePath = "");

//     /**
//      * @brief Send a plain text response.
//      *
//      * Sends a plain text response with the provided string as the response body.
//      *
//      * @param str The plain text content to be sent as the response body.
//      * @return 0 if the plain text response was successfully sent; -1 if the response has already been flushed.
//      */
//     int sendText(std::string const & str);

//     ws_http::statuscodes_t                                  statuscode;             // Status code of the response.

// private:
//     buff_t                                                  _resHeader;             // Buffer for storing response headers.
//     buff_t                                                  _resBody;               // Buffer for storing the response body.
//     ws_http::version_t                                      _httpVersion;           // HTTP version for the response.
//     std::map<ws_http::header_field_t, std::string>          _resHeaderFields;       // Map of HTTP header fields.
//     std::map<std::string, std::string, ws_http::CaInCmp>    _resCustomHeaderFields; // Map of custom header fields.
//     std::string                                             _dummyEmptyHeader;      // Placeholder for an empty header value.
//     ClientSock*                                             _client;                // Pointer to the client socket.
//     bool                                                    _isFlushed;             // Flag to indicate if the response has been flushed.
//     std::size_t                                             _contentLength;         // Content length of the response body.
// };


#endif
