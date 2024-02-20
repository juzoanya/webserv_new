/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexPars.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 18:56:21 by mberline          #+#    #+#             */
/*   Updated: 2023/09/27 17:38:53 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "LexPars.hpp"

void mkthrow(std::string msg1, std::string msg2, std::string msg3 )
{
	std::string err = "error: ";
	err.append(msg1);
	err.append(": ");
	err.append(msg2);
	err.append(": ");
	err.append(msg3);
    
    throw std::runtime_error(err.c_str());
}

LexToken::LexToken(webserv_lexical_t tokenName, std::string value, int lineNo = 0)
 : tokenName_(tokenName), value_(value), lineNo_(lineNo)
{
    
}

// lex_token_t    LexPars::const_lex[WEBSERV_LEX_MAX] = {
//     std::make_pair(WEBSERV_LEX_VALUE, ""),
//     std::make_pair(WEBSERV_SEPARATOR_SEMICOLON, ";"),
//     std::make_pair(WEBSERV_SEPARATOR_BRACE_OPEN, "{"),
//     std::make_pair(WEBSERV_SEPARATOR_BRACE_CLOSE, "}"),
//     std::make_pair(WEBSERV_SEPARATOR_QUOTE, "\""),
//     std::make_pair(WEBSERV_COMMENT, "#"),
// 	std::make_pair(WEBSERV_DIRECTIVE_SERVER, "server"),
// 	std::make_pair(WEBSERV_DIRECTIVE_LISTEN, "listen"),
// 	std::make_pair(WEBSERV_DIRECTIVE_SERVER_NAME, "server_name"),
// 	std::make_pair(WEBSERV_DIRECTIVE_ERROR_PAGE, "error_page"),
// 	std::make_pair(WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE, "client_body_size"),
// 	std::make_pair(WEBSERV_DIRECTIVE_LOCATION, "location"),
// 	std::make_pair(WEBSERV_DIRECTIVE_ROOT, "root"),
// 	std::make_pair(WEBSERV_DIRECTIVE_DIRECTORY_LISTING, "directory_listing"),
// 	std::make_pair(WEBSERV_DIRECTIVE_INDEX, "index"),
// 	std::make_pair(WEBSERV_DIRECTIVE_ACCEPTED_METHODS, "accepted_methods"),
// };


LexToken    LexPars::const_lex[WEBSERV_LEX_MAX] = {
    LexToken(WEBSERV_LEX_VALUE, ""),
    LexToken(WEBSERV_SEPARATOR_SEMICOLON, ";"),
    LexToken(WEBSERV_SEPARATOR_BRACE_OPEN, "{"),
    LexToken(WEBSERV_SEPARATOR_BRACE_CLOSE, "}"),
    LexToken(WEBSERV_SEPARATOR_QUOTE, "\""),
    LexToken(WEBSERV_COMMENT, "#"),
	LexToken(WEBSERV_DIRECTIVE_SERVER, "server"),
	LexToken(WEBSERV_DIRECTIVE_LISTEN, "listen"),
	LexToken(WEBSERV_DIRECTIVE_SERVER_NAME, "server_name"),
	LexToken(WEBSERV_DIRECTIVE_ERROR_PAGE, "error_page"),
	LexToken(WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE, "client_body_size"),
	LexToken(WEBSERV_DIRECTIVE_LOCATION, "location"),
	LexToken(WEBSERV_DIRECTIVE_ROOT, "root"),
	LexToken(WEBSERV_DIRECTIVE_DIRECTORY_LISTING, "directory_listing"),
	LexToken(WEBSERV_DIRECTIVE_INDEX, "index"),
	LexToken(WEBSERV_DIRECTIVE_ACCEPTED_METHODS, "accepted_methods"),
};


LexPars::LexPars( void ) : currLine(0)
{
    
}

LexPars::~LexPars( void )
{
    
}

const char *webservLexicalNames(webserv_lexical_t e)
{
    switch (e)
    {
        case WEBSERV_LEX_VALUE: return ("WEBSERV_LEX_VALUE");
        case WEBSERV_SEPARATOR_SEMICOLON: return ("WEBSERV_SEPARATOR_SEMICOLON");
        case WEBSERV_SEPARATOR_BRACE_OPEN: return ("WEBSERV_SEPARATOR_BRACE_OPEN");
        case WEBSERV_SEPARATOR_BRACE_CLOSE: return ("WEBSERV_SEPARATOR_BRACE_CLOSE");
        case WEBSERV_SEPARATOR_QUOTE: return ("WEBSERV_SEPARATOR_QUOTE");
        case WEBSERV_COMMENT: return ("WEBSERV_COMMENT");
        case WEBSERV_DIRECTIVE_SERVER: return ("WEBSERV_DIRECTIVE_SERVER");
        case WEBSERV_DIRECTIVE_LISTEN: return ("WEBSERV_DIRECTIVE_LISTEN");
        case WEBSERV_DIRECTIVE_SERVER_NAME: return ("WEBSERV_DIRECTIVE_SERVER_NAME");
        case WEBSERV_DIRECTIVE_ERROR_PAGE: return ("WEBSERV_DIRECTIVE_ERROR_PAGE");
        case WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE: return ("WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE");
        case WEBSERV_DIRECTIVE_LOCATION: return ("WEBSERV_DIRECTIVE_LOCATION");
        case WEBSERV_DIRECTIVE_ROOT: return ("WEBSERV_DIRECTIVE_ROOT");
        case WEBSERV_DIRECTIVE_DIRECTORY_LISTING: return ("WEBSERV_DIRECTIVE_DIRECTORY_LISTING");
        case WEBSERV_DIRECTIVE_INDEX: return ("WEBSERV_DIRECTIVE_INDEX");
        case WEBSERV_DIRECTIVE_ACCEPTED_METHODS: return ("WEBSERV_DIRECTIVE_ACCEPTED_METHODS");
        case WEBSERV_LEX_MAX: return ("WEBSERV_LEX_MAX");
    }
    return ("");
}

webserv_lexical_t	check_sep(char c)
{
	switch (c)
	{
	case ';':
		return (WEBSERV_SEPARATOR_SEMICOLON);
	case '{':
		return (WEBSERV_SEPARATOR_BRACE_OPEN);
	case '}':
		return (WEBSERV_SEPARATOR_BRACE_CLOSE);
	case '#':
		return (WEBSERV_COMMENT);
	case '"':
		return (WEBSERV_SEPARATOR_QUOTE);
	}
	return (WEBSERV_LEX_MAX);
}

void    pushDirectiveOrValue(std::string newToken, std::vector<LexToken>& token, int currLine)
{
    for (int j = 0; j < WEBSERV_LEX_MAX; j++) {
        if (LexPars::const_lex[j].value_ == newToken) {
            token.push_back(LexToken(LexPars::const_lex[j].tokenName_, LexPars::const_lex[j].value_, currLine));
            return ;
        }
    }
    token.push_back(LexToken(WEBSERV_LEX_VALUE, newToken, currLine));
}

int LexPars::pushToken(const char *buffer, std::size_t pos, std::size_t bufferSize, webserv_lexical_t lex)
{
    if (pos > 0)
        pushDirectiveOrValue(std::string(buffer, pos), this->token, this->currLine);
    if (lex == WEBSERV_SEPARATOR_QUOTE) {
        std::size_t quoteEndLen = pos + 1;
        for (; quoteEndLen < bufferSize && buffer[quoteEndLen] != '"'; quoteEndLen++);
        std::size_t len = quoteEndLen == bufferSize ? quoteEndLen - pos : (quoteEndLen - 1) - pos;
        this->token.push_back(LexToken(WEBSERV_LEX_VALUE, std::string(buffer + pos + 1, len), this->currLine));
        return (quoteEndLen);
    } else if (lex == WEBSERV_COMMENT) {
        std::size_t commentEndLen = pos + 1;
        for (; commentEndLen < bufferSize && buffer[commentEndLen] != '\n'; commentEndLen++);
        this->currLine++;
        return (commentEndLen);
    } else if (lex != WEBSERV_LEX_MAX){
        this->token.push_back(LexToken(lex, "", this->currLine));
    }
    return (pos);
}

int     LexPars::processFile(const char *filepath)
{
	std::ifstream	ifs;
	ifs.open(filepath);
	if (ifs.fail())
		mkthrow("webserv config", filepath, strerror(errno));
    std::stringstream   ss;
    ss << ifs.rdbuf();
    std::string buff = ss.str();
    ifs.close();

    const char *buffer = buff.data();
    std::size_t bufferSize = buff.size();
    const char *curr = NULL;
    webserv_lexical_t lex;
    for (std::size_t i = 0; i < bufferSize; i++) {
        while (buffer[i] && std::isspace(buffer[i]) != 0) {
            if (buffer[i] == '\n')
                this->currLine++;
            i++;
        }
        curr = buffer + i;
        while (buffer[i] && std::isspace(buffer[i]) == 0 && ((lex = check_sep(buffer[i])) == WEBSERV_LEX_MAX)) {
            i++;
        }
        i = (curr + pushToken(curr, (buffer + i) - curr, bufferSize - i, lex)) - buffer;
    }
    return (1);
}

const   std::vector<LexToken>&   LexPars::getTokenVector( void ) const
{
    return (this->token);
}
