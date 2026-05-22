CC=gcc
INCLUDE=include

SRC=$(wildcard src/*.c)
OBJ=${SRC:.c=.o}
EXE=solver.out

CFLAGS=-lm -g -O0

all: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o ${EXE}

clean:
	${RM} ${OBJ} ${EXE}
