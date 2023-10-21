CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
# CPPFLAGS = -std=c++98
NAME = ircserv
SRC = main.cpp utils.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re