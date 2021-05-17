#include <mpi.h>
#include "stdio.h"
#include <stdlib.h>
#include <math.h>

int Index_max(double* matr, int n, int k);
void swap_matrix (double* matr, int n, int i, int j, int column_len);
void swap_index (int* index, int i, int j);
int InvMatrix(int n, double* matr, double* reverse, int* index, int my_rank, int fr, int lr, int column_len, int total_threads, double* main_str, double* main_reverse_str);


double matrix_norm(int n, double* matr, int my_rank, int fr, int lr, int total_threads);
