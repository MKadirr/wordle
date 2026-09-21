CC=gcc
INCLUDE=-Iocr/src

SRC=$(wildcard src/*.c)
OBJ=${SRC:.c=.o}
EXE=solver.out

# CFLAGS=-lm -g -O0 -fsanitize=address $(INCLUDE)   # debug
# CFLAGS=-lm -O3 $(INCLUDE)                         # release
CFLAGS=-lm -O0 $(INCLUDE) -g3                       # perf
LDFLAGS=-fsanitize=address -lpng
LDFLAGS= -lpng -lm

OCR_SRC_FILE=$(wildcard ocr/src/*.c) $(wildcard ocr/src/*/*.c)
OCR_SRC=$(filter-out ocr/src/main.c, ${OCR_SRC_FILE})
OCR_OBJ=$(OCR_SRC:.c=.o)

all: ${OBJ}
	${MAKE} -C ocr/

	${CC} ${CFLAGS} ${OBJ} ${OCR_OBJ} -o ${EXE} ${LDFLAGS}

clean:
	${RM} ${OBJ} ${EXE} ${OCR_OBJ}
