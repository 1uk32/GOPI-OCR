#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "neural_network.h"
#include "matrice.h"

matrice* imagetomatrice(SDL_Surface *image) {
    matrice *m = matrice_new(784, 1);
    Uint32* pixelos = (Uint32*)image -> pixels;
    Uint8 r, g, b;
    for (int i = 0; i < 784; i++) {
        SDL_GetRGB(pixelos[i], image->format, &r, &g, &b);
        if (r == 255){
            *(m->data + i) = 1;
        } else {
            *(m->data + i) = 0;
        }
    }
    return m;
}

int** imagetosudoku(SDL_Surface **images) {
    int** sudoku = NULL;
    Network* network = load_neural_network("savedvalues.txt");
    for (int j = 0; j < 9; j++) {
        for (int i = 0; i < 9; i++) {
            matrice *m = imagetomatrice(images[i + j * 9]);
            matrice* output = forwardprop(network, m);
            int max = 0;
            for (int k=0; k<10; k++) {
                if (output->data[k] > output->data[max]) {
                     max = k;
                }
            }
            sudoku[i][j] = max;
        }
    }
    return sudoku;
}
