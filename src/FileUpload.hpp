/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUpload.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 14:26:04 by mberline          #+#    #+#             */
/*   Updated: 2024/02/19 14:30:21 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_UPLOAD_HPP
#define FILE_UPLOAD_HPP

#include "headers.hpp"

class FileUpload {
    public:
        FileUpload( void );
        ~FileUpload( void );

        static const std::string bound; 

        bool    parseMultipart( buff_t::const_iterator it_start, buff_t::const_iterator it_end );
    private:
};

#endif