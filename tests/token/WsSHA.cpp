/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WsSHA.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 18:26:09 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 11:08:02 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WsSHA.hpp"
#include <iostream>
#include <bitset>
#include <iomanip>
#include <sstream>

void    printAsBin(std::string const & msg, unsigned int c)
{
    std::cout << msg << std::bitset<8>((c >> 24) & 0x000000FF) << " "
        << std::bitset<8>((c >> 16) & 0x000000FF) << " "
        << std::bitset<8>((c >> 8) & 0x000000FF) << " "
        << std::bitset<8>((c) & 0x000000FF) << std::endl;
}

const uint32_t WsSHA::constants[] =  {   
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint8_t WsSHA::ipad = 0x36;
const uint8_t WsSHA::opad = 0x5C;

uint32_t rotleft(uint32_t i, uint32_t step)
{
    return ((i << step) | (i >> (((sizeof(uint32_t) * 8) - step))));
}

uint32_t rotright(uint32_t i, uint32_t step)
{
    return ((i >> step) | (i << (((sizeof(uint32_t) * 8) - step))));
}

std::string WsSHA::hash256( std::string text )
{
    uint32_t hashValues[] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
    
    uint64_t    textLen = text.size() * 8;
    text.push_back(static_cast<char>(0x80));
    while (text.size() % 64 != 56)
        text.push_back(static_cast<char>(0x00));
    for (int i = 7; i >= 0; --i)
        text.push_back((textLen >> (i * 8)) & 0xFF);

    for (std::size_t i = 0; i < text.size(); i += 64) {

        uint32_t words[64];

        for (int j = 0; j != 16; ++j) {
            words[j] = (0x000000FF & text[i + j * 4 + 0]) << 24 | (0x000000FF & text[i + j * 4 + 1]) << 16
                        | (0x000000FF & text[i + j * 4 + 2]) << 8 | (0x000000FF & text[i + j * 4 + 3]);
            // printAsBin("word " + std::to_string(j) + ": ", words[j]);
        }
            // printAsBin("word " + std::to_string(0) + ": ", words[0]);

        for (int j = 16; j != 64; ++j) {
            // uint32_t s0 = (words[j - 15] >> 7 | words[j - 15] << 25) ^ (words[j - 15] >> 18 | words[j - 15] << 14) ^ (words[j - 15] >> 3);
            // uint32_t s1 = (words[j - 2] >> 17 | words[j - 2] << 15) ^ (words[j - 2] >> 19 | words[j - 2] << 13) ^ (words[j - 2] >> 10);
            // words[j] = words[j - 16] + s0 + words[j - 7] + s1;
            uint32_t sigma0 = rotright(words[j - 15], 7) ^ rotright(words[j - 15], 18) ^ (words[j - 15] >> 3);
            uint32_t sigma1 = rotright(words[j - 2], 17) ^ rotright(words[j - 2], 19) ^ (words[j - 2] >> 10);
            words[j] = words[j - 16] + sigma0 + words[j - 7] + sigma1;
            // printAsBin("word " + std::to_string(j) + ": ", words[j]);
        }

        uint32_t a = hashValues[0];
        uint32_t b = hashValues[1];
        uint32_t c = hashValues[2];
        uint32_t d = hashValues[3];
        uint32_t e = hashValues[4];
        uint32_t f = hashValues[5];
        uint32_t g = hashValues[6];
        uint32_t h = hashValues[7];

       

        for (int j = 0; j < 64; ++j) {

            // uint32_t sigma1 = (e >> 6 | e << 26) ^ (e >> 11 | e << 21) ^ (e >> 25 | e << 7);
            // uint32_t sigma0 = (a >> 2 | a << 30) ^ (a >> 13 | a << 19) ^ (a >> 22 | a << 10);
            uint32_t sigma1 = rotright(e, 6) ^ rotright(e, 11) ^ rotright(e, 25);
            uint32_t sigma0 = rotright(a, 2) ^ rotright(a, 13) ^ rotright(a, 22);
            uint32_t choose = (e & f) ^ (~e & g);
            uint32_t temp1 = h + sigma1 + choose + constants[j] + words[j];
            uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = sigma0 + majority;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;


        }
        hashValues[0] += a;
        hashValues[1] += b;
        hashValues[2] += c;
        hashValues[3] += d;
        hashValues[4] += e;
        hashValues[5] += f;
        hashValues[6] += g;
        hashValues[7] += h;

    }
    std::string hash;
    for (int i = 0; i < 8; ++i) {
        hash += (hashValues[i] >> 24) & 0x000000FF;
        hash += (hashValues[i] >> 16) & 0x000000FF;
        hash += (hashValues[i] >> 8) & 0x000000FF;
        hash += (hashValues[i]) & 0x000000FF;
    }
    return (hash);
}

std::string WsSHA::hexRep( std::string const value )
{
    std::stringstream ss;

    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i != value.size(); ++i)
        ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(value[i]));

    return ss.str(); 
}


std::string WsSHA::HMAC_createHashpart( std::string key, bool useIpad )
{
    if (key.size() > 64) {
        key = WsSHA::hash256(key);
    } else {
        while (key.size() < 64)
            key.push_back(static_cast<char>(0x00));
    }
    uint8_t usedPad = useIpad ? WsSHA::ipad : WsSHA::opad;
    for (std::string::iterator it = key.begin(); it != key.end(); ++it)
        *it = static_cast<uint8_t>(*it) ^ usedPad;
    return (key);
}

std::string WsSHA::HMAC_createSignature( std::string const & key, std::string const & message )
{
    std::string innerHashPart = WsSHA::HMAC_createHashpart(key, true);
    std::string outerHashPart = WsSHA::HMAC_createHashpart(key, false); 
    std::string innerHash = WsSHA::hash256(innerHashPart + message);
    std::string outerHash = WsSHA::hash256(outerHashPart + innerHash);
    return (outerHash);
}



int main( int argc, char** argv )
{
    (void)argc;
    (void)argv;
    if (argc != 2)
        return (1);
    std::string hash = WsSHA::hexRep(WsSHA::hash256(argv[1]));
    std::cout << "hashed: " << hash << std::endl;
    std::string hmacSignature = WsSHA::HMAC_createSignature("hallo", "guten tag");
    std::cout << "hmacSignature: " << WsSHA::hexRep(hmacSignature) << std::endl;
    return (0);
}

