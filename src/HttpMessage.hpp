/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:20:26 by mberline          #+#    #+#             */
/*   Updated: 2024/02/15 07:54:45 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

#include "headers.hpp"

class IOnHttpMessageBodySize {
    public:
        IOnHttpMessageBodySize( void ) {};
        virtual ~IOnHttpMessageBodySize( void ) {};
        virtual long    operator()( std::string const & host, std::string const & path ) = 0;
    private:
};

class HttpMessage {
    public:
        enum BodyBools {
            PARSE_SIZE = 0x01,
            CR_FOUND = 0x02,
            HEADER_DONE = 0x04,
            BODY_CHUNKED = 0x08,
            BODY_DONE = 0x10,
            MAKE_CGI = 0x20,
            IS_RESPONSE_FROM_CGI = 0x40,
            RESPONSE_SET = 0x80
        };

        HttpMessage( void );
        HttpMessage( IOnHttpMessageBodySize* maxBodySizeGetter );
        ~HttpMessage( void );

        HttpHeader  header;

        static std::size_t      readBufferSize;
        static char             readBuffer[4096];

        bool                    isCgi( void ) const;
        bool                    responseSet( void ) const;
        ws_http::statuscodes_t  getStatus( void ) const;
        buff_t const &          getBody( void ) const;

        void                    printMessage( void ) const;
        int                     readFromSocketAndParseHttp( int socketfd, int flags);

        void                    setResponse( ws_http::statuscodes_t status, std::istream *data, std::string const & mimeType, std::string const & location );
        int                     sendDataToSocket( int socketfd, int flags );

        void                    prepareCgi( void );
        void                    setCgiResponseDone( void );


    private:

        ws_http::statuscodes_t  parseBody(const char* start, const char* end);
        ws_http::statuscodes_t  parseBodyChunk(unsigned char c);
        ws_http::statuscodes_t  parseBodyChunkSize(unsigned char c);

        buff_t                  _buffer2;
        const char*             _dataPtr;
        IOnHttpMessageBodySize  *_maxBodySizeGetter;
        std::size_t             _contentLength;
        std::size_t             _maxBodySize;
        ws_http::statuscodes_t  _status;
        short                   _state;
        
};

#endif
