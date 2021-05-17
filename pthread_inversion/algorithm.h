
int index_max(double* matr, int n, int k);
void swap_matrix (double* matr,int n, int i, int j);
void swap_index (int* index, int i, int j);
void synchronize(int total_threads);
int InvMatrix(int n, double* matr, double* reverse, int* index, int my_rank, int total_threads, double* main_str);
void *Inversion(void *p_arg);

double norm_matr(double* matr, int n);