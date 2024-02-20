/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 08:53:26 by mberline          #+#    #+#             */
/*   Updated: 2024/02/12 19:54:30 by mberline         ###   ########.fr       */
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
        typedef std::vector< std::pair<std::string, std::string> >    headers_t;
        
        HttpMessage( void );
        HttpMessage( IOnHttpMessageBodySize* maxBodySizeGetter );
        virtual ~HttpMessage( void );

        int     readFromSocketAndParseHttp( int socketfd, int flags, bool parseReqline );
        int     sendDataToSocket( int socketfd, int flags );
        void    setMaxBodySize( long maxBodySize );
        HttpMessage&    setHeader( std::string const & key, std::string const & value );
        HttpMessage&    clearHeader( void );
        HttpMessage&    clearBody( void );
        HttpMessage&    setBody( std::istream & data, std::string const & mimeType );
        HttpMessage&    setBodyError( ws_http::statuscodes_t errorStatus );
        HttpMessage&    setStatusLine( ws_http::version_t httpVersion, ws_http::statuscodes_t statusCode );
        HttpMessage&    setStatusLine( ws_http::version_t httpVersion, std::string const & status );
        HttpMessage&    setRequestLine( ws_http::method_t mehthod, std::string const & requestTarget, ws_http::version_t httpVersion );
        HttpMessage&    flushMessage( void );
        bool            readyToSend( void ) const;
        bool            sendDone( void ) const;
        void            resetMessage( void );

        bool                    headerDone( void ) const;
        std::string const &     getHeader( std::string const & key ) const;
        bool                    hasHeader( std::string const & key ) const;
        buff_t const &          getBody( void ) const;
        headers_t const &       getHeaderVector( void ) const;
        ws_http::statuscodes_t  getStatus( void ) const;
        void                    printMessage( void ) const;

        static const std::size_t    readBufferSize;
        static std::vector<char>    readBuffer;
        static const std::string    method;
        static const std::string    path;
        static const std::string    pathDecoded;
        static const std::string    query;
        static const std::string    fullUri;
        static const std::string    version;

        ws_http::statuscodes_t  parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
    protected:
        enum EnumBits { SEND_RESPONSE, SEND_HEADER_DONE, SEND_DONE, HEADER_DONE, BODY_DONE, CR_FOUND, HEADER_PARSE_KEY, PARSE_SIZE, HEADER_VALUE_START_FOUND, BITS_MAX };
        enum ReqLinePos { REQLINE_PARSE_METHOD, REQLINE_PARSE_PATH, REQLINE_PARSE_QUERY, REQLINE_PARSE_VERSION, REQLINE_DONE };
        enum EnumNbrsHeader { NBR_HEADER_WHITEPOS_LEN };
        enum EnumNbrsBody { NBR_BODY_CONTENT_LENGTH, NBR_BODY_CHUNK_SIZE };


        void    insertHeaderClearValue(std::string const & key, buff_t & buffer);
        void    insertHeaderClearKeyValue(buff_t & key, buff_t & buffer);
        ws_http::statuscodes_t  setStatus( ws_http::statuscodes_t statusCode );
        ws_http::statuscodes_t  parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
        ws_http::statuscodes_t  parseHeaderLine(unsigned char c);
        ws_http::statuscodes_t  parseMethod(unsigned char c);
        ws_http::statuscodes_t  parsePath(unsigned char c);
        ws_http::statuscodes_t  parseQuery(unsigned char c);
        ws_http::statuscodes_t  parseVersion(unsigned char c);

        headers_t               _headerMap;
        // std::string             _buffer1;
        // std::string             _buffer2;
        buff_t             _buffer1;
        buff_t             _buffer2;
        ws_http::statuscodes_t  _status;
        IOnHttpMessageBodySize* _maxBodySizeGetter;
        std::bitset<BITS_MAX>   _bits;
        long                    _contentLength;
        long                    _maxBodySize;
        unsigned int            _chunkSize;
        char                    _reqLinePos;

        const char* headerPtr;
        std::size_t headerSize;
        const char* bodyPtr;
        std::size_t bodySize;
        // std::array<bool, BITS_MAX>  _bits;
};

template<typename T>
std::string printBool(T t)
{
    // std::cout << "bool: " << t << std::endl;
    if (t == 1)
        return (std::string("true"));
    return (std::string("false"));
}

#endif

// #ifndef HTTP_MESSAGE_HPP
// #define HTTP_MESSAGE_HPP

// #include "headers.hpp"

// class OnHttpMessageBodySize {
//     public:
//         OnHttpMessageBodySize( void );
//         virtual ~OnHttpMessageBodySize( void );
//         long    operator()( std::string const & host, std::string const & path );
//     private:
// };

// // struct HttpRequestLine {
// //     HttpRequestLine( void );
// //     HttpRequestLine( ws_http::statuscodes_t statusCode );
// //     ~HttpRequestLine( void );
// //     ws_http::statuscodes_t  status;
// //     std::string  method;
// //     std::string  path;
// //     std::string  pathDecoded;
// //     std::string  query;
// //     std::string  fullUri;
// //     std::string  version;
// // };

// class HttpMessage {
//     public:
//         HttpMessage( void );
//         HttpMessage( OnHttpMessageBodySize* maxBodySizeGetter );
//         virtual ~HttpMessage( void );

//         typedef std::vector< std::pair<std::string, std::string> >    headers_t;
//         // static HttpRequest*     createHttpRequest();
//         // static HttpResponse*    createHttpResponse();

//         ws_http::statuscodes_t  parseMessage(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         std::string const &     getHeader( std::string const & key ) const;
//         bool                    hasHeader( std::string const & key ) const;
//         buff_t const &          getBody( void ) const;
//         headers_t const &       getHeaderVector( void ) const;
//         ws_http::statuscodes_t  getStatus( void ) const;
//         void                    printMessage( void ) const;



//         ws_http::statuscodes_t  setStatus( ws_http::statuscodes_t statusCode );
//         ws_http::statuscodes_t  parseBody(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//         ws_http::statuscodes_t  parseBodyChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end);
//     protected:

//         bool    parseReqline(unsigned char c);
//         // ws_http::statuscodes_t  parseHeaders(buff_t::const_iterator it_start, buff_t::const_iterator it_end);

//         // void    insertHeader(std::string const & key, std::string const & buffer);
//         void    insertHeader(std::string const & key, std::string & buffer);
//         void    insertHeader(std::string & key, std::string & buffer);

//         ws_http::statuscodes_t    parseHeaderLine(unsigned char c);
//         ws_http::statuscodes_t    parseMethod(unsigned char c);
//         ws_http::statuscodes_t    parsePath(unsigned char c);
//         ws_http::statuscodes_t    parseQuery(unsigned char c);
//         ws_http::statuscodes_t    parseVersion(unsigned char c);

//         // bool        _headerDone;
//         // bool        _LineDone;
//         // bool        _crFound;
//         // bool        _reqLineDone;
//         // bool        _bodyDone;
//         // bool        _parseSize;
//         // bool parseKey;
//         // bool valueStartFound;
//         // bool crFound;

//         //              1               0           0           0           0           1               1           0                           1                       0               0                       0                   0
//         // enum EnumBits { PARSE_REQLINE, LINE_DONE, HEADER_DONE, BODY_DONE, CR_FOUND, HEADER_PARSE_KEY, PARSE_SIZE, HEADER_VALUE_START_FOUND, REQLINE_PARSE_METHOD, REQLINE_PARSE_PATH, REQLINE_PARSE_QUERY, REQLINE_PARSE_VERSION, REQLINE_HAS_QUERY, BITS_MAX };
//         enum EnumBits { HEADER_DONE, BODY_DONE, CR_FOUND, HEADER_PARSE_KEY, PARSE_SIZE, HEADER_VALUE_START_FOUND, BITS_MAX };
//         enum ReqLinePos { REQLINE_PARSE_METHOD, REQLINE_PARSE_PATH, REQLINE_PARSE_QUERY, REQLINE_PARSE_VERSION, REQLINE_DONE };
//         enum EnumNbrsHeader { NBR_HEADER_WHITEPOS_LEN };
//         enum EnumNbrsBody { NBR_BODY_CONTENT_LENGTH, NBR_BODY_CHUNK_SIZE };
        
//         long    _nbrs[2];
        
//         std::bitset<BITS_MAX>  _bits;
//         char    _reqLinePos;

//         ws_http::statuscodes_t  status;
//         headers_t           _headerMap;
//         std::string         _buffer1;
//         std::string         _buffer2;
//         // std::string         _firstLine;
//         // buff_t              _bodyBuffer;

//         OnHttpMessageBodySize*  _maxBodySizeGetter;

//         long        _contentLength;
//         long        _maxBodySize;
//         long        _nbrHelper;
//         // bool        _headerDone;
//         // bool        _reqLineDone;
//         // bool        _crFound;
//         // bool        _parseSize;
//         // bool        _reqLineDone;
//         // bool        _bodyDone;
// };

// template<typename T>
// std::string printBool(T t)
// {
//     // std::cout << "bool: " << t << std::endl;
//     if (t == 1)
//         return (std::string("true"));
//     return (std::string("false"));
// }

// #endif
