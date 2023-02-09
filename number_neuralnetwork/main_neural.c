#include <string.h>
#include "neural_network.h"
#include "mnist.h"
#include "matrice.h"

int main (int argc, char **argv) {
    
    load_mnist();

    if (argc != 2){
        errx(1,"Need argument at execution");
        return 1;
    }
    
    if (strcmp(argv[1], "train") == 0){
        
        Network* network = (Network*)malloc(sizeof(Network));
        init_network(network);
        int trainingSetOrder[NUMTRAININGSETS];
        for(int c=0;c<NUMTRAININGSETS;c++)
            trainingSetOrder[c] = c;
        struct matrice* images = get_images();
        struct matrice* labels = get_labels();
        for(size_t n = 0; n<EPOCHS;n++)
            network_training(*images, *labels, network, trainingSetOrder);
        
        matrice_free(images);
        matrice_free(labels);
        save_neural_network(network, NN_SAVE_FILENAME);
        free_network(network);
    }

    else if (strcmp(argv[1], "test") == 0){
    
        Network* network = load_neural_network(NN_SAVE_FILENAME);
        struct matrice* images = get_test_images();
        struct matrice* labels = get_test_labels();
        network_testing(images, labels, network);
        matrice_free(images);
        matrice_free(labels);
        free_network(network);
    }
    
    else if (strcmp(argv[1], "debug") == 0){
        //Network* network = load_neural_network(NN_SAVE_FILENAME);
        Network* network = (Network*)malloc(sizeof(Network));
        init_network(network);
        struct matrice* image = matrice_new(784,1);
        struct matrice* label = matrice_new(1,1);
        for (int i=0; i<784; i++)
            matrice_set(image, i, 0, (double)train_image[0][i]);
        matrice_set(label, 0, 0, (double)train_label[0]);
        matrice *result = forwardprop(network, image);
        int max = 0;
        for (int j=0; j<10; j++) {
            if (result->data[j] > result->data[max]) {
                max = j;
            }
        }
        matrice_free(result);
        matrice_free(image);
        matrice_free(label);
        free_network(network);
    }
    
    else{
        printf("%s\n",argv[1]);
        errx(1,"Only train and test arguments are valid");
        return 1;
    }
    return 0;
}
