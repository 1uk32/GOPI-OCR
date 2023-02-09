#include "neural_network.h"
#include "matrice.h"

double sigmoid(double x) {return 1.0 / (1.0 + exp(-x));}
double dSigmoid(double x) {return x * (1 - x);}
double init_weight() { return (double)rand() / RAND_MAX * 2.0 - 1.0; }
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void write_to_file(char *filename, char *data) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        errx(1, "write_to_file: Could not open file %s\n", filename);
    }

    fprintf(f, "%s", data);
    fclose(f);
}

char *read_from_file(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        errx(1, "read_from_file: Could not open file %s\n", filename);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = malloc(fsize + 1);
    if(fread(data, fsize, 1, f) != 1) {
        errx(1, "read_from_file: Could not read file %s\n", filename);
    }
    fclose(f);

    data[fsize] = '\0';

    return data;
}

struct matrice* mse_delta(struct matrice *z, struct matrice *a, struct matrice *y){
    struct matrice *sig = matrice_clone(z);
    matrice_map(sig, dSigmoid);
    struct matrice *sub = matrice_sub(a, y);
    struct matrice *res = matrice_mul(sub, sig);
    matrice_free(sub);
    matrice_free(sig);
    return res;
}

matrice* cross_entropy(matrice *a, matrice *y){
    (void)a;
    (void)y;
    return NULL;
}

struct matrice* cross_entropy_delta(struct matrice* z , struct matrice *a, struct matrice *y){
    (void) z;
    return matrice_sub(a, y);
}

Layer *create_layer(int nb_neurons, int nb_inputs) {
    Layer *layer = malloc(sizeof(Layer));
    layer->weights = matrice_random(nb_neurons, nb_inputs, -1, 1);
    layer->biases = matrice_random(nb_neurons, 1, -1, 1);

    // improved weights initialization:
    // http://neuralnetworksanddeeplearning.com/chap3.html#weight_initialization
    matrice_multiply(layer->weights, 1 / sqrt(nb_inputs));

    return layer;
}

#define mse_cost (CostFunction){NULL, mse_delta, 1}
#define cross_entropy_cost (CostFunction){cross_entropy, cross_entropy_delta, 0}
#define COST_FUNCTIONS (CostFunction[]){cross_entropy_cost, mse_cost}

void init_network(struct Network* network) {
    network->nb_layers = 2;
    network->layers = malloc(sizeof(Layer*) * network->nb_layers);
    network->layers[0] = create_layer(NUMHIDDENNODES, NUMINPUTS);
    network->layers[1] = create_layer(NUMOUTPUTS, NUMHIDDENNODES);
    network->cost_function = cross_entropy_cost;
}

void free_network(Network* network) {
    matrice_free(network->layers[1]->weights);
    matrice_free(network->layers[1]->biases);
    matrice_free(network->layers[0]->weights);
    matrice_free(network->layers[0]->biases);
    free(network->layers[1]);
    free(network->layers[0]);
    free(network->layers);
    free(network);
}

void update_mini_batch(struct Network *nn, 
        struct matrice*inputs, struct matrice*targets, double learning_rate,
        int start, int end) {
    struct matrice **nabla_b = malloc(nn->nb_layers * sizeof(struct matrice *));
    struct matrice **nabla_w = malloc(nn->nb_layers * sizeof(struct matrice *));

    for (int i = 0; i < nn->nb_layers; i++) {
        Layer *layer = nn->layers[i];
        nabla_b[i] = matrice_zeros(layer->biases->rows, layer->biases->columns);
        nabla_w[i] = matrice_zeros(layer->weights->rows, layer->weights->columns);
    }
    for (int i = start; i < end; i++) {
        matrice *input = matrice_get_column(inputs, i);
        matrice *target = matrice_get_column(targets, i);
        backprop(nn, input, target, nabla_b, nabla_w);
    }

    // update NeuralNetwork weights
    for (int i = 0; i < nn->nb_layers; i++) {
        matrice_sub_inplace(nn->layers[i]->weights, 
                matrice_multiply(nabla_w[i], (double)learning_rate));

        matrice_sub_inplace(nn->layers[i]->biases,
                matrice_multiply(nabla_b[i], (double)learning_rate));

        matrice_free(nabla_b[i]);
        matrice_free(nabla_w[i]);
    }

    free(nabla_b);
    free(nabla_w);
}


struct matrice* forwardprop(Network* nn, struct matrice *input) {
    struct matrice *output = input;
    struct matrice *old_output;
    for (int i = 0; i < nn->nb_layers; i++) {
        old_output = output;
        struct matrice *dot = matrice_dot(nn->layers[i]->weights, output);
        output = matrice_add(dot, nn->layers[i]->biases);   // output = weights * output + biases
        matrice_map(output, sigmoid); // output = sigmoid(output)

        matrice_free(dot);
        if (i > 0) {
            matrice_free(old_output);
        }
    }
    return output;
}

void backprop(struct Network *nn, 
        struct matrice *input, 
        struct matrice *target,
        struct  matrice **nabla_b, 
        struct matrice **nabla_w) {
    // feedforward
    struct matrice *output = input;
    struct matrice **activations = malloc((nn->nb_layers + 1) * sizeof(struct matrice *));
    activations[0] = input; // list to store all the activations, layer by layer

    struct matrice **zs = malloc(nn->nb_layers * sizeof(struct matrice *)); // list to store all the z vectors, layer by layer
    struct matrice *dot;
    struct matrice *sig;
    struct matrice *delta;
    struct matrice *transpose;

    for (int i = 0; i < nn->nb_layers; i++) {
        output = matrice_dot(nn->layers[i]->weights, output);
        matrice_add_inplace(output, nn->layers[i]->biases);

        zs[i] = matrice_clone(output);
        matrice_map(output, sigmoid);
        activations[i + 1] = output;
    }

    // backpropagation
   
    delta = nn->cost_function.delta(zs[nn->nb_layers - 1], activations[nn->nb_layers], target);

    transpose = matrice_transpose(activations[nn->nb_layers - 1]);
    dot = matrice_dot(delta, transpose);

    matrice_add_inplace(nabla_b[nn->nb_layers - 1], delta);
    matrice_add_inplace(nabla_w[nn->nb_layers - 1], dot);

    matrice_free(transpose);
    matrice_free(dot);

    for (int i = nn->nb_layers - 2; i >= 0; i--) {
        sig = matrice_clone(zs[i]);
        matrice_map(sig, dSigmoid);
        transpose = matrice_transpose(nn->layers[i + 1]->weights);
        dot = matrice_dot(transpose, delta);

        matrice_free(delta);
        matrice_free(transpose);

        delta = matrice_mul(dot, sig);
        matrice_free(dot);

        transpose = matrice_transpose(activations[i]);
        dot = matrice_dot(delta, transpose);
        matrice_add_inplace(nabla_b[i], delta);
        matrice_add_inplace(nabla_w[i], dot);

        matrice_free(sig);
        matrice_free(dot);
        matrice_free(transpose);
    }

    matrice_free(delta);

    for (int i = 0; i < nn->nb_layers; i++) {
        matrice_free(zs[i]);
    }

    for (int i = 1; i < nn->nb_layers + 1; i++) {
        matrice_free(activations[i]);
    }

    free(zs);
    free(activations);
}


void network_training(struct matrice input, struct matrice target, struct Network* network, int* trainingSetOrder){
    shuffle(trainingSetOrder,NUMTRAININGSETS);
    size_t start = 0;
    size_t end = 200;
    while(start!=NUMTRAININGSETS){
        update_mini_batch(network, &input, &target, LR, start, end);
        start = end;
        end = end + 200;
        if(end>NUMTRAININGSETS){
            end = NUMTRAININGSETS;
        }
    }
}   

void network_testing(struct matrice *inputs, struct matrice *target, struct Network* network){
    int correct = 0;
    for (int i=0; i<NUMTESTINGSETS; i++) {
        struct matrice *input = matrice_get_column(inputs, i);
        struct matrice *output = forwardprop(network, input);
        int max = 0; 
        for (int j=0; j<10; j++) {
            if (output->data[j] > output->data[max]) {
                max = j;
            }
        }
        if (max == target->data[i])
            correct++;
        printf("Expected:%f Got:%d Results: %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg \n",\
                target->data[i], max, output->data[0],output->data[1],output->data[2],output->data[3],\
                output->data[4],output->data[5],output->data[6],output->data[7],output->data[8],output->data[9]);
        matrice_free(input);
        matrice_free(output);
    }
    printf("Accuracy: %f \n", (double)correct/NUMTESTINGSETS);
}

void save_neural_network(Network *nn, char *filename) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        errx(1, "save_neural_network: Could not open file %s\n", filename);
    }

    fprintf(f, "%i\n", nn->nb_layers);
    fprintf(f, "%i\n", nn->cost_function.id);

    for (int i = 0; i < nn->nb_layers; i++){
        char *serialized_weights = matrice_serialize(nn->layers[i]->weights, NULL);
        fprintf(f, "# Weights of layers[%i] :\n{\n%s}\n", i, serialized_weights);

        char *serialized_biases = matrice_serialize(nn->layers[i]->biases, NULL);
        fprintf(f, "# Biases of layers[%i] :\n{\n%s}\n", i, serialized_biases);

        free(serialized_weights);
        free(serialized_biases);
    }

    fclose(f);
}

Network *load_neural_network(char *filename) {
    char *content = read_from_file(filename);

    int nb_layers;
    int cost_function_id;
    sscanf(content, "%i\n%i\n", &nb_layers, &cost_function_id);

    Network *nn = malloc(sizeof(Network));

    Layer **layers = malloc(sizeof(Layer *) * nb_layers);

    char *ptr;
    char *end = content;
    for (int i = 0; i < nb_layers; i++) {
        // find next {} block
	ptr = strstr(end, "{") + 2;

        // Deserialize weights
	layers[i] = malloc(sizeof(Layer));
    matrice *weights = matrice_deserialize(ptr, &end);
    layers[i]->weights = matrice_clone(weights);
    matrice_free(weights);

        // Deserialize biases
    ptr = strstr(end, "{") + 2;
	matrice *biases = matrice_deserialize(ptr, &end);
    layers[i]->biases = matrice_clone(biases);
    matrice_free(biases);
    }
    nn->nb_layers = nb_layers;
    nn->cost_function = COST_FUNCTIONS[cost_function_id];
    nn->layers = layers;

    free(content);
    
    return nn;
}