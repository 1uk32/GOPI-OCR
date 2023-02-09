# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -g `pkg-config --cflags sdl2 sdl SDL2_image SDL_gfx`
LDFLAGS = -lm
LDLIBS = `pkg-config --libs sdl2 sdl SDL2_image SDL_gfx `

all: image_manipulation in_grayscale

SRC = image_manipulation.c detect_angle.c in_grayscale.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

prog: image_manipulation.o
grayscale: in_grayscale.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}
	rm -f image_manipulation.o
	rm -f in_grayscale.o
	${RM} *.png

# END
