#include "neural_network.c"
#include "in_grayscale.c"

int main(){
    Network* network = load_neural_network("savedvalues.txt");
    SDL_Surface** images = malloc(9*sizeof(SDL_Surface*));
    for (int i = 1; i<10; i++){
        char* filename = malloc(20);
        sprintf(filename, "example_numbers/%d.png", i);
        SDL_Surface* image = load_image(filename);
        SDL_Surface* image_grayscale = in_grayscale(image);
        SDL_FreeSurface(image);
        images[i-1] = image_grayscale;
        matrice *m = imagetomatrice(image_grayscale);
        matrice *output = forwardprop(network, m);
        int max = 0;
        for (int j=0; j<10; j++) {
            if (output->data[j] > output->data[max]) {
                max = j;
            }
        }
        if (max == i){
            printf("Number %d recognized correctly\n", i);
        } else {
            printf("Number %d recognized as %d\n", i, max);
        }
    }
    return 0;
}
