#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithm.h"



int index_max(double* matr, int n, int k)
{	
	double max =fabs( matr[k*n+k] );
	int max_index = 0;

	for (int i=k; i<n; i++)
	{
		if (fabs(matr[k*n+i])>=max)
		{
			max = fabs(matr[k*n+i]);
			max_index = i;
		}
	}
return max_index;
}

void swap_matrix (double* matr, int n, int i, int j)
{
	double a;
	
	for(int k=0; k<n; k++)
	{
		a = matr[k*n+i];
		matr[k*n+i] = matr[k*n+j];
		matr[k*n+j] = a;
	}
}

void swap_index (int* index, int i, int j)
{
	double a;
	a = index[i];
	index[i] = index[j];
	index[j] = a;
}



double norm_matr(double* matr, int n)
{

	double sum, max;
	max = 0;
	for (int i=0; i<n; i++)
	{	
		sum = 0;
		for (int j=0; j<n; j++)
		{
			sum = sum + fabs(matr[i*n+j]);	
		}
	
		if (sum > max)
		{
			max = sum;
		}
	}

	return max;
}





void synchronize(int total_threads)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
	static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
	static int threads_in = 0;
	static int threads_out = 0;

	pthread_mutex_lock(&mutex);

	threads_in++;
	if (threads_in >= total_threads)
	{
		threads_out = 0;
		pthread_cond_broadcast(&condvar_in);
	} else
		while (threads_in < total_threads)
			pthread_cond_wait(&condvar_in,&mutex);

	threads_out++;
	if (threads_out >= total_threads)
	{
		threads_in = 0;
		pthread_cond_broadcast(&condvar_out);
	} else
		while (threads_out < total_threads)
			pthread_cond_wait(&condvar_out,&mutex);

	pthread_mutex_unlock(&mutex);
}



int InvMatrix(int n, double* matr, double* reverse, int* index, int my_rank, int total_threads, double* main_str)
{
	
	int i, j, k;
	int res = 1;
	int max_index;
	int first_row;
	int last_row;
	double main_elem, tmp, norm_check = 0;
	
	
	if (my_rank == 0)
	{

		norm_check = norm_matr(matr, n);
		
		for (i = 0; i < n; i++)
			index[i] = i;

		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				reverse[i * n + j] = (double)(i == j);
	}
	


	for (i=0; i<n; i++)
	{
		
		if (my_rank == 0)
		{
			max_index = index_max(matr, n, i);
			
		//    проверка на вырожденность
			if ( fabs(matr[i*n + max_index] ) < 1e-15 * norm_check )
			{			
				res = -1;
			} 

			if (res > 0)
			{
				swap_matrix(matr, n, i, max_index);
				swap_index(index, i, max_index);
				
				
				main_elem = 1/matr[i*n+i];
				for (j=i; j<n; j++)
				{
					matr[i*n+j] *= main_elem;
				}
				
				for (j=0; j<n; j++)
				{
					reverse[i*n+j] *= main_elem;
				}
			}
		}	
		
		
		synchronize(total_threads);
	
		if (res < 0)
		{
			return -1;
		}


		first_row = i * my_rank;
		first_row = first_row/total_threads;
		last_row = i * (my_rank + 1);
		last_row = last_row/total_threads;
		
		for (int m = 0; m < n; m++)
		{
			main_str[m] = matr[i*n + m];
		}
		

		for (j = first_row; j < last_row; j++)
		{
			tmp = matr[j * n + i];
			for (k = i; k < n; k++)
				matr[j * n + k] -= tmp * main_str[k];
			for (k = 0; k < n; k++)
				reverse[j * n + k] -= tmp * reverse[i * n + k];
		}
		synchronize(total_threads);
		
		first_row = (n - i - 1) * my_rank;
		first_row = first_row/total_threads + i + 1;
		last_row = (n - i - 1) * (my_rank + 1);
		last_row = last_row/total_threads + i + 1;
		//printf("2: From %d to %d : for %d \n", first_row, last_row, my_rank);

		for (int m = 0; m < n; m++)
		{
			main_str[m] = matr[i*n + m];
		}

		for (j = first_row; j < last_row; j++)
		{
			tmp = matr[j * n + i];
			for (k = i; k < n; k++)
				matr[j * n + k] -= tmp * main_str[k];
			for (k = 0; k < n; k++)
				reverse[j * n + k] -= tmp * reverse[i * n + k];
		}
		synchronize(total_threads);
	}


	if (my_rank == 0)
	{
		for (int i=0; i<n; i++)
		{
			for (int j=0; j<n; j++)
			{
				matr[index[i]*n+j] = reverse[i*n+j];
			}
		
		}
		
		
		
		for (int i=0; i<n; i++)
		{
			for (int j=0; j<n; j++)
			{
				reverse[i*n+j] = matr[i*n+j];
			}
		
		}
	}

	synchronize(total_threads);	
		
	return 1;	
}
