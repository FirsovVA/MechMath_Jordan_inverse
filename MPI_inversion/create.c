#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "matrix.h"

int max (int i, int j)
{
	if (i > j)
	{
		return i;
	}
	else
	{
		return j;
	}
}


int Input_matr(int n, double* matr, int k, FILE* input, int my_rank,  int total_threads, int fr, int lr){
	int i, j, p, res = 1;
	int row_count, cur;
	double a;
	
	row_count = n / total_threads;
	cur = 0;

	if(k == 0)
	{
		for(i = 0; i < n; i++)
		{
			for(p = 0; p < total_threads; p++)
			{
				if(i == p * row_count )
				{
					cur = p;
					p = total_threads;
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
			
			for(j = 0; j < n; j++)
			{
				if(my_rank == 0)
				{
					res = fscanf(input, "%lf", &a);
					if(res != -1)
					{
						if(cur == 0)
						{
							matr[(i - fr)*n + j] = a;
						} 
						else 
						{
							MPI_Send( &a, 1, MPI_DOUBLE, cur, 0, MPI_COMM_WORLD ); 
						}
					}
				}
				
				if(cur != 0 && my_rank == cur)
				{
					MPI_Recv(&a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					matr[(i - fr) * n + j ] = a;
				}
				if(res == -1){
					if(my_rank == 0){
						if(res == EOF)
						{
							printf("Not enough elements\n");
						}
						else 
						{
							printf("Wrong type of element\n");
						}
					}
					return -1;
				}
			}
		}
	} 
	else 
	{
		for(i = 0; i < n; i++)
		{
			if( i >= fr && i< lr )
			{	
				for(j = 0; j < n; j++)
				{			
					//printf("I am %d - here in %d_%d \n", my_rank, i, j);			
					fr = my_rank * row_count;
					matr[(i - fr) * n + j] = create_element(k, n, i + 1, j + 1);
				}
			}
		}
	}
	return 1;
}	

double create_element(int k, int n, int i, int j)
{

	if (k==1)
	{
		return n-max(i, j) + 1;
	}	
	
	if (k==2)
	{
			
		return max(i, j);	
	}
	
	if (k==3)
	{
		return abs(i - j);
	}
	else
	{
		return (double) 1/(i+j-1) ;
	}	
}
