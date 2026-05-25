CC=gcc
INCLUDE=include

SRC=$(wildcard src/*.c)
OBJ=${SRC:.c=.o}
EXE=solver.exe

# CFLAGS=-lm -g -O0 -fsanitize=address
CFLAGS=-lm -O3
LDFLAGS=-fsanitize=address

all: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o ${EXE}

clean:
	${RM} ${OBJ} ${EXE}
