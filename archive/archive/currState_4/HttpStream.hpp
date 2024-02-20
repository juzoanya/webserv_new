/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStream.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 13:53:29 by mberline          #+#    #+#             */
/*   Updated: 2024/02/04 13:55:26 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_STREAM_HPP
#define HTTP_STREAM_HPP

#include <streambuf>

class httpstream : public std::streambuf {
    public:
        httpstream( void );
        ~httpstream( void );
};

#endif