#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"


void output (int n, int m, double* matr)
{
	for (int i=0; i<m; i++)
	{
		for (int j=0; j<m; j++)
		{
			printf(" %10.3e", matr[i*n + j]);
		}
	printf("\n");		
	}
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


double norma (double* matr, double* reverse, int n)
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
			sum = sum + fabs(mult[i*n+j]);	
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



//Вычисление процессорного времени 
long int get_time(void)
{
	struct rusage buf;

	getrusage(RUSAGE_SELF, &buf);

	return buf.ru_utime.tv_sec * 100 + buf.ru_utime.tv_usec/10000;
}

//Вычисление астрономического времени
long int get_full_time(void)
{
	struct timeval buf;

	gettimeofday(&buf, 0);

	return buf.tv_sec * 100 + buf.tv_usec/10000;
}