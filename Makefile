# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: juzoanya <juzoanya@student.42wolfsburg,    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/12/27 11:12:28 by juzoanya          #+#    #+#              #
#    Updated: 2024/02/20 16:02:32 by juzoanya         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -g

FILES = \
		CgiHandler.cpp \
		ConfigHandler.cpp \
		ConfigParser.cpp \
		HttpConstants.cpp \
		HttpHandler.cpp \
		HttpHeader.cpp \
		HttpMessage.cpp \
		HttpServer.cpp \
		main.cpp \
		Polling.cpp \
		utils.cpp

TFILES = \
		HttpRequestTestClass.cpp \
		HttpRequestTest.cpp

BIN_DIR	=	bin
OBJ_DIR	=	build
SRC_DIR	=	src
SRC		=	$(addprefix $(SRC_DIR)/, $(FILES))
OBJ		=	$(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))
T_DIR	=	tests
TSRC		=	$(addprefix $(T_DIR)/, $(TFILES))
TOBJ		=	$(addprefix $(OBJ_DIR)/, $(TFILES:.cpp=.o))

REQUEST_TEST_FILES = HttpConstants HttpHeader HttpMessage utils HttpRequestTestClass HttpRequestTest
REQUEST_TEST_OBJ = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(REQUEST_TEST_FILES)))


HEADERS = headers.hpp
INCS = .


all: $(NAME)

reqTest: $(REQUEST_TEST_OBJ)
	@$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$@

echos:
	echo -n $(SRC)

echoo:
	echo -n $(OBJ)

$(NAME): $(OBJ) | $(BIN_DIR)
	@$(CC) $(CFLAGS) -I$(INCS) $(OBJ) -o $(BIN_DIR)/$(NAME)
	@echo "$(NAME) created!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INCS) -c $< -o $@
	@echo "Compiling..."

$(OBJ_DIR)/%.o: $(T_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INCS) -c $< -o $@
	@echo "Compiling..."


$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
	@/bin/rm -f $(OBJ) $(TOBJ)
	@/bin/rm -r $(OBJ_DIR)

fclean: clean
	@/bin/rm -f $(BIN_DIR)/$(NAME)

re: fclean all

.PHONY: all clean fclean re