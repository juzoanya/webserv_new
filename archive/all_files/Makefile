# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mberline <mberline@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/27 11:12:28 by juzoanya          #+#    #+#              #
#    Updated: 2024/01/19 11:11:34 by mberline         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# NAME = webserv
# CC = g++
# CFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

# SRC = 	HttpServer.cpp \
# 		ConfigHandler.cpp \
# 		ConfigParser.cpp \
# 		HttpConstants.cpp \
# 		HttpStatic.cpp \
# 		RequestHandler.cpp \
# 		ResponseHandler.cpp \
# 		main.cpp

# HEADERS = headers.hpp
# INCS = .

# OBJ = $(SRC:.cpp=.o)

# all: $(NAME)

# %.o: %.cpp
# 	@$(CC) $(CFLAGS) -I$(INCS) -c $< -o $@
# 	@echo "Compiling..."

# $(NAME): $(OBJ) $(HEADERS)
# 	@$(CC) $(CFLAGS) -I$(INCS) $(OBJ) -o $(NAME)
# 	@echo "$(NAME) created!"

# clean:
# 	@/bin/rm -f $(OBJ)

# fclean: clean
# 	@/bin/rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re

NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -ggdb3

FILES = HttpServer.cpp \
		ConfigHandler.cpp \
		ConfigParser.cpp \
		HttpConstants.cpp \
		HttpStatic.cpp \
		RequestHandler.cpp \
		ResponseHandler.cpp \
		main.cpp

BIN_DIR	=	bin
OBJ_DIR	=	build
SRC_DIR	=	src
SRC		=	$(addprefix $(SRC_DIR)/, $(FILES))
OBJ		=	$(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))
BIN		=	$(addprefix $(BIN_DIR)/, $(NAME))

HEADERS = headers.hpp
INCS = .


all: $(NAME)

echos:
	echo -n $(SRC)

echoo:
	echo -n $(OBJ)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INCS) -c $< -o $@
	@echo "Compiling..."

$(NAME): $(OBJ) | $(BIN_DIR)
	@$(CC) $(CFLAGS) -I$(INCS) $(OBJ) -o $(NAME)
	@echo "$(NAME) created!"

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re