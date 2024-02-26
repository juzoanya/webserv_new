/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestTestClass.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:31:25 by mberline          #+#    #+#             */
/*   Updated: 2024/02/14 12:03:34 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestTestClass.hpp"


std::string HttpRequestTest::getNumberStringHex(long number)
{
    std::stringstream   ss;
    ss << std::hex << number;
    return (ss.str());
}

std::string HttpRequestTest::makeChunked(std::string str, int chunkSize)
{
    buff_t::const_iterator it_start = str.begin();
    buff_t::const_iterator it_end = str.end();
    long size = it_end - it_start;
    int chunkCount = size / chunkSize;
    std::string chunkSizeHexStr = getNumberStringHex(chunkSize);
    int chunkBuffSize = (chunkCount + 2) * (chunkSizeHexStr.size() + ws_http::crlf.size() + chunkSize + ws_http::crlf.size());
    std::string  chunk;
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

std::vector<std::string> HttpRequestTest::splitStringRandomly(std::string input, std::size_t minPartStringSize, std::size_t maxPartStringSize) {
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
    return (parts);
}

std::string HttpRequestTest::requestHeader = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\t\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Connection: keep-alive\r\n";

std::string HttpRequestTest::requestBody = 
            "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
            "<title>"
            "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005."
            "</title>";

std::string HttpRequestTest::request = HttpRequestTest::requestHeader + HttpRequestTest::requestBody;

HttpRequestTest::HttpRequestTest( void )
{
    this->_requestSplitted = splitStringRandomly(request, 50, 100);
    this->_headerSplitted = splitStringRandomly(requestHeader, 50, 100);
    this->_bodySplitted = splitStringRandomly(requestBody, 50, 100);
}

HttpRequestTest::~HttpRequestTest( void )
{ }


int HttpRequestTest::testHeaderParsingChunked( int minPartSize, int maxPartSize, int chunkSize )
{
    std::string rechunked = makeChunked(requestBody, chunkSize);
    std::string re = requestHeader + "Transfer-Encoding: chunked" + ws_http::crlf + ws_http::crlf + rechunked;
    return (testHeaderParsingInt(re, minPartSize, maxPartSize));
}

int HttpRequestTest::testHeaderParsing( int minPartSize, int maxPartSize )
{
    std::string re = requestHeader + "Content-length: " + toStr(requestBody.size()) + ws_http::crlf + ws_http::crlf + requestBody;
    return (testHeaderParsingInt(re, minPartSize, maxPartSize));
}

int HttpRequestTest::testHeaderParsingInt( std::string re, int minPartSize, int maxPartSize )
{
    HttpMessage req;
    std::vector<std::string> parts = splitStringRandomly(re, minPartSize, maxPartSize);

    std::vector<std::string>::iterator it;
    int i = 0;
    std::string res;
    for (it = parts.begin(); it != parts.end(); ++it, ++i) {
        std::string& part = *it;
        std::cout << "\n------- Part: " << i << " -------" << std::endl;
        std::cout << part << std::endl;
        std::cout << "------------------------------------" << std::endl;
        ws_http::statuscodes_t status = req.parseMessage(part.begin(), part.end());
        std::cout << "------------------------------------" << std::endl;
        if (status >= ws_http::STATUS_400_BAD_REQUEST) {
            std::cout << "ERROR - " << ws_http::statuscodes.at(status) << std::endl;
            break ;
        } else if (status == ws_http::STATUS_200_OK) {
            res = std::string(req.getBody().begin(), req.getBody().end());
            break ;
        }
    }
    req.printMessage();
    std::cout << res << std::endl;
    if (res !=  requestBody)
        std::cout << "UNEQUAL!" << std::endl;
    else
        std::cout << "EQUAL!" << std::endl;
    return (0);
}

// int HttpRequestTest::testBodyParsing( int chunkSize, int minPartSize, int maxPartSize )
// {
//     // HttpRequest req(NULL, false, NULL);
//     HttpMessage req;

//     std::string chunkedString = makeChunked(requestBody, chunkSize);
//     // std::cout << "chunked: " << chunkedString << std::endl;
//     std::vector<std::string> parts = splitStringRandomly(chunkedString, minPartSize, maxPartSize);

//     std::vector<std::string>::iterator it;
//     int i = 0;
//     for (it = parts.begin(); it != parts.end(); ++it, ++i) {
//         std::string& part = *it;
//         // std::cout << "\n------- Part: " << i << " -------" << std::endl;
//         // std::cout << part << std::endl;
//         // std::cout << "------------------------------------" << std::endl;
//         ws_http::statuscodes_t status = req.parseBody(part.begin(), part.end());
//         // std::cout << "------------------------------------" << std::endl;
//         if (status >= ws_http::STATUS_400_BAD_REQUEST) {
//             std::cout << "ERROR - " << ws_http::statuscodes.at(status) << std::endl;
//             break ;
//         } else if (status == ws_http::STATUS_200_OK) {
//             std::string res(req.getBody().begin(), req.getBody().end());
//             // std::cout << res << std::endl;
//             if (res != requestBody)
//                 std::cout << "UNEQUAL!" << std::endl;
//             else
//                 std::cout << "EQUAL!" << std::endl;
//             break ;
//         }
//     }


//     return (0);
// }
