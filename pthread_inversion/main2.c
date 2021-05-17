#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/sysinfo.h> 
#include <unistd.h>     


#include "matrix.h"
#include "algorithm.h"



typedef struct
{
	int n;
	double *matr;
	double *reverse;
	int *index;
	double *main_str;
	int my_rank;
	int total_threads;
	bool check;
	long int time;
} ARGS;



long int thread_time = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *Inversion(void *p_arg)
{
	ARGS *arg = (ARGS*)p_arg;
	long int t1;

	synchronize(arg->total_threads);
	
	t1 = get_full_time();
	if(	InvMatrix(arg->n, arg->matr, arg->reverse, arg->index, arg->my_rank, arg->total_threads, arg->main_str) == -1)
	{
		arg->check = false;
		return NULL;
	}

	synchronize(arg->total_threads);

	t1 = get_full_time() - t1;

	pthread_mutex_lock(&mutex);
	arg->time = t1;
	pthread_mutex_unlock(&mutex);

	return NULL;
}



int main (int argc, char** argv)
{
	int n, k, m, result, count=0;
	FILE* inp = NULL;
	double* matr = NULL;
	double* reverse = NULL;
	int* index = NULL;
	double a;
	struct sysinfo info;
	long int time_full;
	int total_threads;
	pthread_t *threads;
	ARGS *args;




	if (argc==6 || argc==5)
	{
		result = sscanf(argv[1], "%d", &n);
		if (result == 1)
		{
			if (result == EOF)			
			{
				printf("Error of input 1.1\n");
				return -1;
			}
		}
		else
		{
			printf("Error of input 1.2\n");
			return -1;
		}

		result = sscanf(argv[2],"%d", &m);
		if (result == 1)
		{
			if (result == EOF)			
			{
				printf("Error of input 2.1 \n");
				return -2;
			}
		}
		else
		{
			printf("Error of input 2.2 \n");
			return -2;
		}


		result = sscanf(argv[3],"%d", &total_threads);
		if (result == 1)
		{
			if (result == EOF)			
			{
				printf("Error of input 3.1\n");
				return -2;
			}
		}
		else
		{
			printf("Error of input 3.2\n");
			return -2;
		}




		result = sscanf(argv[4],"%d", &k);
		if (result == 1)
		{
			if (result == EOF)			
			{
				printf("Error of input 4.1\n");
				return -2;
			}
		}
		else
		{
			printf("Error of input 4.2\n");
			return -2;
		}

		if ( (k!=0 && argc==6) || (k==0 && argc==5)  )
		{
			printf("Error of input 4\n");
			return -3;
		}		

		if (n<=0 || m<=0 || k<0 || k>4 || m>n || total_threads < 1)
		{
				printf("Error of input \n");
				return -3;
		}
	}
	else
	{
		printf("Wrong number of arguments \n");
		return -7;
	}
	
	
	if (k==0)
	{
		inp = fopen(argv[5],"r");	
		if (inp == NULL)
		{
			printf("Can't open file \n");
			return -4;
		}

		while(fscanf(inp,"%lf",&a) == 1)
		{
			count++;
		}
		
		fclose(inp);
		
		if (count!=n*n)
		{
			printf("Error of input: wrong size of matrix !!!!!!!!!!\n");
			return -5;
		}
	}

	
    if (sysinfo(&info) != 0)
	{
        printf("sysinfo: error reading system statistics");
	}


	if ( (long long)((long long)(info.totalram-info.freeram) - (long long)8*n*n*7) < 0 )   // 7 ~ кол-во массивов 
	{
		printf("Memory troubles \n");
		return -10;
	}



	matr=(double*)malloc(n*n*sizeof(double));
	reverse=(double*)malloc(n*n*sizeof(double));
	index = (int*)malloc(n*sizeof(int));
	//main_str = (double*)malloc(n*sizeof(double));
	threads = (pthread_t*)malloc(total_threads * sizeof(pthread_t));
	args = (ARGS*)malloc(total_threads * sizeof(ARGS));
	
	
	
	
	if ( !(matr) || !(reverse) || !(index) || !(threads) || !(args) )
	{
		printf("Memory troubles  \n");
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (threads) free(threads);
		if(args) free(args);
		return -7;
	}

	
	if (k==0)
	{
		inp = fopen(argv[5],"r");	
		if (inp == NULL)
		{
			printf("Can't open file \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			if (threads) free(threads);
			if(args) free(args);
			return -4;
		}
		count = 0;

		while(fscanf(inp,"%lf",&a) == 1)
		{
			count++;
		}
		
		fclose(inp);
		
		if (count!=n*n)
		{
			printf("Error of input: wrong size of matrix \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			if (threads) free(threads);
			if(args) free(args);
			return -5;
		}
	}

	if (k==0)
	{	
		create_matrix_file(matr, n, argv[5]);
	}
	else
	{
		for (int i=0;i<n;i++)
		{
			for(int j=0;j<n;j++)
			{
				matr[i*n + j] = create_element(k, n, i+1, j+1); 
			}
		}
	}
	
	


	printf("Input matrix : \n");
	output (n, m, matr);

	for (int i = 0; i < total_threads; i++)
	{	
		
		args[i].n = n;
		args[i].matr = matr;
		args[i].reverse = reverse;
		args[i].index = index;
		args[i].my_rank = i;
		args[i].total_threads = total_threads;
		args[i].check = true;
		args[i].main_str = (double*)malloc(n*sizeof(double));
	}


	//time_full = get_full_time();

	for (int i = 0; i < total_threads; i++)
	{
		if (pthread_create(threads + i, 0, Inversion, args + i))
		{
			printf("Cannot create thread %d!\n", i);

			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -7;
		}
	}
	
	
	for (int i = 0; i < total_threads; i++)
	{

		
		if (pthread_join(threads[i], 0))
		{
			printf("Cannot wait thread %d!\n", i);

			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -8;
		}

		if (args[0].check == false)
		{
			printf("Input Matrix is degenerate \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -6;
		}
	}


	for (int i = 0; i < total_threads; i++)
	{
		if (args[i].check == false)
		{
			printf("Input Matrix is degenerate \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -6;
		}
	}


	time_full = args[0].time;
	if(time_full == 0)
	{
		time_full = 1; 
	}

	printf("Reverse matrix : \n");
	output(n, m, reverse);

	printf("Time spent while inverting = %ld\n", time_full);

	
	if (k==0)
	{
		inp = fopen(argv[5],"r");	
		if (inp == NULL)
		{
			printf("Can't open file \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -4;
		}
		count = 0;
		
		while(fscanf(inp,"%lf",&a) == 1)
		{
			count++;
		}
		
		fclose(inp);
		
		if (count!=n*n)
		{
			printf("Error of input: wrong size of matrix \n");
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
			if (threads) free(threads);
			if(args) free(args);
			return -5;
		}
	}


	if (k==0)
	{	
		create_matrix_file(matr, n, argv[5]);
	}
	else
	{
		for (int i=0;i<n;i++)
		{
			for(int j=0;j<n;j++)
			{
				matr[i*n + j] = create_element(k, n, i+1, j+1); 
			}
		}
	}


	printf("Norma = %10.3e \n", norma(matr, reverse, n));
	

	free(matr);
	free(reverse);
	free(index);
	for (int i = 0; i < total_threads; i++)
			{
				if (args[i].main_str) free(args[i].main_str);
			}
	free(threads);
	free(args);
return 1;
}
