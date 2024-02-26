/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hexTest.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 19:29:26 by mberline          #+#    #+#             */
/*   Updated: 2024/02/18 11:25:38 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits>
#include <cctype>
#include <iostream>
#include <sstream>
#include <iomanip>


int getCharFromHex(unsigned char a)
{
    std::cout << "getCharFromHex: a: " << (int)a << std::endl;
    if (std::isdigit(a))
        return (a - '0');
    if (a >= 'A' && a <= 'F')
        return (a + 10 - 'A');
    if (a >= 'a' && a <= 'f')
        return (a + 10 - 'a');
    return (std::numeric_limits<unsigned char>::max());
}

void    printCharDecoded(const char *arg)
{
    const char *str = arg;
    std::cout << "'a': " << (int)'a' << std::endl;
    std::cout << "('a' + 10): " << ('a' + 10) << std::endl;

    long part = 0;
    for(; *str; ++str) {
        int currPart = getCharFromHex(*str);
        std::cout << "currPart: " << currPart << std::endl;
        part = 16 * part + currPart;
    }
    std::cout << "\narg: " << arg << std::endl;
    std::cout << "nbr: " << part << std::endl;

    std::stringstream ss;
    ss << std::hex << arg;
    std::cout << "real hex to dec: " << ss.str() << std::endl;

}



// bool urlDecodeString(std::string const & value, std::string & result)
// {
//     result.clear();
//     std::string allowed = "!$&'()*+,;=-._~@:/";
//     std::istringstream iss(value);
//     std::ostringstream decoded;
//     char c;
//     int decod;
//     while (iss.get(c)) {
//         if (c == '%') {
//             std::string tmp;
//             iss >> std::setw(2) >> tmp;
//             std::istringstream(tmp) >> std::hex >> decod;
//             if (decod == 0)
//                 return (false);
//             c = decod;
//         } else if (!(isalnum(c) || allowed.find(c) != std::string::npos)) {
//             return (false);
//         }
//         decoded << c;
//     }
//     result = decoded.str();
//     return (true);
// }

bool urlDecodeString(std::string const & value, std::string & result)
{
    result.clear();
    std::string allowed = "!$&'()*+,;=-._~@:/";
    std::ostringstream decoded;
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        unsigned char c = *i;
        std::cout << "char: " << c << std::endl;
        if (c == '%') {
            std::cout << "str rep: " << std::string(i + 1, i + 3) << std::endl;
            std::istringstream(std::string(i + 1, i + 3)) >> std::hex >> c;
            std::cout << " -> decod: " << (int)c << std::endl;
            if (c == 0)
                return (false);
            i += 2;
            decoded << c;
        } else if (isalnum(c) || allowed.find(c) != std::string::npos) {
            decoded << c;
        } else {
            return (false);
        }
    }
    result = decoded.str();
    return (true);
}

std::string urlEncodeString(std::string const & value)
{
    std::string allowed = "!$&'()*+,;=-._~@:/";
    std::ostringstream escaped;
    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        unsigned char c = *i;
        if (isalnum(c) || allowed.find(c) != std::string::npos) {
            escaped << c;
        } else {
            escaped << '%' << std::hex << std::uppercase << std::setw(2) << static_cast<int>(c) << std::nouppercase;
        }
    }
    return (escaped.str());
}


int main( int argc, char**argv )
{
    if (argc != 2)
        return (1);
    
    // std::string decoded;
    // if (!urlDecodeString(argv[1], decoded)) {
    //     std::cout << "error decoding" <<std::endl;
    //     return (0);
    // }
    // std::cout << std::setw(8) << "before: ";
    // std::cout << argv[1] << std::endl;
    // std::cout << std::setw(8) << "decoded: ";
    // std::cout << decoded << std::endl;
    
    std::string encoded, decoded;
    encoded = urlEncodeString(argv[1]);
    if (!urlDecodeString(encoded, decoded)) {
        std::cout << "error decoding" <<std::endl;
        return (0);
    }
    std::cout << std::setw(10) << "before: ";
    std::cout << argv[1] << std::endl;
    std::cout << std::setw(10) << "encoded: ";
    std::cout << encoded << std::endl;
    std::cout << std::setw(10) << "decoded: ";
    std::cout << decoded << std::endl;
    return (0);
}
