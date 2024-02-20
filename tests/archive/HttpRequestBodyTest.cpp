
#include <vector>
#include <sstream>
#include <iostream>
#include <random>

#include "../src/headers.hpp"

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
    HttpBody chunkParser;
    std::vector<std::string> parts = SplitStringRandomly(text, minPartStringSize, maxPartStringSize);
    std::vector<std::string>::iterator it;
    for (it = parts.begin(); it != parts.end(); ++it) {
        std::string& part = *it;
        // std::cout << "\n-------> Part: " << part << std::endl;
        // std::cout << "------------------------------------" << std::endl;
        ws_http_parser_status_t status = chunkParser.parseBody(part.begin(), part.end());
        // std::cout << "------------------------------------" << std::endl;
        if (status >= ws_http_REQ_PARSE_ERROR_DEFAULT) {
            std::cout << "ERROR\n";
            break ;
        } else if (status == ws_http_REQ_PARSE_BODY_DONE) {
            std::string res(chunkParser.getBody().begin(), chunkParser.getBody().end());
            // std::cout << res << std::endl;
            if (res != original)
                std::cout << "UNEQUAL!" << std::endl;
            break ;
        }
    }
}


void    bodyParserTest(std::string const & text, std::size_t contentLength, std::size_t maxBodySize, std::size_t chunkSize, std::size_t minPartStringSize, std::size_t maxPartStringSize)
{
    HttpRequest chunkParser(contentLength, maxBodySize);
    std::string cmpStr;
    if (contentLength == 0) {
        buff_t chunked = makeChunked(text.begin(), text.end(), 82);
        cmpStr = std::string(chunked.begin(), chunked.end());
    } else {
        cmpStr = text;
    }
    std::vector<std::string> parts = SplitStringRandomly(cmpStr, minPartStringSize, maxPartStringSize);
    std::vector<std::string>::iterator it;
    for (it = parts.begin(); it != parts.end(); ++it) {
        std::string& part = *it;
        ws_http_parser_status_t status = chunkParser.parseBody(part.begin(), part.end());
        if (status >= ws_http_REQ_PARSE_ERROR_DEFAULT) {
            std::cout << "ERROR: " << status << std::endl;
            break ;
        } else if (status == ws_http_REQ_PARSE_BODY_DONE) {
            std::string res(chunkParser.getBody().begin(), chunkParser.getBody().end());
            std::cout << res << std::endl;
            if (res != text)
                std::cout << "UNEQUAL!" << std::endl;
            break ;
        }
    }
}

int main( void )
{
    std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005.";

    buff_t chunked = makeChunked(text.begin(), text.end(), 82);
    std::string chunkedStr = std::string(chunked.begin(), chunked.end());
    // std::cout << chunkedStr << std::endl;

    for (int i = 0; i < 10000; i++) {
        chunkSplitTest(text, chunkedStr, 10, 100);
        // std::cout << "result decoded: \n" << decoded << std::endl;

    }
}

int main( void )
{
    std::string text = "Iakovos wurde auf der Insel Imbros im Osmanischen Reich geboren, die damals griechisch besiedelt war. Im Alter von 15 schrieb er sich an der theologischen Schule in Chalki ein und nahm nach Abschluss 1934 „Iakovos“ als kirchlichen Namen an.\n\nFünf Jahre nach seiner Ordination erhielt Diakon Iakovos eine Einladung, als Erzdiakon bei Erzbischof Athenagoras zu arbeiten, der später Ökumenischer Patriarch von Konstantinopel wurde. 1940 wurde er in Lowell, Massachusetts, zum Priester geweiht. Gleichzeitig absolvierte er ein Studium an der Harvard University und erwarb einen Master of Sacred Theology-Abschluss.[1]\n\nNach Wahl der Heiligen Synode von Konstantinopel am 14. Februar 1959 trat er zum 1. April 1959 als Nachfolger des verstorbenen Erzbischof Michael die Stelle des Erzbischofs von Nord- und Südamerika ein, die Metropolie von Amerika zählte zu diesem Zeitpunkt bereits über 500 Gemeinden. Eines seiner ersten Treffen war der Besuch von Papst Johannes XXIII., welches das erste Treffen eines Papstes mit einer orthodoxen Persönlichkeit seit 350 Jahren war. In den USA versuchte er die ethnische Ausrichtung vieler Gemeinden zu minimieren und die Ökumene zu stärken.[2] Er initiierte die Standing Conference of the Canonical Orthodox Bishops in the Americas, deren Vorsitz er übernahm.\n\nIakovos wurde immer wieder auch politisch als Vermittler tätig, der türkische Premierminister Turgut Özal besuchte ihn 1985, um in den Beziehungen zwischen der Türkei und Griechenland zu vermitteln.[3] Iakovos hatte zu Griechenlands sozialistischem Premier Andreas Papandreou jedoch selbst kein gutes Verhältnis. Iakovos erreichte eine kleine Annäherung der beiden Länder in der Zypernfrage und das Treffen von Davos. Beide Kandidaten des Präsidentschaftswahlkampfes 1980 baten Iakovos um Unterstützung,[4] er sagte beiden gemeinsame Auftritte zu, jedoch unter der Bedingung für keinen der beiden Partei zu beziehen.\n\nNachdem sich in den 1990er Jahren die Meinungsverschiedenheiten mit dem neuen Patriarchen Bartholomäus I. mehrten, ging er in den Ruhestand. Er verstarb am 10. April 2005.";

    bodyParserTest(text, text.size(), 4096, 89, 10, 100);
}
