/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wsJWT.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 11:13:30 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 11:45:29 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WsJWT.hpp"

#include <iostream>
#include <bitset>

WsJWT::WsJWT( std::string const & header, std::string const & payload )
{

}

WsJWT::~WsJWT( void )
{ }

















void    printAsBin(std::string const & msg, unsigned int c)
{
    std::cout << msg << std::bitset<8>((c >> 24) & 0x000000FF) << " "
        << std::bitset<8>((c >> 16) & 0x000000FF) << " "
        << std::bitset<8>((c >> 8) & 0x000000FF) << " "
        << std::bitset<8>((c) & 0x000000FF) << std::endl;
}

const std::string Base64::base64Lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string Base64::base64UrlLookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

std::string Base64::encode( std::string const & binaryStr, bool useBase64Url, bool putPadding )
{
    std::string const & lookup = useBase64Url ? base64UrlLookup : base64Lookup;
    std::string encoded;
    std::string::const_iterator it = binaryStr.begin();
    encoded.reserve( (binaryStr.size() / 3 + binaryStr.size() % 3 > 0) * 4 );
    for (std::size_t i = 0; i != binaryStr.size() / 3; ++i, it += 3) {
        unsigned int triplet = (0x000000FF & it[0]) << 16 | (0x000000FF & it[1]) << 8 | (0x000000FF & it[2]);
        encoded.push_back(lookup[ (triplet >> 18) & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet >> 12) & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet >> 6)  & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet)       & 0x0000003F ]);
    }
    if (binaryStr.size() % 3 == 2) {
        unsigned int triplet = (*it++) << 16 | (*it++) << 8;
        encoded.push_back(lookup[ (triplet >> 18) & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet >> 12) & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet >> 6) & 0x0000003F ]);
        if (putPadding)
            encoded += "=";
    } else if (binaryStr.size() % 3 == 1) {
        unsigned int triplet = (*it++) << 16;
        encoded.push_back(lookup[ (triplet >> 18) & 0x0000003F ]);
        encoded.push_back(lookup[ (triplet >> 12) & 0x0000003F ]);
        if (putPadding)
            encoded += "==";
    }
    return (encoded);
}

std::string Base64::decode( std::string const & base64Str, bool useBase64Url )
{
    std::string const & lookup = useBase64Url ? base64UrlLookup : base64Lookup;
    std::string decoded;
    std::string::const_iterator it = base64Str.begin();
    unsigned int triplet = 0;
    int k = 3;
    for (; it < base64Str.end(); ++it, --k) {
        long dist = base64Str.end() - it;
        if (it[0] != '=') {
            std::size_t pos = lookup.find(*it);
            if (pos == std::string::npos)
                return (std::string(""));
            triplet |= pos << (6 * k);
        }
        if (it[0] == '=' || (dist == 1 && k == 1)) {
            decoded.push_back(triplet >> 16 & 0x000000FF);
            decoded.push_back(triplet >> 8  & 0x000000FF);
            triplet = 0;
            k = 4;
        } else if ((dist > 2 && it[0] == '=' && it[1] == '=') || (dist == 1 && k == 2)) {
            decoded.push_back(triplet >> 16 & 0x000000FF);
            it++;
            triplet = 0;
            k = 4;
        } else if (k == 0) {
            decoded.push_back(triplet >> 16 & 0x000000FF);
            decoded.push_back(triplet >> 8  & 0x000000FF);
            decoded.push_back(triplet & 0x000000FF);
            triplet = 0;
            k = 4;
        }
    }
    return (decoded);
}

int main(int argc, char** argv)
{
    if (argc != 2)
        return (1);
    std::string encoded = Base64::encode(argv[1], false, true);
    std::cout << "encoded: " << encoded << std::endl;
    std::string decoded = Base64::decode(encoded, false);
    std::cout << "decoded: " << decoded << std::endl;
    return (0);
}

















// int main(int argc, char** argv)
// {
//     if (argc != 2)
//         return (1);
//     std::string encoded = Base64::encode(argv[1], false, true);
//     (void)encoded;
//     // std::cout << "encoded: " << encoded << std::endl;
//     std::string newencoded = "aGFsbG9vZmZ3Z3dyb2d3w6TDvMO2w7zDvMO2w7zDtsO8w7bDvMO2w7zCtcKowqp+fcaSwrbiiKvGkuKAnH0=";
//     std::string newencodedoPadd = "aGFsbG9vZmZ3Z3dyb2d3w6TDvMO2w7zDvMO2w7zDtsO8w7bDvMO2w7zCtcKowqp+fcaSwrbiiKvGkuKAnH0";
//     std::string newencodedother = "aGFsbG9vZmZ3Z3dyb2d3w6TDvMO2w7zDvMO2w7zDtsO8w7bDvMO2w7zCtcKowqp+fcaSwrbiiKvGkuKAnH1hYQ==";
//     std::string newencodedotheroPadd = "aGFsbG9vZmZ3Z3dyb2d3w6TDvMO2w7zDvMO2w7zDtsO8w7bDvMO2w7zCtcKowqp+fcaSwrbiiKvGkuKAnH1hYQ";
//     std::string newencodedcat = newencoded + newencoded;
//     std::string newencodedcat2 = newencodedother + newencodedother;
//     (void)newencoded;
//     (void)newencodedoPadd;
//     (void)newencodedother;
//     (void)newencodedotheroPadd;
//     (void)newencodedcat;
//     (void)newencodedcat2;


//     std::string usedstr = newencodedoPadd;
//     std::cout << "parse this: " << usedstr << std::endl;
//     std::string decoded = Base64::decode(usedstr, false);
//     // std::string decoded = Base64::decode(encoded, false);
//     std::cout << "decoded: " << decoded << std::endl;
//     return (0);
// }


// std::string Base64::decode( std::string const & base64Str, bool useBase64Url )
// {
//     std::string const & lookup = useBase64Url ? base64UrlLookup : base64Lookup;
//     std::string decoded;
//     std::string::const_iterator it = base64Str.begin();
//     unsigned int triplet = 0;
//     int k = 3;
//     for (; it != base64Str.end(); ++it, k--) {
//         std::cout << "\nchar: " << *it << " | k: " << k << " | shift by: " << k * 6 << std::endl;
//         if (k == -1) {
//             decoded.push_back(triplet >> 16 & 0x000000FF);
//             // if (base64Str.end() - it > 1)
//             std::cout << "decoded: " << decoded << std::endl;
//             decoded.push_back(triplet >> 8  & 0x000000FF);
//             // if (base64Str.end() - it > 2)
//             std::cout << "decoded: " << decoded << std::endl;
//             decoded.push_back(triplet & 0x000000FF);
//             std::cout << "decoded: " << decoded << std::endl;
//             triplet = 0;
//             k = 3;
//             std::cout << "-> new k: " << k << std::endl;
//         }
//         std::cout << "\nchar: " << *it << " | k: " << k << " | shift by: " << k * 6 << std::endl;
//         if (*it == '=')
//             break ;
//         std::size_t pos = lookup.find(*it);
//         if (pos == std::string::npos)
//             return (std::string(""));
//         printAsBin("triplet before: ", triplet);
//         printAsBin("pos           : ", pos);
//         triplet |= pos << (6 * k);
//         printAsBin("triplet after : ", triplet);
//         // if (k == 0) {
//         //     decoded.push_back(triplet >> 16 & 0x000000FF);
//         //     decoded.push_back(triplet >> 8  & 0x000000FF);
//         //     decoded.push_back(triplet & 0x000000FF);
//         //     triplet = 0;
//         //     k = 4;
//         // }
//     }
//     std::cout << "k end: " << k << std::endl;
//     if (base64Str.end() - it == 1) {
//         decoded.push_back(triplet >> 16 & 0x000000FF);
//         decoded.push_back(triplet >> 8  & 0x000000FF);
//     } else if ((base64Str.end() - it) == 2) {
//         decoded.push_back(triplet >> 10 & 0x000000FF);
//     } else if (base64Str.end() - it > 2) {
//         return (std::string(""));
//     }
//     return (decoded);
// }

// std::string Base64::decode( std::string const & base64Str, bool useBase64Url )
// {
//     std::string const & lookup = useBase64Url ? base64UrlLookup : base64Lookup;
//     std::string decoded;
//     std::string::const_iterator it = base64Str.begin();
//     unsigned int triplet = 0;
//     while (it != base64Str.end()) {
//         triplet = 0;
//         for (int k = 3; k >= 0 && it != base64Str.end(); k--) {
//             if (*it == '=') {

//                 break ;
//             }
//             std::size_t pos = lookup.find(*it);
//             if (pos == std::string::npos)
//                 return (std::string(""));
//             triplet |= pos << (6 * k);
//             it++;
//         }
//         decoded.push_back(triplet >> 16 & 0x000000FF);
//         decoded.push_back(triplet >> 8  & 0x000000FF);
//         decoded.push_back(triplet & 0x000000FF);
//     }
//     std::cout << "end: " << decoded << "base64Str.end() - it: " << (base64Str.end() - it) << std::endl;
//     // if (base64Str.end() - it == 1) {
//     //     decoded.push_back(triplet >> 16 & 0x000000FF);
//     //     decoded.push_back(triplet >> 8  & 0x000000FF);
//     // } else if ((base64Str.end() - it) == 2) {
//     //     decoded.push_back(triplet >> 10 & 0x000000FF);
//     // } else {
//     //     return (std::string(""));
//     // }
//     return (decoded);
// }

// std::string Base64::decode( std::string const & base64Str, bool useBase64Url )
// {
//     std::string const & lookup = useBase64Url ? base64UrlLookup : base64Lookup;
//     std::string decoded;
//     std::string::const_iterator it = base64Str.begin();
//     for (std::size_t i = 0; i != base64Str.size() / 4; ++i, it += 4) {
//         // unsigned int quartet = (0x0000003F & it[0]) << 18 | (0x0000003F & it[1]) << 12 | (0x0000003F & it[2]) << 6 | (0x0000003F & it[3]);
//         // unsigned int quartet = (0x000000FF & it[0]) << 24 | (0x000000FF & it[1]) << 16 | (0x000000FF & it[2]) << 8 | (0x000000FF & it[3]);
//         unsigned int quartet = (0x000000FF & it[0]) << 18 | (0x000000FF & it[1]) << 12 | (0x000000FF & it[2]) << 6 | (0x000000FF & it[3]);
//         std::cout << "quartet: " << quartet << std::endl;
//         std::cout << "quartet binary: " << std::bitset<32>(quartet) << std::endl;
//         for (int k = 4; k >= 0; k--) {
//             std::cout << "k: " << k << " | char: " << ((quartet >> (4 * k)) & 0x000000FF) << std::endl;
//             std::size_t c = lookup.find((quartet >> (4 * k)) & 0x000000FF);
//             // std::cout << "c: " << c << std::endl;
//             // std::cout << "c" << (char)c << std::endl;
//             if (c == std::string::npos)
//                 return (std::string(""));
//             decoded.push_back(c);
//         }

//         // 1100001 00000000 00000000 00000000
//     }
//     return ("");
// }

// const char base64PadCharacter = '=';

// std::string Base64::encode( std::string const & binaryStr )
// {
//     std::string encoded;
//     std::string::const_iterator it = binaryStr.begin();
//     encoded.reserve( (binaryStr.size() / 3 + binaryStr.size() % 3 > 0) * 4 );
//     for (std::size_t i = 0; i != binaryStr.size() / 3; ++i) {
//         std::cout << "a: " << (int)it[0] << std::endl;
//         std::cout << "b: " << (int)it[1] << std::endl;
//         std::cout << "c: " << (int)it[2] << std::endl;

//         unsigned int triplet = (*it++) << 16 | (*it++) << 8 | (*it++);
//         std::cout << "triplet: " << triplet << std::endl;
//         std::cout << "1: " << ((triplet >> 18) & 0x0000003F) << std::endl;
//         std::cout << "2: " << ((triplet >> 12) & 0x0000003F) << std::endl;
//         std::cout << "3: " << ((triplet >> 6)  & 0x0000003F) << std::endl;
//         std::cout << "4: " << ((triplet)       & 0x0000003F) << std::endl;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 6)  & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet)       & 0x0000003F ]);
//     }
    
//     if (binaryStr.size() % 3 == 2) {
//         unsigned int triplet = (*it++) << 16 | (*it++) << 8;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 6) & 0x0000003F ]);
//         encoded.append(1, base64PadCharacter);
//     } else if (binaryStr.size() % 3 == 1) {
//         unsigned int triplet = (*it++) << 16;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         encoded.append(2, base64PadCharacter);
//     }


//     std::cout << "remain: " << binaryStr.size() % 3 << std::endl;
    
//     std::cout << "encoded: " << encoded << std::endl;
//     return (encoded);
// }

// std::string Base64::encode( std::string const & binaryStr )
// {
//     std::string encoded;
//     std::string::const_iterator it = binaryStr.begin();
//     encoded.reserve( (binaryStr.size() / 3 + binaryStr.size() % 3 > 0) * 4 );
//     for (std::size_t i = 0; i != binaryStr.size() / 3; ++i, it += 3) {
//         std::cout << "Encode triple\n";
//         std::cout << "a: " << (int)it[0] << std::endl;
//         std::cout << "a: " << std::bitset<32>( it[0]) << std::endl;
//         std::cout << "a: " << std::bitset<32>( 0x000000FF & it[0]) << std::endl;
//         std::cout << "a: " << std::bitset<32>( 0x00FF0000 & it[0] << 16) << std::endl;
//         std::cout << "b: " << (int)it[1] << std::endl;
//         std::cout << "b: " << std::bitset<32>( it[1]) << std::endl;
//         std::cout << "b: " << std::bitset<32>( 0x000000FF & it[1]) << std::endl;
//         std::cout << "b: " << std::bitset<32>( 0x0000FF00 & it[1] << 8) << std::endl;
//         std::cout << "c: " << (int)it[2] << std::endl;
//         std::cout << "c: " << std::bitset<32>( it[2]) << std::endl;
//         std::cout << "c: " << std::bitset<32>( 0x000000FF & it[2]) << std::endl;

//         unsigned int triplet = (0x000000FF & it[0]) << 16 | (0x000000FF & it[1]) << 8 | (0x000000FF & it[2]);
//         // unsigned int triplet = it[0] << 16 | it[1] << 8 | it[2];
//         // unsigned int triplet = (unsigned int)(*it++) << 16 | (unsigned int)(*it++) << 8 | (unsigned int)(*it++);
//         // unsigned int triplet = (*it++) << 16 | (*it++) << 8 | (*it++);
//         // unsigned int triplet = ((*it++) << 16) + ((*it++) << 8) + (*it++);
//         std::cout << "triplet: " << triplet << std::endl;
//         // std::cout << "1: " << ((triplet >> 18) & 0x0000003F) << std::endl;
//         // std::cout << "2: " << ((triplet >> 12) & 0x0000003F) << std::endl;
//         // std::cout << "3: " << ((triplet >> 6)  & 0x0000003F) << std::endl;
//         // std::cout << "4: " << ((triplet)       & 0x0000003F) << std::endl;
//         // // std::cout << "0x0000003F as bin: " << std::bitset<32>(0x0000003F) << std::endl;
//         std::cout << "1: " << std::bitset<32>(triplet >> 18) << std::endl; std::cout << "1: " << std::bitset<32>(0x0000003F) << std::endl;
//         std::cout << "2: " << std::bitset<32>(triplet >> 12) << std::endl; std::cout << "2: " << std::bitset<32>(0x0000003F) << std::endl;
//         std::cout << "3: " << std::bitset<32>(triplet >> 6)  << std::endl; std::cout << "3: " << std::bitset<32>(0x0000003F) << std::endl;
//         std::cout << "4: " << std::bitset<32>(triplet)       << std::endl; std::cout << "4: " << std::bitset<32>(0x0000003F) << std::endl;
//         // std::cout << "0x0000003F as bin: " << std::bitset<32>(0x0000003F) << std::endl;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 6)  & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet)       & 0x0000003F ]);
//     }
    
//     if (binaryStr.size() % 3 == 2) {
//         unsigned int triplet = (*it++) << 16 | (*it++) << 8;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 6) & 0x0000003F ]);
//         // encoded.append(1, base64PadCharacter);
//         encoded += "=";
//     } else if (binaryStr.size() % 3 == 1) {
//         unsigned int triplet = (*it++) << 16;
//         encoded.push_back(base64Lookup[ (triplet >> 18) & 0x0000003F ]);
//         encoded.push_back(base64Lookup[ (triplet >> 12) & 0x0000003F ]);
//         // encoded.append(2, base64PadCharacter);
//         encoded += "==";
//     }


//     std::cout << "remain: " << binaryStr.size() % 3 << std::endl;
    
//     std::cout << "encoded: " << encoded << std::endl;
//     return (encoded);
// }
