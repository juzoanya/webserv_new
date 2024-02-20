/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexPars.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 18:55:54 by mberline          #+#    #+#             */
/*   Updated: 2023/12/11 07:45:47 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXPARS_HPP
#define LEXPARS_HPP

#include <exception>
#include <string>
#include <vector>
#include <utility>
#include <map>

typedef enum {
    WEBSERV_LEXTYPE_SEPARATOR = 0x100,
    WEBSERV_LEXTYPE_DIRECTIVE = 0x200,
    WEBSERV_LEXTYPE_SERVER_DIRECTIVE = 0x400,
    WEBSERV_LEXTYPE_LOCATION_DIRECTIVE = 0x800
}   webserv_lextype_t;

// typedef enum {
//     WEBSERV_LEX_VALUE = 0,
//     WEBSERV_COMMENT,
//     WEBSERV_SEPARATOR_QUOTE,
//     WEBSERV_SEPARATOR_SEMICOLON             = WEBSERV_LEXTYPE_SEPARATOR,
//     WEBSERV_SEPARATOR_BRACE_OPEN,
//     WEBSERV_SEPARATOR_BRACE_CLOSE,
// 	WEBSERV_DIRECTIVE_SERVER                = 1  + WEBSERV_LEXTYPE_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_LISTEN                = 2  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_SERVER_NAME           = 3  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_LOCATION              = 4  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_ERROR_PAGE            = 5  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE  = 6  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_ROOT                  = 7  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_DIRECTORY_LISTING     = 8  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_INDEX                 = 9  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
// 	WEBSERV_DIRECTIVE_ACCEPTED_METHODS      = 10 + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
//     WEBSERV_LEX_MAX = 16
// }   webserv_lexical_t;


typedef enum {
    WEBSERV_LEX_VALUE = 0,
    WEBSERV_COMMENT,
    WEBSERV_SEPARATOR_QUOTE,
    WEBSERV_SEPARATOR_SEMICOLON             = WEBSERV_LEXTYPE_SEPARATOR,
    WEBSERV_SEPARATOR_BRACE_OPEN,
    WEBSERV_SEPARATOR_BRACE_CLOSE,
	WEBSERV_DIRECTIVE_SERVER                = 1  + WEBSERV_LEXTYPE_DIRECTIVE,
	WEBSERV_DIRECTIVE_LISTEN                = 2  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
	WEBSERV_DIRECTIVE_SERVER_NAME           = 3  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
	WEBSERV_DIRECTIVE_LOCATION              = 4  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE,
	WEBSERV_DIRECTIVE_ERROR_PAGE            = 5  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
	WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE  = 6  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
	WEBSERV_DIRECTIVE_ROOT                  = 7  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
	WEBSERV_DIRECTIVE_DIRECTORY_LISTING     = 8  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
	WEBSERV_DIRECTIVE_INDEX                 = 9  + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
	WEBSERV_DIRECTIVE_ACCEPTED_METHODS      = 10 + WEBSERV_LEXTYPE_DIRECTIVE | WEBSERV_LEXTYPE_SERVER_DIRECTIVE | WEBSERV_LEXTYPE_LOCATION_DIRECTIVE,
    WEBSERV_LEX_MAX = 16
}   webserv_lexical_t;


struct LexToken {
    LexToken(webserv_lexical_t tokenName, std::string value, int lineNo);
    webserv_lexical_t   tokenName_;
    std::string         value_;
    int                 lineNo_;
};

typedef std::pair<webserv_lexical_t, std::string>   lex_token_t;

class   LexPars {
    public:
        LexPars( void );
        ~LexPars( void );

        // static  lex_token_t const_lex[WEBSERV_LEX_MAX];
        static  LexToken const_lex[WEBSERV_LEX_MAX];

        const   std::vector<LexToken>&   getTokenVector( void ) const;
        int     processFile(const char *filepath);
    private:
        int     pushToken(const char *buffer, std::size_t pos, std::size_t bufferSize, webserv_lexical_t lex);

        std::vector<LexToken>    token;
        int     currLine;
    
};

const char *webservLexicalNames(webserv_lexical_t e);
void mkthrow(std::string msg1, std::string msg2, std::string msg3 );

#endif
