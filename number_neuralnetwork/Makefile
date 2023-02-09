# Makefile

CC = gcc -fsanitize=address
CPPFLAGS =
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = 
LDLIBS = -lm

SRC = main_neural.c matrice.c neural_network.c
OBJ = ${SRC:%.c=%.o}
EXE = main_neural

default: all

all : ${EXE}

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o  $(EXE) $(LDLIBS)

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
