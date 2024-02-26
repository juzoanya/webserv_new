/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WsSHA.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 18:26:09 by mberline          #+#    #+#             */
/*   Updated: 2024/02/26 10:57:32 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WS_SHA_HPP
#define WS_SHA_HPP

#include <stdint.h>
#include <string>

class WsSHA {
    public:
        WsSHA( void );
        ~WsSHA( void );

        static std::string hash256( std::string text );
        static std::string hexRep( std::string const value );
        static std::string HMAC_createSignature( std::string const & key, std::string const & message );

    private:
        static std::string HMAC_createHashpart( std::string key, bool useIpad );

        static const uint32_t constants[];
        static const uint8_t ipad;
        static const uint8_t opad;
        std::string hash;
        std::string hash_hexrep;
};

#endif
