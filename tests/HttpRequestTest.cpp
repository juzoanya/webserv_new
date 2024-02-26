/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestTest.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 17:55:55 by mberline          #+#    #+#             */
/*   Updated: 2024/02/14 12:06:05 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestTestClass.hpp"

int main( void )
{
    // HttpRequestTest test;
    // std::random_device rd;
    // std::mt19937 gen(rd()); 
    // std::uniform_int_distribution<> distrChunkSize(20, 1000);
    // std::uniform_int_distribution<> distrMinSize(20, 70);
    // std::uniform_int_distribution<> distrMaxSize(80, 200);
    // for (int i = 0; i != 10000; ++i) {
    //     std::cout << distrChunkSize(gen) << std::endl;
    //     std::cout << distrMinSize(gen) << std::endl;
    //     std::cout << distrMaxSize(gen) << std::endl;
    //     test.testBodyParsing(distrChunkSize(gen) ,distrMinSize(gen), distrMaxSize(gen));
    // }

    std::cout << "size of HttpHeader: " << sizeof(HttpHeader) << std::endl;
    std::cout << "size of HttpMessage: " << sizeof(HttpMessage) << std::endl;

    
    
    HttpRequestTest test;
    // test.testHeaderParsing();
    // test.testHeaderParsingChunked();
    // test.testHeaderParsing(300, 600);
    return (0);
}
