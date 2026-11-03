CC=gcc
INCLUDE=-Iocr/src

SRC=$(wildcard src/*.c)
OBJ=${SRC:.c=.o}
EXE=solver.out

CFLAGS_BASE=-lm $(INCLUDE) -mavx2

# CFLAGS=$(CFLAGS_BASE) -g -O0 -fsanitize=address    # debug
# CFLAGS=$(CFLAGS_BASE) -O3 -mtune=native           # release
CFLAGS=$(CFLAGS_BASE) -O3 -mtune=native -g3       # perf
# CFLAGS=$(CFLAGS_BASE) -O3 -g3                     # perf

# LDFLAGS=-fsanitize=address -lpng                  # ASAN
LDFLAGS= -lpng -lm                                  # release

OCR_SRC_FILE=$(wildcard ocr/src/*.c) $(wildcard ocr/src/*/*.c)
OCR_SRC=$(filter-out ocr/src/main.c, ${OCR_SRC_FILE})
OCR_OBJ=$(OCR_SRC:.c=.o)

all: ${OBJ}
	${MAKE} -C ocr/

	${CC} ${CFLAGS} ${OBJ} ${OCR_OBJ} -o ${EXE} ${LDFLAGS}

clean:
	${RM} ${OBJ} ${EXE} ${OCR_OBJ}
