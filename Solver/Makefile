# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra `pkg-config --cflags sdl2 SDL2_image SDL2_ttf` -g
LDFLAGS = -lm
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf`

all: save_grid

SRC = save_grid.c solve.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

savegrid: save_grid.o solver.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} save_grid
	${RM} *.bmp
# END
