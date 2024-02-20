NAME	=	webserv
CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -std=c++98 -pedantic

FILES	=	ConfigParser

BIN_DIR	=	bin
OBJ_DIR	=	build
SRC_DIR	=	src
SRC		=	$(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJ		=	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(FILES)))
BIN		=	$(addprefix $(BIN_DIR)/, $(NAME))

all: $(NAME)

$(NAME): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -g $(OBJ) -o $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -g -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re