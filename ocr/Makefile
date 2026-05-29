CC=gcc
INC=-Isrc

SRC_FILES=$(wildcard src/*.c) $(wildcard src/*/*.c)
SRC=$(filter-out src/main.c, ${SRC_FILES})
OBJ=${SRC:.c=.o}
EXE=ocr.exe

CFLAGS=-lm -O3 $(INC)
LDFLAGS=-lpng

all: ${OBJ}
	
test: ${OBJ} src/main.o
	${CC} ${CFLAGS} ${OBJ} src/main.o -o ${EXE} ${LDFLAGS}

clean:
	${RM} ${OBJ} ${EXE}