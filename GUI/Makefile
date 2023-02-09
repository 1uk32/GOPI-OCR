# Makefile

GUI: OCR_GUI.c
	gcc `pkg-config --cflags gtk+-3.0 sdl2 sdl SDL2_image SDL_gfx` -Wall -O3    OCR_GUI.c  `pkg-config --libs gtk+-3.0 sdl2 sdl SDL2_image SDL_gfx ` -o OCR_GUI -rdynamic

clean:
	rm -f OCR_GUI
	rm -f OCR_GUI.o

# END
