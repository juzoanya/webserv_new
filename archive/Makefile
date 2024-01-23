# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/27 11:12:28 by juzoanya          #+#    #+#              #
#    Updated: 2024/01/08 21:31:51 by juzoanya         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

SRC = 	HttpServer.cpp \
		RequestHandler.cpp \
		ResponseHandler.cpp \
		ConfigHandler.cpp \
		ConfigParser.cpp \
		HttpConstants.cpp \
		HttpStatic.cpp \
		main.cpp

HEADERS = headers.hpp
INCS = .

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	@$(CC) $(CFLAGS) -I$(INCS) -c $< -o $@
	@echo "Compiling..."

$(NAME): $(OBJ) $(HEADERS)
	@$(CC) $(CFLAGS) -I$(INCS) $(OBJ) -o $(NAME)
	@echo "$(NAME) created!"

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re