/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringView.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 09:59:59 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 10:39:38 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <string>
#include <iostream>

class StringView {
    public:
        StringView( void );
        StringView( std::string::iterator itStart, std::string::iterator itEnd );
        StringView( std::string::const_iterator itStart, std::string::const_iterator itEnd );
        StringView( const char * cStr );
        StringView( const char * cStr, std::size_t len );
        StringView( std::string const & str );
        ~StringView( void );

        StringView const &  operator=( std::string const & str );
        bool operator==( std::string const & str ) const;
        bool operator!=( std::string const & str ) const;
        char const & operator[]( std::size_t i ) const;
        bool empty( void ) const;
        std::size_t size( void ) const;
        std::size_t find( std::string const & str ) const;
        std::size_t find( const char c ) const;
    private:
        const char* _ptr;
        std::size_t _size;
};

std::ostream& operator<<( std::ostream & os, StringView const & rhs );

#endif