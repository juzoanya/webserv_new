/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParserTests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:53:12 by mberline          #+#    #+#             */
/*   Updated: 2023/11/03 21:04:20 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <random>


#define CR "\r"
#define LF "\n"

// std::vector<std::string> SplitStringRandomly(std::string& input, int numParts) {
//     std::vector<std::string> parts;
//     int inputLength = input.length();
//     std::srand(std::time(nullptr));

//     for (int i = 0; i < numParts - 1; ++i) {
//         int randomPos = std::rand() % inputLength;
//         parts.push_back(input.substr(0, randomPos));
//         input.erase(0, randomPos);
//         inputLength = input.length();
//     }

//     parts.push_back(input);  // Das letzte Teil ist der Rest des Strings
//     return parts;
// }


// std::vector<std::string> SplitStringRandomly(std::string& input, int numParts) {
//     std::vector<std::string> parts;
//     int inputLength = input.length();
//     std::random_device rd;
//     std::default_random_engine generator(rd());

//     for (int i = 0; i < numParts - 1; ++i) {
//         std::uniform_int_distribution<int> distribution(0, inputLength);
//         int randomPos = distribution(generator);
//         parts.push_back(input.substr(0, randomPos));
//         input.erase(0, randomPos);
//         inputLength = input.length();
//     }

//     parts.push_back(input);  // Das letzte Teil ist der Rest des Strings
//     return parts;
// }


// std::vector<std::string> SplitStringRandomly(std::string& input, int numParts) {
//     std::vector<std::string> parts;
//     int inputLength = input.length();
//     std::random_device rd;
//     std::default_random_engine generator(rd());

//     if (numParts <= 0) {
//         // Fehler: ungültige Parameter
//         return parts;
//     }

//     for (int i = 0; i < numParts; ++i) {
//         int maxPartLength = inputLength / (numParts - i);
//         std::uniform_int_distribution<int> distribution(1, maxPartLength);
//         int randomPartLength = distribution(generator);

//         if (randomPartLength <= inputLength) {
//             parts.push_back(input.substr(0, randomPartLength));
//             input.erase(0, randomPartLength);
//             inputLength = input.length();
//         } else {
//             // Der Rest des Strings ist zu kurz, um weitere Teile zu erstellen.
//             break;
//         }
//     }

//     return parts;
// }


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



void comparerequest(HttpRequest & req, bool cmpBody, std::size_t bodySize, std::string const & reqbody)
{
    if (req.getMethod() != 0)
        std::cout << "Cmp Method: " << req.getMethod() << std::endl;
    if (req.getVersion() != 1)
        std::cout << "Cmp Version: " << req.getVersion() << std::endl;
    if (req.getPath() != "/index.html")
        std::cout << "Cmp Path: " << req.getPath() << std::endl;
    if (req.getHeader("Host") != "www.example.com")
        std::cout << "Cmp Host: " << req.getHeader("Host") << std::endl;
    if (req.getHeader("User-Agent") != "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36")
        std::cout << "Cmp User-Agent: " << req.getHeader("User-Agent") << std::endl;
    if (req.getHeader("Accept") != "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8")
        std::cout << "Cmp Accept: " << req.getHeader("Accept") << std::endl;
    if (req.getHeader("Accept-Language") != "en-US,en;q=0.5")
        std::cout << "Cmp Accept-Language: " << req.getHeader("Accept-Language") << std::endl;
    if (req.getHeader("Connection") != "keep-alive")
        std::cout << "Cmp Connection: " << req.getHeader("Connection") << std::endl;
    if (req.getHeader("Content-Length") != std::to_string(bodySize)) {
        std::cout << "Cmp Content-Length: " << req.getHeader("Content-Length") << std::endl;
        std::cout << "std::to_string(bodySize): " << std::to_string(bodySize) << std::endl;
    }

    (void)cmpBody;
    (void)reqbody;
    // if (cmpBody) {
    //     // std::string reqbody =
    //     //     "<!DOCTYPE html>\r\n"
    //     //     "<html lang=\"en\">\r\n"
    //     //     "<head>\r\n"
    //     //     "<meta charset=\"UTF-8\">\r\n"
    //     //     "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
    //     //     "<title>";
    //     if (std::string(req.getBody().begin(), req.getBody().end()) != reqbody)
    //         std::cout << "Cmp Body: " << std::string(req.getBody().begin(), req.getBody().end()) << std::endl;
    // }
}

void requestSplitTest(std::size_t minPartStringSize, std::size_t maxPartStringSize, int numTests, bool cmpBody, bool showDetail)
{
    std::string httpReqBody =
            "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
            "<title>";
    std::string httpReqHeader = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: ";
    httpReqHeader += std::to_string(httpReqBody.size()) + ws_http::crlf;
    std::string httpRequest = httpReqHeader + ws_http::crlf + httpReqBody;

    int i = 0;
    while (numTests--) {
        if (showDetail)
            std::cout << "\n --- Round: " << i++ << " ---" << std::endl;
        HttpRequest req;
        std::string reqToCheck = httpRequest;
        std::vector<std::string> httpRequestParts = SplitStringRandomly(reqToCheck, minPartStringSize, maxPartStringSize);
        std::vector<std::string>::iterator it;
        for (it = httpRequestParts.begin(); it != httpRequestParts.end(); ++it) {
            std::string& part = *it;
            if (showDetail) {
                std::cout << "---------- PART -----------" << std::endl;
                std::cout << part << std::endl;
                std::cout << "----------------------------" << std::endl;
            }
            ws_http_parser_status_t status = req.parseRequest(part.begin(), part.end());
            if (status == WS_HTTP_PARSING_DONE) {
                std::cout << "done parsing" << std::endl;
                break ;
            } else if (status >= WS_HTTP_PARSE_ERROR_DEFAULT) {
                std::cout << "error: " << status << std::endl;
                exit(1);
            } else {
                std::cout << "KA!: " << status << std::endl;
                exit(1);
            }
            if (showDetail) {
                std::cout << "return value: " << req.getStatus() << std::endl;
                std::cout << "--------- COMPARE -----------" << std::endl;
                std::cout << "Compare: " << std::endl;
                comparerequest(req, cmpBody, 2, ws_http::crlf);
                std::cout << "----------------------------" << std::endl;
            }
        }
        if (!showDetail) {
            comparerequest(req, cmpBody, httpReqBody.size(), ws_http::crlf);
        }
    }
}

#include <chrono>

void    httpParserPerformanceTest()
{
   std::string httpRequest =
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: 147\r\n"
            "\r\n"
            "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
            "<title>";

    HttpRequest req;
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    req.parseRequest(httpRequest.begin(), httpRequest.end());
    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time duration: " << duration.count() << std::endl;
}


void    httpParserSimpleTest()
{
    std::string httpReqBody =
            "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
            "<title>";
    std::string httpReqHeader = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: ";
    std::string httpRequestRealHeader = httpReqHeader + std::to_string(httpReqBody.size());
    std::string httpRequest = httpRequestRealHeader + ws_http::crlf + ws_http::crlf + httpReqBody;
    std::cout << "size of real Header: " << httpRequestRealHeader.size() << std::endl;

    HttpRequest req(100);
    req.parseRequest(httpRequest.begin(), httpRequest.end());
    std::cout << "result status: " << req.getStatus() << std::endl;
    req.printRequest();
}


void    httpParserSplitEndTest()
{
    std::string httpReqBody =
            "<!DOCTYPE html>\r\n"
            "<html lang=\"en\">\r\n"
            "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
            "<title>";
    std::string httpReqHeader = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: www.example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: ";
    // std::string end1 = "\r";
    // std::string end2 = "\n\r";
    // std::string end3 = "\nfwfweg435";
    // std::string end4 = " 3g34 g34g";

    std::string httpRequestRealHeader = httpReqHeader + std::to_string(httpReqBody.size()) + ws_http::crlf;
    std::cout << "size of real Header: " << httpRequestRealHeader.size() << std::endl;

    std::string parse1 = httpRequestRealHeader + ws_http::crlf + httpReqBody;
    // std::string parse2 = end2;
    // buff_t::const_iterator itp1End = parse1.end();
    // buff_t::const_iterator itp2End = parse2.end();

    HttpRequest req(4096);
    
    std::cout << "--------- 1 ----------" << std::endl;
    req.parseRequest(parse1.begin(), parse1.end());
    std::cout << "\nreturn value parse1: " << req.getStatus() << std::endl;
    std::cout << "----------------------" << std::endl;
    req.printRequest();
    buff_t const& body = req.getBody();
    std::cout << std::string(body.begin(), body.end()) << std::endl;
    
    // std::cout << "--------- 2 ----------" << std::endl;
    // req.parseRequest(end2.begin(), end2.end());
    // std::cout << "\nreturn value parse2: " << req.getStatus() << std::endl;
    // std::cout << "----------------------" << std::endl;
    // std::cout << "--------- 3 ----------" << std::endl;
    // req.parseRequest(end3.begin(), end3.end());
    // std::cout << "\nreturn value parse3: " << req.getStatus() << std::endl;
    // std::cout << "----------------------" << std::endl;
    // std::cout << "--------- 4 ----------" << std::endl;
    // req.parseRequest(end4.begin(), end4.end());
    // std::cout << "\nreturn value parse3: " << req.getStatus() << std::endl;
    // std::cout << "----------------------" << std::endl;

    // buff_t::const_iterator itp1 = req.parseRequest(parse1.begin(), parse1.end());
    // std::cout << "\nreturn value parse1: " << req.getStatus() << std::endl;
    // std::cout << "-------- Buffer Body rest after parse1: -------- \n" << std::string(itp1, itp1End) << std::endl;
    // std::cout << "-----------------------------" << std::endl;
    // buff_t::const_iterator itp2 = req.parseRequest(parse2.begin(), parse2.end());
    // std::cout << "\nreturn value parse2: " << req.getStatus() << std::endl;
    // std::cout << "-------- Buffer Body rest after parse2: -------- \n" << std::string(itp2, itp2End) << std::endl;
    // std::cout << "-----------------------------" << std::endl;
    
}

struct Request {
    HttpRequest request;
    buff_t      requestBody;
};

int main() {
    
    try
    {
        httpParserSplitEndTest();
        std::cout << "sizeof reqineparser: " << sizeof(HttpRequestLineParser) << std::endl;
        std::cout << "sizeof headerparser: " << sizeof(HttpHeaderParser) << std::endl;
        std::cout << "sizeof bodyparser: " << sizeof(HttpBody) << std::endl;
        std::cout << "sizeof requestparser: " << sizeof(HttpRequest) << std::endl;
        std::cout << "sizeof string: " << sizeof(std::string) << std::endl;
        // httpParserSimpleTest();
        // httpParserPerformanceTest();

        // for (int i = 0; i < 3; i++) {
        //     requestSplitTest(i, 100000, true, false);
        // }

        // requestSplitTest(1, 100, 10000, true, false);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
