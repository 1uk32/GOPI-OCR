#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrice.h"

#define NUMINPUTS 784
#define NUMHIDDENNODES 128
#define NUMOUTPUTS 10
#define NUMTRAININGSETS 60000
#define NUMTESTINGSETS 10000
#define EPOCHS 10
#define LR 10

#ifndef NN_SAVE_FILENAME
    #define NN_SAVE_FILENAME "savedvalues.txt"
#endif

typedef struct CostFunction{
    // z: activation
    // a: output
    // y: target
    struct matrice* (*fn)(struct matrice* /*a*/, struct matrice* /*y*/);
    struct matrice* (*delta)(struct matrice* /*z*/, struct matrice* /*a*/, struct matrice* /*y*/);
    int id; // used for serialization and deserialization of the network
} CostFunction;

typedef struct Layer {
    struct matrice *weights;
    struct matrice *biases;
} Layer;

typedef struct Network {
    int nb_layers;
    struct Layer **layers;
    struct CostFunction cost_function;
} Network;

double sigmoid(double x);
double dSigmoid(double x);
double init_weight();
void shuffle(int *array, size_t n);
void write_to_file(char *filename, char *data);
char *read_from_file(char *filename);
struct matrice* mse_delta(matrice *z, matrice *a, matrice *y);
struct matrice* cross_entropy(matrice *a, matrice *y);
struct matrice* cross_entropy_delta(matrice* z , matrice *a, matrice *y);
Layer *create_layer(int nb_neurons, int nb_inputs);
void init_network(Network* network);
void free_network(Network* network);
void update_mini_batch(Network *nn, struct matrice*inputs, struct matrice*targets, double learning_rate, int start, int end);
struct matrice* forwardprop(Network* network, struct matrice* input);
void backprop(struct Network *nn, struct matrice *input, struct matrice *target,struct  matrice **nabla_b, struct matrice **nabla_w);
void network_training(struct matrice input, struct matrice target, Network* network, int* trainingSetOrder);
void network_testing(struct matrice* input, struct matrice* target, Network* network);
void save_neural_network(Network *nn, char *filename);
Network *load_neural_network(char *filename);
#endif
