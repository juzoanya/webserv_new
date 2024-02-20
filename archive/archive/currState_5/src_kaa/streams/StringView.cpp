/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringView.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 10:00:18 by mberline          #+#    #+#             */
/*   Updated: 2024/02/08 10:54:05 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StringView.hpp"

StringView::StringView( void ) : _ptr(NULL), _size(0)
{ }

StringView::StringView( std::string::iterator itStart, std::string::iterator itEnd ) : _ptr(&(*itStart)), _size(itEnd - itStart)
{ }

StringView::StringView( std::string::const_iterator itStart, std::string::const_iterator itEnd ) : _ptr(&(*itStart)), _size(itEnd - itStart)
{ }

StringView::StringView( const char * cStr ) : _ptr(NULL), _size(0)
{
    if (cStr) {
        this->_ptr = cStr;
        this->_size = std::strlen(cStr);
    }
}

StringView::StringView( const char * cStr, std::size_t len ) : _ptr(NULL), _size(0)
{
    if (cStr) {
        this->_ptr = cStr;
        std::size_t realLen = std::strlen(cStr);
        this->_size = realLen < len ? realLen : len;
    }
}

StringView::StringView( std::string const & str ) : _ptr(str.data()), _size(str.length())
{ }

StringView::~StringView( void )
{ }


StringView const & StringView::operator=( std::string const & str )
{
    this->_ptr = str.data();
    this->_size = str.length();
    return (*this);
}

bool StringView::operator==( std::string const & str ) const
{
    return (this->_size == str.size() && std::strncmp(this->_ptr, str.data(), str.size()) == 0);
}

bool StringView::operator!=( std::string const & str ) const
{
    return (!this->operator==(str));
}

char const & StringView::operator[]( std::size_t i ) const
{
    if (i >= this->_size)
        throw std::range_error("out of bounds");
    return (*(this->_ptr + i));
}

bool StringView::empty( void ) const
{
    return (this->_size == 0);
}

std::size_t StringView::size( void ) const
{
    return (this->_size);
}

std::size_t StringView::find(std::string const & str ) const
{
    const char * resPtr = std::search(this->_ptr, this->_ptr + this->_size, str.begin(), str.end());
    if (resPtr == this->_ptr + this->_size)
        return (std::string::npos);
    return (resPtr - this->_ptr);
}

std::size_t StringView::find( const char c ) const
{
    const char * resPtr = std::find(this->_ptr, this->_ptr + this->_size, c);
    if (resPtr == this->_ptr + this->_size)
        return (std::string::npos);
    return (resPtr - this->_ptr);
}

std::ostream& operator<<( std::ostream & os, StringView const & rhs )
{
    for (std::size_t i = 0; i != rhs.size(); ++i)
        os << rhs[i];
    return (os);
}

int main( void )
{
    std::string bla = "hallo, moin!";
    StringView joo(bla);
    std::cout << joo << std::endl;

    StringView hallo = bla;
    std::cout << hallo << std::endl;
    if (hallo == "hallo, moin!")
        std::cout << "is eqal!" << std::endl;
    if (hallo.find("hal") != std::string::npos)
        std::cout << "has hallo at begin" << std::endl;
    return (0);
}
