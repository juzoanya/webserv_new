/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 13:12:22 by mberline          #+#    #+#             */
/*   Updated: 2023/12/11 07:51:18 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Config.hpp"

Config::Config( void )
{

}

Config::~Config( void )
{
	
}

int	parseDirective(webserv_lexical_t lextoken)
{
	switch (lextoken)
	{
	case WEBSERV_DIRECTIVE_LISTEN:
		break;
	case WEBSERV_DIRECTIVE_SERVER_NAME:
		break;
	case WEBSERV_DIRECTIVE_LOCATION:
		break;
	case WEBSERV_DIRECTIVE_ERROR_PAGE:
		break;
	case WEBSERV_DIRECTIVE_CLIENT_MAX_BODY_SIZE:
		break;
	case WEBSERV_DIRECTIVE_ROOT:
		break;
	case WEBSERV_DIRECTIVE_DIRECTORY_LISTING:
		break;
	case WEBSERV_DIRECTIVE_INDEX:
		break;
	case WEBSERV_DIRECTIVE_ACCEPTED_METHODS:
		break;
	
	default:
		break;
	}
}

int	parseServerConfig(const std::vector<LexToken>& token, std::size_t pos, webserv_lexical_t lexPrev)
{
	ServerConfig	serverConfig;
	webserv_lexical_t	lexCurr = WEBSERV_LEX_MAX;
	for (std::size_t i = 0; token[pos + i].tokenName_ != WEBSERV_SEPARATOR_BRACE_CLOSE && pos + i < token.size(); i++) {
		lexPrev = lexCurr;
		lexCurr = token[pos + i].tokenName_;
		if (lexCurr != WEBSERV_SEPARATOR_BRACE_OPEN)
			mkthrow("missing brace for server directive", "\"{\" ", std::to_string(token[i - 1].lineNo_ + 1));
		// if ((lexPrev == WEBSERV_SEPARATOR_BRACE_OPEN || lexPrev == WEBSERV_SEPARATOR_SEMICOLON)
		// 	&& !(lexCurr & WEBSERV_LEXTYPE_DIRECTIVE))
		// 	mkthrow("missing brace for server directive", "\"{\" ", std::to_string(token[i - 1].lineNo_ + 1));
		if (lexPrev == WEBSERV_SEPARATOR_BRACE_OPEN || lexPrev == WEBSERV_SEPARATOR_SEMICOLON) {
			
		}
		if (lexCurr & WEBSERV_LEXTYPE_DIRECTIVE && !(lexCurr & WEBSERV_LEXTYPE_SERVER_DIRECTIVE)) {
				mkthrow("wrong context for directive", token[i].value_, std::to_string(token[i].lineNo_ + 1));
		}
		if (lexPrev & WEBSERV_LEXTYPE_DIRECTIVE) {
			
		}
	}
	return (0);
}

int Config::parseConfig( const std::vector<LexToken>& token )
{
	webserv_lexical_t	lexPrev = WEBSERV_LEX_MAX;
	webserv_lexical_t	lexCurr = WEBSERV_LEX_MAX;
	for (std::size_t i = 0; i < token.size(); i++) {
        std::cout << webservLexicalNames(token[i].tokenName_) << ": " << token[i].value_ << " | line:" << token[i].lineNo_ << std::endl;
		lexPrev = lexCurr;
		lexCurr = token[i].tokenName_;
		if (lexCurr == WEBSERV_DIRECTIVE_SERVER) {
			if (!(lexPrev == WEBSERV_LEX_MAX || lexPrev == WEBSERV_SEPARATOR_BRACE_CLOSE))
				mkthrow("unknown context for", token[i].value_, std::to_string(token[i].lineNo_));
		}
		if (lexPrev == WEBSERV_DIRECTIVE_SERVER && lexCurr != WEBSERV_SEPARATOR_BRACE_OPEN)
				mkthrow("missing brace for server directive", "\"{\" ", std::to_string(token[i - 1].lineNo_ + 1));
	
	
	
	
	
	
	}
	return (0);
}

int Config::readConfig( const char *filepath )
{
	this->lexer.processFile(filepath);
	const std::vector<LexToken>&	tokenvec = this->lexer.getTokenVector();
	
	parseConfig(tokenvec);
	return (1);
}

int	main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "invalid number of arguments\n";
	}
	Config	conf;
	try
	{
		conf.readConfig(argv[1]);
		// conf.readConfig("/Users/maximilian/coding/c/projects/42coding/62_webserv/webserv/srcs/conf/webserv1.conf");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}
