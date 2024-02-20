/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParserStatus.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:19:26 by mberline          #+#    #+#             */
/*   Updated: 2023/11/03 18:43:18 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_PARSER_STATUS_HPP
#define HTTP_PARSER_STATUS_HPP

typedef enum {
    WS_HTTP_PARSE_PARSING = 0, // Indicates that the parser is actively parsing the HTTP request.
    WS_HTTP_PARSE_HEADER_DONE, // Signifies that the parsing of the request header is complete, and the parser is moving on to the request body.
    WS_HTTP_PARSE_BODY_DONE,
    WS_HTTP_PARSING_DONE,
    WS_HTTP_PARSE_ERROR_DEFAULT = 0x10, // Marks the starting value for HTTP request parsing error codes.
    WS_HTTP_PARSE_ALREADY_DONE, // Indicates that the parsing is already complete.
    WS_HTTP_PARSE_ERROR_REQUEST_LINE_METHOD, // Indicates an error parsing the request line method.
    WS_HTTP_PARSE_ERROR_REQUEST_LINE_URI, // Indicates an error parsing the request line URI.
    WS_HTTP_PARSE_ERROR_REQUEST_LINE_VERSION, // Indicates an error parsing the request line version.
    WS_HTTP_PARSE_ERROR_HEADER_SYNTAX, // Indicates a syntax error in the request header.
    WS_HTTP_PARSE_ERROR_HEADER_DOUBLE_FIELD, // Indicates an attempt to insert a duplicate header field.
    WS_HTTP_PARSE_ERROR_HEADER_SYNTAX_FIELD, // Indicates a syntax error in a header field.
    WS_HTTP_PARSE_ERROR_HEADER_OVERSIZE, // Indicates that the request header has exceeded the maximum allowed size.
    WS_HTTP_PARSE_ERROR_BODY_OVERSIZE,
    WS_HTTP_PARSE_ERROR_BODY_SYNTAX
} ws_http_parser_status_t;

#endif
