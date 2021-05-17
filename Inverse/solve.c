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


int solve(int n, double* matr, double* reverse, int* index, double* answer)
{
	int max_index;
	double main_elem, keff;
	
	
	
	for (int i=0; i<n; i++)
	{
		max_index = index_max(matr, n, i);
		
	//    проверка на вырожденность
		if ( fabs(matr[i*n + max_index] ) < 1e-15 )
		{			
			return -1;
		} 
	
		swap_matrix(matr, n, i, max_index);
		swap_index(index, i, max_index);
		
		main_elem = matr[i*n+i];
		for (int j=i; j<n; j++)
		{
			matr[i*n+j] = matr[i*n+j]/main_elem;
		}
		
		for (int j=0; j<n; j++)
		{
			reverse[i*n+j] = reverse[i*n+j]/main_elem;
		}
		
		for (int j=0; j<n; j++)
		{	
			if (j!=i)							//чтоб не вычесть строку саму из себя 
			{	
				keff = matr[j*n+i];				//зануляющий кэф
				for (int k=0; k<n; k++)
				{
					reverse[j*n+k] = reverse[j*n+k] - reverse[i*n+k]*keff;
					
					if (k>=i)
					{
						matr[j*n+k] = matr[j*n+k] - matr[i*n+k]*keff; 
					} 
				}
			}
		
		}
		
	}
	
	
	
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			answer[index[i]*n+j] = reverse[i*n+j];
		}
	
	}
	
	
	
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			reverse[i*n+j] = answer[i*n+j];
		}
	
	}
	
		
	return 1;	
}
