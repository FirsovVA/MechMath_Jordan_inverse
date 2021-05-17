#include <mpi.h>
#include "stdio.h"
#include <stdlib.h>
#include <math.h>
extern int error;

int Input_matr(int n, double* matr, int k, FILE* input, int my_rank,  int total_threads, int fr, int lr);
double create_element(int k, int n, int i, int j);

void output (int n, double* matr, int m, int total_threads, int my_rank, int fr, int lr);
int max (int i, int j);
int min(int m, int lr);



void mult_matrix(double* matr, double* reverse, double* mult, int n);
double norm (double* matr, double* reverse, int n);
