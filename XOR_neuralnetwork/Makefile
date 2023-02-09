# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3
LDFLAGS = 
LDLIBS = -lm

all: neural_network_xor

SRC = neural_network_xor
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

prog: prog.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
