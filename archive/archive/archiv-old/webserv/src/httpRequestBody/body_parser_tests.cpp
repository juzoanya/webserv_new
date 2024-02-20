/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   body_parser_tests.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 10:53:21 by mberline          #+#    #+#             */
/*   Updated: 2024/01/19 20:43:15 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestBody.hpp"

std::string getNumberStringHex(long number)
{
    std::stringstream   ss;
    ss << std::hex << number;
    return (ss.str());
}

buff_t makeChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end, int chunkSize)
{
    long size = it_end - it_start;
    int chunkCount = size / chunkSize;
    std::string chunkSizeHexStr = getNumberStringHex(chunkSize);
    int chunkBuffSize = (chunkCount + 2) * (chunkSizeHexStr.size() + ws_http::crlf.size() + chunkSize + ws_http::crlf.size());
    buff_t  chunk;
    chunk.reserve(chunkBuffSize);
    
    
    
    while (it_start != it_end) {
        if (it_end - it_start < chunkSize) {
            chunkSize = it_end - it_start;
            chunkSizeHexStr = getNumberStringHex(chunkSize);
            chunkBuffSize = chunkSizeHexStr.size() + ws_http::crlf.size() + chunkSize + ws_http::crlf.size();
        }
        std::copy(chunkSizeHexStr.begin(), chunkSizeHexStr.end(), std::back_inserter(chunk));
        std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
        std::copy(it_start, it_start + chunkSize, std::back_inserter(chunk));
        std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
        it_start += chunkSize;
    }
    std::string endHex = getNumberStringHex(0);
    std::copy(endHex.begin(), endHex.end(), std::back_inserter(chunk));
    std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
    std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
    return (chunk);
}

std::vector<std::string> SplitStringRandomly(std::string& input, std::size_t minPartStringSize, std::size_t maxPartStringSize) {
    std::vector<std::string> parts;
    std::random_device rd;
    std::default_random_engine generator(rd());

    while (!input.empty()) {
        std::uniform_int_distribution<int> distribution(minPartStringSize, maxPartStringSize);
        std::size_t randomPartLength = distribution(generator);

        if (randomPartLength <= input.length()) {
            parts.push_back(input.substr(0, randomPartLength));
            input.erase(0, randomPartLength);
        } else {
            parts.push_back(input.substr(0, input.length()));
            input.clear();
        }
    }
    return parts;
}

void chunkSplitTest(std::string original, std::string text, std::size_t minPartStringSize, std::size_t maxPartStringSize)
{
    HttpRequestBody chunkParser(-1, 4096);
    std::vector<std::string> parts = SplitStringRandomly(text, minPartStringSize, maxPartStringSize);
    std::vector<std::string>::iterator it;
    for (it = parts.begin(); it != parts.end(); ++it) {
        std::string& part = *it;
        // std::cout << "\n-------> Part: " << part << std::endl;
        // std::cout << "------------------------------------" << std::endl;
        ws_http::statuscodes_t status = chunkParser.parseBody(part.begin(), part.end());
        // std::cout << "------------------------------------" << std::endl;
        if (status >= ws_http::STATUS_400_BAD_REQUEST) {
            std::cout << "ERROR: " << status << std::endl;
            break ;
        } else if (status == ws_http::STATUS_200_OK) {
            std::string res(chunkParser.getBody().begin(), chunkParser.getBody().end());
            std::cout << res << std::endl;
            if (res != original)
                std::cout << "UNEQUAL!" << std::endl;
            break ;
        } else {
            std::string res(chunkParser.getBody().begin(), chunkParser.getBody().end());
            std::cout << "res buffer: " << res << std::endl;
        }
    }
}

void    bodyParserTest(std::string const & text, std::size_t contentLength, std::size_t maxBodySize, std::size_t chunkSize, std::size_t minPartStringSize, std::size_t maxPartStringSize)
{
    HttpRequestBody chunkParser(contentLength, maxBodySize);
    std::string cmpStr;
    if (contentLength == -1) {
        buff_t chunked = makeChunked(text.begin(), text.end(), 82);
        cmpStr = std::string(chunked.begin(), chunked.end());
    } else {
        cmpStr = text;
    }
    std::vector<std::string> parts = SplitStringRandomly(cmpStr, minPartStringSize, maxPartStringSize);
    std::vector<std::string>::iterator it;
    for (it = parts.begin(); it != parts.end(); ++it) {
        std::string& part = *it;
        // std::cout << "-------------- Part: ---------------" << std::endl;
        // std::cout << part << std::endl;
        // std::cout << "------------------------------------" << std::endl;
        ws_http::statuscodes_t status = chunkParser.parseBody(part.begin(), part.end());
        // std::cout << "------------------------------------" << std::endl;
        if (status >= ws_http::STATUS_400_BAD_REQUEST) {
            std::cout << "ERROR: " << status << std::endl;
            break ;
        } else if (status == ws_http::STATUS_200_OK) {
            std::string res(chunkParser.getBody().begin(), chunkParser.getBody().end());
            std::cout << res << std::endl;
            if (res != text)
                std::cout << "UNEQUAL!" << std::endl;
            else
                std::cout << "EQUAAAL :)" << std::endl;
            break ;
        }
    }
}

int main( void )
{
    std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005.";
    // std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein.";

    // bodyParserTest(text, text.size(), 4096, 89, 10, 100);
    bodyParserTest(text, -1, 4096, 89, 10, 100);
}















































// typedef std::vector<char> buff_t;

// namespace ws_http {

//     typedef enum {
//         STATUS_UNDEFINED,
//         STATUS_200_OK = 200,
//         STATUS_301_MOVED_PERMANENTLY = 301,
//         STATUS_302_FOUND = 302,
//         STATUS_400_BAD_REQUEST = 400,
//         STATUS_403_FORBIDDEN = 403,
//         STATUS_404_NOT_FOUND = 404,
//         STATUS_405_METHOD_NOT_ALLOWED = 405,
//         STATUS_408_REQUEST_TIMEOUT = 408,
//         STATUS_411_LENGTH_REQUIRED = 411,
//         STATUS_413_PAYLOAD_TOO_LARGE = 413,
//         STATUS_414_URI_TOO_LONG = 414,
//         STATUS_415_UNSUPPORTED_MEDIA_TYPE = 415,
//         STATUS_429_TOO_MANY_REQUESTS = 429,
//         STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
//         STATUS_500_INTERNAL_SERVER_ERROR = 500,
//         STATUS_501_NOT_IMPLEMENTED = 501,
//         STATUS_503_SERVICE_UNAVAILABLE = 503,
//         STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505
//     } statuscodes_t;

//     std::string crlf = "\r\n";
// };

// struct HttpRequest {
//     HttpRequest(long contSize, long maxBody) : _contentLength(contSize), maxBodySize(maxBody) { }
//     ~HttpRequest(void) { }
//     ws_http::statuscodes_t    parseBody(buff_t::iterator it_start, buff_t::iterator it_end);
//     ws_http::statuscodes_t    parseChunk(buff_t::iterator it_start, buff_t::iterator it_end);
//     ws_http::statuscodes_t    parseChunkEmpty(buff_t::iterator it_start, buff_t::iterator it_end);
//     ws_http::statuscodes_t    parseChunkNotEmpty(buff_t::iterator it_start, buff_t::iterator it_end);
//     long    _contentLength;
//     long    maxBodySize;
//     long    chunkSize;
//     buff_t  _buffer;
//     buff_t  bodyBuffer;
// };

// // int    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// // {
// //     if (!this->_buffer.empty()) {
// //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //         it_start = this->_buffer.begin();
// //         it_end = this->_buffer.end();
// //     }
// //     while (true) {
// //         buff_t::iterator  itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
// //         if (itSize != it_end) {
// //             long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
// //             buff_t::iterator contStart = itSize + ws_http::crlf.size();
// //             if (chunkSize == 0 && std::distance(contStart, it_end) == 2)
// //                 return (1);
// //             if (std::distance(contStart, it_end) >= chunkSize + ws_http::crlf.size() + 2) {
// //                 std::copy(contStart, contStart + chunkSize, std::back_inserter(bodyBuffer));
// //                 it_start = contStart + chunkSize + ws_http::crlf.size();
// //                 continue ;
// //             }
// //         }
// //         break ;
// //     }
// //     if (!this->_buffer.empty()) {
// //         this->_buffer.erase(this->_buffer.begin(), it_start);
// //     } else {
// //         std::copy(it_start, it_end, std::back_inserter(this->_buffer));
// //     }
// //     return (0);
// // }

// std::string getNumberStringHex(long number)
// {
//     std::stringstream   ss;
//     ss << std::hex << number;
//     return (ss.str());
// }

// buff_t makeChunked(buff_t::const_iterator it_start, buff_t::const_iterator it_end, int chunkSize)
// {
//     long size = it_end - it_start;
//     int chunkCount = size / chunkSize;
//     std::string chunkSizeHexStr = getNumberStringHex(chunkSize);
//     int chunkBuffSize = (chunkCount + 2) * (chunkSizeHexStr.size() + ws_http::crlf.size() + chunkSize + ws_http::crlf.size());
//     buff_t  chunk;
//     chunk.reserve(chunkBuffSize);
    
    
    
//     while (it_start != it_end) {
//         if (it_end - it_start < chunkSize) {
//             chunkSize = it_end - it_start;
//             chunkSizeHexStr = getNumberStringHex(chunkSize);
//             chunkBuffSize = chunkSizeHexStr.size() + ws_http::crlf.size() + chunkSize + ws_http::crlf.size();
//         }
//         std::copy(chunkSizeHexStr.begin(), chunkSizeHexStr.end(), std::back_inserter(chunk));
//         std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
//         std::copy(it_start, it_start + chunkSize, std::back_inserter(chunk));
//         std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
//         it_start += chunkSize;
//     }
//     std::string endHex = getNumberStringHex(0);
//     std::copy(endHex.begin(), endHex.end(), std::back_inserter(chunk));
//     std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
//     std::copy(ws_http::crlf.begin(), ws_http::crlf.end(), std::back_inserter(chunk));
//     return (chunk);
// }

// std::vector<std::string> SplitStringRandomly(std::string& input, std::size_t minPartStringSize, std::size_t maxPartStringSize) {
//     std::vector<std::string> parts;
//     std::random_device rd;
//     std::default_random_engine generator(rd());

//     while (!input.empty()) {
//         std::uniform_int_distribution<int> distribution(minPartStringSize, maxPartStringSize);
//         std::size_t randomPartLength = distribution(generator);

//         if (randomPartLength <= input.length()) {
//             parts.push_back(input.substr(0, randomPartLength));
//             input.erase(0, randomPartLength);
//         } else {
//             parts.push_back(input.substr(0, input.length()));
//             input.clear();
//         }
//     }
//     return parts;
// }

// void chunkSplitTest(std::string original, std::string text, std::size_t minPartStringSize, std::size_t maxPartStringSize)
// {
//     HttpRequest chunkParser(-1, 4096);
//     std::vector<std::string> parts = SplitStringRandomly(text, minPartStringSize, maxPartStringSize);
//     std::vector<std::string>::iterator it;
//     for (it = parts.begin(); it != parts.end(); ++it) {
//         std::string& part = *it;
//         // std::cout << "\n-------> Part: " << part << std::endl;
//         // std::cout << "------------------------------------" << std::endl;
//         ws_http::statuscodes_t status = chunkParser.parseBody(part.begin(), part.end());
//         // std::cout << "------------------------------------" << std::endl;
//         if (status >= ws_http::STATUS_400_BAD_REQUEST) {
//             std::cout << "ERROR: " << status << std::endl;
//             break ;
//         } else if (status == ws_http::STATUS_200_OK) {
//             std::string res(chunkParser.bodyBuffer.begin(), chunkParser.bodyBuffer.end());
//             std::cout << res << std::endl;
//             if (res != original)
//                 std::cout << "UNEQUAL!" << std::endl;
//             break ;
//         } else {
//             std::string res(chunkParser.bodyBuffer.begin(), chunkParser.bodyBuffer.end());
//             std::cout << "res buffer: " << res << std::endl;
//         }
//     }
// }

// void    bodyParserTest(std::string const & text, std::size_t contentLength, std::size_t maxBodySize, std::size_t chunkSize, std::size_t minPartStringSize, std::size_t maxPartStringSize)
// {
//     HttpRequest chunkParser(contentLength, maxBodySize);
//     std::string cmpStr;
//     if (contentLength == -1) {
//         buff_t chunked = makeChunked(text.begin(), text.end(), 82);
//         cmpStr = std::string(chunked.begin(), chunked.end());
//     } else {
//         cmpStr = text;
//     }
//     std::vector<std::string> parts = SplitStringRandomly(cmpStr, minPartStringSize, maxPartStringSize);
//     std::vector<std::string>::iterator it;
//     for (it = parts.begin(); it != parts.end(); ++it) {
//         std::string& part = *it;
//         // std::cout << "-------------- Part: ---------------" << std::endl;
//         // std::cout << part << std::endl;
//         // std::cout << "------------------------------------" << std::endl;
//         ws_http::statuscodes_t status = chunkParser.parseBody(part.begin(), part.end());
//         // std::cout << "------------------------------------" << std::endl;
//         if (status >= ws_http::STATUS_400_BAD_REQUEST) {
//             std::cout << "ERROR: " << status << std::endl;
//             break ;
//         } else if (status == ws_http::STATUS_200_OK) {
//             std::string res(chunkParser.bodyBuffer.begin(), chunkParser.bodyBuffer.end());
//             std::cout << res << std::endl;
//             if (res != text)
//                 std::cout << "UNEQUAL!" << std::endl;
//             break ;
//         }
//     }
// }


// void    printBuffer(std::string const & msg, buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::cout << " -------------------- " << msg << ":  -------------------- " <<std::endl;
//     std::cout << std::string(it_start, it_end) << std::endl;
//     std::cout << " ----------------------------------------------------------------- " <<std::endl;
// }

// ws_http::statuscodes_t  HttpRequest::parseChunkNotEmpty(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::cout << "buffer not empty\n";
//     printBuffer("current buffer content", this->_buffer.begin(), this->_buffer.end());
//     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     printBuffer("new buffer content", this->_buffer.begin(), this->_buffer.end());
    

//     it_start = this->_buffer.begin();
//     buff_t::iterator  itSize = it_start;
//     while (true) {
//         std::cout << "\nchunked loop\n";
//         itSize = std::search(it_start, this->_buffer.end(), ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itSize == it_end) {
//             std::cout << "crlf after chunksize was not found" << std::endl;
//             this->_buffer.erase(this->_buffer.begin(), it_start);
//             printBuffer("buffer after erase", this->_buffer.begin(), this->_buffer.end());
//             return (ws_http::STATUS_UNDEFINED);
//         }
//         long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//         std::cout << "chunksize found: " << chunkSize << std::endl;
//         std::size_t distance = std::distance(itSize + 2, it_end);
//         std::cout << "rest distance: itSize after crlf <-> this->_buffer.end(): " << distance << std::endl;
//         if (distance < chunkSize + 2)
//             break ;
//         if (chunkSize == 0 && distance == 2) {
//             std::cout << "chunksize 0 and ending" << std::endl;
//             return (ws_http::STATUS_200_OK);
//         }
//         if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(itSize + 2, itSize + 2 + chunkSize, std::back_inserter(bodyBuffer));
//         printBuffer("new bodyBuffer content", this->bodyBuffer.begin(), this->bodyBuffer.end());
//         it_start = itSize + 2 + chunkSize + 2;
//         this->maxBodySize -= chunkSize;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// ws_http::statuscodes_t  HttpRequest::parseChunkEmpty(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::cout << "buffer empty\n";
//     buff_t::iterator  itSize = it_start;
//     while (true) {
//         std::cout << "\nchunked loop\n";
//         itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itSize == it_end) {
//             std::cout << "crlf after chunksize was not found" << std::endl;
//             std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//             printBuffer("buffer after copy into", this->_buffer.begin(), this->_buffer.end());
//             return (ws_http::STATUS_UNDEFINED);
//         }
//         long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//         std::size_t distance = std::distance(itSize + 2, it_end);
//         if (distance < chunkSize + 2) {
//             std::cout << "distance < chunkSize + 2\n";
//             std::copy(itSize + 2, it_end, std::back_inserter(this->bodyBuffer));
//             printBuffer("new bodyBuffer content", this->bodyBuffer.begin(), this->bodyBuffer.end());
//             this->maxBodySize -= distance;
//             return (ws_http::STATUS_UNDEFINED);
//         }
//         if (chunkSize == 0 && distance == 2) {
//             std::cout << "chunksize 0 and ending" << std::endl;
//             return (ws_http::STATUS_200_OK);
//         }
//         if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(itSize + 2, itSize + 2 + chunkSize, std::back_inserter(bodyBuffer));
//         printBuffer("new bodyBuffer content", this->bodyBuffer.begin(), this->bodyBuffer.end());
//         it_start = itSize + 2 + chunkSize + 2;
//         this->maxBodySize -= chunkSize;
//     }
//     return (ws_http::STATUS_UNDEFINED);
// }

// // ws_http::statuscodes_t  HttpRequest::parseChunk(buff_t::iterator it_start, buff_t::iterator it_end)
// // {
// //     std::cout << " -- parse chunked --\n";
// //     printBuffer("new content", it_start, it_end);
// //     if (this->_buffer.empty())
// //         return (this->parseChunkEmpty(it_start, it_end));
// //     return (this->parseChunkNotEmpty(it_start, it_end));

// // }

// struct  HttpBodyChunk {
//     HttpBodyChunk( buff_t& body ) :chunkSize(0), bodyBuffer(body) {
        
//     }
//     ~HttpBodyChunk( void ) {

//     }

//     ws_http::statuscodes_t  parseChunk(buff_t::iterator it_start, buff_t::iterator it_end) {
//         buff_t::iterator contentStart = it_start;
//         std::size_t dist = std::min(std::distance(contentStart, it_end), chunkSize);
//         std::copy(contentStart, )
//     }    
//     long   chunkSize;
//     buff_t& bodyBuffer;
//     buff_t  tmpBuffer;
// };

// ws_http::statuscodes_t  HttpRequest::parseChunk(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::cout << " -- parse chunked --\n";
//     printBuffer("new content", it_start, it_end);
//     buff_t::iterator  itSize = it_start;
//     while (true) {
//         std::cout << "\nchunked loop\n";
//         itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//         if (itSize == it_end) {
//             std::cout << "crlf after chunksize was not found" << std::endl;
//             std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//             printBuffer("buffer after copy into", this->_buffer.begin(), this->_buffer.end());
//             return (ws_http::STATUS_UNDEFINED);
//         }
//         long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//         std::size_t distance = std::distance(itSize + 2, it_end);
//         if (distance < chunkSize + 2) {
//             std::cout << "distance < chunkSize + 2\n";
//             std::copy(itSize + 2, it_end, std::back_inserter(this->bodyBuffer));
//             printBuffer("new bodyBuffer content", this->bodyBuffer.begin(), this->bodyBuffer.end());
//             this->maxBodySize -= distance;
//             return (ws_http::STATUS_UNDEFINED);
//         }
//         if (chunkSize == 0 && distance == 2) {
//             std::cout << "chunksize 0 and ending" << std::endl;
//             return (ws_http::STATUS_200_OK);
//         }
//         if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
//             return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//         std::copy(itSize + 2, itSize + 2 + chunkSize, std::back_inserter(bodyBuffer));
//         printBuffer("new bodyBuffer content", this->bodyBuffer.begin(), this->bodyBuffer.end());
//         it_start = itSize + 2 + chunkSize + 2;
//         this->maxBodySize -= chunkSize;
//     }
//     return (ws_http::STATUS_UNDEFINED);

// }

// ws_http::statuscodes_t    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::cout << "\n\n---- PARSE BODY ----\n";
//     if (this->_contentLength >= 0) {
//         std::cout << " -- parse normally --\n";
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
//     return (this->parseChunk(it_start, it_end));
//     // std::cout << " -- parse chunked --\n";
//     // if (!this->_buffer.empty()) {
//     //     std::cout << "buffer not empty\n";
//     //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//     //     std::cout << "current buffer: " << std::string(this->_buffer.begin(), this->_buffer.end()) << std::endl;
//     //     it_start = this->_buffer.begin();
//     //     it_end = this->_buffer.end();
//     // }
//     // buff_t::iterator  itSize = it_start;
//     // while (true) {
//     //     std::cout << "\nchunked loop\n";
//     //     itSize = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     //     if (itSize == it_end) {
//     //         std::cout << "crlf after chunksize was not found" << std::endl;
//     //         itSize = it_start;
//     //         break ;
//     //     }
//     //     long chunkSize = strtol(std::string(it_start, itSize).c_str(), NULL, 16);
//     //     std::size_t distance = std::distance(itSize + 2, it_end);
//     //     if (distance < chunkSize + 2)
//     //         break ;
//     //     if (chunkSize == 0 && distance == 2) {
//     //         std::cout << "chunksize 0 and ending" << std::endl;
//     //         return (ws_http::STATUS_200_OK);
//     //     }
//     //     if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
//     //         return (ws_http::STATUS_413_PAYLOAD_TOO_LARGE);
//     //     std::copy(itSize + 2, itSize + 2 + chunkSize, std::back_inserter(bodyBuffer));
//     //     it_start = itSize + 2 + chunkSize + 2;
//     //     this->maxBodySize -= chunkSize;
//     // }
//     // if (!this->_buffer.empty()) {
//     //     std::cout << "buffer not empty - erase\n";
//     //     this->_buffer.erase(this->_buffer.begin(), itSize);
//     // } else {
//     //     std::cout << "buffer empty - copy to buffer\n";
//     //     std::cout << "rest distance: " << std::distance(itSize, it_end) << std::endl;
//     //     std::cout << "current rest: " << std::string(itSize, it_end) << std::endl;
//     //     std::copy(itSize, it_end, std::back_inserter(this->_buffer));
//     //     std::cout << "current buffer: " << std::string(this->_buffer.begin(), this->_buffer.end()) << std::endl;
//     // }
//     // return (ws_http::STATUS_UNDEFINED);
// }

// int main( void )
// {
//     // std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005.";
//     std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein.";

//     // bodyParserTest(text, text.size(), 4096, 89, 10, 100);
//     bodyParserTest(text, -1, 4096, 89, 10, 100);
// }











































// int main( void )
// {
//     std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005.";

//     buff_t chunked = makeChunked(text.begin(), text.end(), 82);
//     std::string chunkedStr = std::string(chunked.begin(), chunked.end());
//     // std::cout << chunkedStr << std::endl;

//     for (int i = 0; i < 10000; i++) {
//         chunkSplitTest(text, chunkedStr, 10, 100);
//         // std::cout << "result decoded: \n" << decoded << std::endl;

//     }
// }


// int main( void )
// {
//     HttpRequest request(-1, 4096);
// }

// long    getChunkSize(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     buff_t::iterator sizeIt = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     long chunkSize = strtol(std::string(it_start, sizeIt).c_str(), NULL, 10);
//     return (chunkSize);
// }

// void    fillBodyBuffer(buff_t::iterator it_start, buff_t::iterator it_end, long chunkSize, buff_t& buffer)
// {
//     std::copy(it_start, it_end, std::back_inserter(buffer));
// }

// buff_t::iterator parseChunk(buff_t::iterator it_start, buff_t::iterator it_end, buff_t& bodyBuffer, buff_t& tempBuffer)
// {
//     buff_t::iterator sizeIt = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//     if (sizeIt != it_end) {
//         long chunkSize = strtol(std::string(it_start, sizeIt).c_str(), NULL, 10);
//         std::copy(sizeIt + ws_http::crlf.size(), sizeIt + ws_http::crlf.size() + chunkSize, std::back_inserter(bodyBuffer));
//         return (sizeIt + ws_http::crlf.size() + chunkSize);
//     }
//     return (it_start);
// }

// void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// {
//     std::size_t dist = std::distance(it_start, it_end);
//     if (dist < 0)
//         throw std::runtime_error("invalid iterator distance");
//     if (this->_contentLength == -1) {
//         while (std::distance(it_start, it_end) > 0) {
            

//             buff_t::iterator sizeIt = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//             if (sizeIt == it_end) {
                
//             }




//             // buff_t::iterator chunkSizePos;
//             // if (!this->_buffer.empty()) {
//             //     chunkSizePos = parseChunk(this->_buffer.begin(), this->_buffer.end(), this->bodyBuffer);
//             // } else {
//             //     chunkSizePos = parseChunk(it_start, it_end, this->bodyBuffer);
//             // }
//             // if (chunkSizePos == it_start)


            
//             // buff_t::iterator sizeIt = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
//             // if (sizeIt == it_end) {
//             //     std::copy(it_start, it_end, std::back_inserter(this->_buffer));
//             //     return ;
//             // }
//             // this->chunkSize = strtol(std::string(it_start, sizeIt).c_str(), NULL, 10);
//             // if (this->chunkSize < std::distance(it_start, it_end)) {
//             //     return ;
//             // }
//             // if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
//             //     return ;
//             // std::copy(sizeIt + ws_http::crlf.size(), sizeIt + ws_http::crlf.size() + chunkSize, std::back_inserter(this->bodyBuffer));
//             // it_start = sizeIt + ws_http::crlf.size() + chunkSize;
//         }
//     } else {
//         if (this->bodyBuffer.size() + dist > this->_contentLength || this->bodyBuffer.size() + dist > this->maxBodySize)
//             return ;
//         std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
//         if (this->bodyBuffer.size() == this->_contentLength)
//             return ;
//     }
// }

// // void    HttpRequest::parseBody(buff_t::iterator it_start, buff_t::iterator it_end)
// // {
// //     std::size_t dist = std::distance(it_start, it_end);
// //     if (dist < 0)
// //         throw std::runtime_error("invalid iterator distance");
// //     if (this->_contentLength == -1) {
// //         while (std::distance(it_start, it_end) > 0) {
// //             buff_t::iterator sizeIt = std::search(it_start, it_end, ws_http::crlf.begin(), ws_http::crlf.end());
// //             long chunkSize = strtol(std::string(it_start, sizeIt).c_str(), NULL, 10);
// //             if (this->bodyBuffer.size() + chunkSize > this->maxBodySize)
// //                 return ;
// //             std::copy(sizeIt + ws_http::crlf.size(), sizeIt + ws_http::crlf.size() + chunkSize, std::back_inserter(this->bodyBuffer));
// //             it_start = sizeIt + ws_http::crlf.size() + chunkSize;
// //         }
// //     } else {
// //         if (this->bodyBuffer.size() + dist > this->_contentLength || this->bodyBuffer.size() + dist > this->maxBodySize)
// //             return ;
// //         std::copy(it_start, it_end, std::back_inserter(this->bodyBuffer));
// //         if (this->bodyBuffer.size() == this->_contentLength)
// //             return ;
// //     }
// // }
