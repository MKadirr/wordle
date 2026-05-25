CC=gcc
INC=-Isrc

SRC=$(wildcard src/*.c) $(wildcard src/*/*.c)
OBJ=${SRC:.c=.o}
EXE=ocr.exe

CFLAGS=-lm -O3 $(INC)
LDFLAGS=-lpng

all: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o ${EXE} ${LDFLAGS}

clean:
	${RM} ${OBJ} ${EXE}