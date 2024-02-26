/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WsJWT.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 11:13:32 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 11:42:07 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WS_JWT_HPP
#define WS_JWT_HPP

#include <string>

class Base64 {
    public:
        Base64( void );
        ~Base64( void );
        static std::string encode( std::string const & binaryStr, bool useBase64Url, bool putPadding );
        static std::string decode( std::string const & base64Str, bool useBase64Url );
    private:
        static const std::string base64Lookup;
        static const std::string base64UrlLookup;
        static const char   base64PadCharacter;
};

class WsJWT {
    public:
        WsJWT( std::string const & header, std::string const & payload );
        ~WsJWT( void );
    private:

};

class WsJWTManager {
    public:
        WsJWTManager( void );
        ~WsJWTManager( void );
        static std::string createJWT( void );
        // static  createJWT( void );
    private:

};

#endif
