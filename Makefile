NAME = my_tar
CC = gcc
SRCS = *.c
CFLAGS  += -Wall -Wextra# -Werror
SANITIZE = -g3 -fsanitize=address

$(NAME):
	$(CC) $(CFLAGS) $(SANITIZE) my_tar.c -o $(NAME)

all: $(NAME)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)

re: fclean all

// tojiboyev Javohir
