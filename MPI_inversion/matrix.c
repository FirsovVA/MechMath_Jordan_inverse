#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

int min(int m, int lr)
{
	if (lr <= m)
	{
		return lr;
	}
	else
		return m;

}

void output (int n, double* matr, int m, int total_threads, int my_rank, int fr, int lr)
{
	for (int i = 0; i < total_threads; i++)
	{
		//printf("CHECK 1\n");
		if(my_rank == i)
		{
			MPI_Barrier(MPI_COMM_WORLD);
			//printf("CHECK 2\n");
			if(fr <= m)
			{
				//printf("CHECK 3\n");
				for (int j = 0; j < ( min(m, lr) - fr); j++)
				{
				//	printf("CHECK 4\n");
					for (int k = 0; k < m; k++)
					{
						printf("%10.3e ", matr[j * n + k]);
					}
					printf("\n");
				}
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	//MPI_Barrier(MPI_COMM_WORLD);

}

void mult_matrix(double* matr, double* reverse, double* mult, int n)
{
	for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            mult[i*n + j] = 0;
            for (int k = 0; k < n; k++)
			{
                mult[i*n + j] += matr[i*n + k] * reverse[k*n + j];
			}
        }
    }
}





double norm (double* matr, double* reverse, int n)
{
	double* mult = NULL;
	double sum;
	double max = 0;


	mult=(double*)malloc(n*n*sizeof(double));
	

	mult_matrix(matr, reverse, mult, n);

	
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			if(i ==j)
				mult[i*n+j] -= 1;
		}
	}

	for (int i=0; i<n; i++)
	{	
		sum = 0;
		for (int j=0; j<n; j++)
		{
			sum += fabs(mult[i*n+j]);	
		}
		if (sum > max)
		{
			max = sum;
		}
	}
	
	if(mult)
	{
		free(mult);
	}

return max;	
}