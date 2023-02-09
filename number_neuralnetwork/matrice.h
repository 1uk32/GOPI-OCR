#pragma once
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DOUBLE_STR_LEN 100
#define DOUBLE_PRECISION 14
#define MAX_PARSING_COLUMNS 1000
#define SEPARATOR ';'
#define LINE_SEPARATOR '\n'

typedef struct matrice {
    int rows;
    int columns;
    double *data;
} matrice;

double doubleabs(double x);
char *serialize_double(double f);
matrice *matrice_new(int rows, int columns);
void matrice_free(matrice *m);
int count_chars(char *str, char c);
double *matrice_get_ref(matrice *m, int row, int column);
double matrice_get(matrice *m, int row, int column);
void matrice_set(matrice *m, int row, int column, double value);
struct matrice* matrice_get_row(matrice* inputs, int i);
struct matrice* matrice_get_column(matrice* inputs, int i);
matrice *matrice_from_string(char *str);
double random_double(double min, double max);
matrice *matrice_random(int rows, int columns, double min, double max);
matrice *matrice_zeros(int rows, int columns);
int matrice_equals(matrice *m1, matrice *m2);
int matrice_equals_epsilon(matrice *m1, matrice *m2, double epsilon);
matrice *matrice_dot(matrice *m1, matrice *m2);
matrice *matrice_clone(matrice *m);
matrice *matrice_invert(matrice *m);
matrice *matrice_elementwise_inner(matrice *m1, matrice *m2, double (*f)(double, double), const char *function_name);
matrice *matrice_elementwise(matrice *m1, matrice *m2,double (*f)(double, double));
double add(double a, double b);
double sub(double a, double b);
double mul(double a, double b);
matrice *matrice_transpose(matrice *m);
matrice *matrice_add(matrice *m1, matrice *m2);
matrice *matrice_sub(matrice *m1, matrice *m2);
matrice *matrice_mul(matrice *m1, matrice *m2);
matrice *matrice_map(matrice *m, double (*f)(double));
void matrice_add_inplace(matrice *dest, matrice *source);
void matrice_sub_inplace(matrice *dest, matrice *source);
matrice *matrice_multiply(matrice *m, double scalar);
double *matrice_max(matrice *m, int *row, int *column);
double matrice_sum(matrice *m);
double matrice_mean(matrice *m);
double matrice_std(matrice *m);
char *matrice_serialize(matrice *m, char *name);
matrice *matrice_deserialize(char *str, char **endptr);