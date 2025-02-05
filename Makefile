.PHONY: all clean fclean re lib

NAME = pipex
CC = cc
CFLAGS = -Wall -Werror -Wextra -g3

SRC_FILE =	pipex.c\

OBJ = ${SRC_FILE:.c=.o}

LIB_NAME = libft.a
LIB_DIR = libft/

INC = -I . -I ${LIB_DIR}

PARAM = input "wc -l" "wc -m" output

%.o: %.c 
	${CC} ${CFLAGS} ${INC} -c $< -o $@

all:
	${MAKE} ${NAME}

${NAME}: ${OBJ} pipex.h
	${MAKE} lib
	${CC} ${CFLAGS} -g3 ${INC} ${OBJ} ${LIB_NAME} -o ${NAME}

lib:
	${MAKE} -C ${LIB_DIR}
	cp ${LIB_DIR}${LIB_NAME} ${LIB_NAME}

clean:
	rm -f ${OBJ}

fclean: 
	${MAKE} -C ${LIB_DIR} fclean
	${MAKE} clean
	rm -f ${LIB_NAME}
	rm -f ${NAME}

re:
	${MAKE} fclean
	${MAKE} all

run:
	${MAKE}
	./${NAME} ${PARAM}

val:
	${MAKE}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --track-fds=yes ./${NAME} ${PARAM}