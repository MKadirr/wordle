CC=gcc
INCLUDE=include

SRC=utils.c data.c
OBJ=${SRC:.c=.o}

SRC_SOLVER=solver.c
OBJ_SOLVER=${SRC_SOLVER:.c=.o}
EXE_SOLVER=solver.out

SRC_REVERSE=reverse.c
OBJ_REVERSE=${SRC_REVERSE:.c=.o}
EXE_REVERSE=reverse.out

CFLAGS=-Ofast

all: solver reverse

solver: ${OBJ} ${OBJ_SOLVER}
	${CC} ${CFLAGS} ${OBJ} ${OBJ_SOLVER} -o ${EXE_SOLVER}

reverse: ${OBJ} ${OBJ_REVERSE}
	${CC} ${CFLAGS} ${OBJ} ${OBJ_REVERSE} -o ${EXE_REVERSE}

clean:
	${RM} ${OBJ_SOLVER} ${OBJ_REVERSE} ${OBJ}
	${RM} ${EXE_REVERSE} ${EXE_SOLVER}
