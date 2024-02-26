/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestTestClass.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 16:32:31 by mberline          #+#    #+#             */
/*   Updated: 2024/02/14 22:31:07 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_TEST_CLASS_HPP
#define HTTP_REQUEST_TEST_CLASS_HPP

#include <random>
#include "../src/headers.hpp"


class HttpRequestTest {
    public:
        HttpRequestTest( void );
        ~HttpRequestTest( void );

        static std::string request;
        static std::string requestHeader;
        static std::string requestBody;
        static std::vector<std::string> splitStringRandomly(std::string input, std::size_t minPartStringSize, std::size_t maxPartStringSize);
        static std::string makeChunked(std::string str, int chunkSize);
        static std::string getNumberStringHex(long number);

        int testHeaderParsingChunked( int minPartSize=10, int maxPartSize=30, int chunkSize=30 );
        int testHeaderParsing( int minPartSize=10, int maxPartSize=30 );
    private:
        int testBodyParsing( int chunkSize=30, int minPartSize=50, int maxPartSize=100 );
        int testHeaderParsingInt( std::string re, int minPartSize, int maxPartSize );
        

        std::vector<std::string>    _headerSplitted;
        std::vector<std::string>    _bodySplitted;
        std::vector<std::string>    _requestSplitted;
};

#endif
