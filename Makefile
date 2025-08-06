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

all: solver reverse

solver: ${OBJ} ${OBJ_SOLVER}
	${CC} -O3 ${OBJ} ${OBJ_SOLVER} -o ${EXE_SOLVER}

reverse: ${OBJ} ${OBJ_REVERSE}
	${CC} -O3 ${OBJ} ${OBJ_REVERSE} -o ${EXE_REVERSE}

clean:
	${RM} ${OBJ_SOLVER} ${OBJ_REVERSE}
	${RM} ${EXE_REVERSE} ${EXE_SOLVER}
