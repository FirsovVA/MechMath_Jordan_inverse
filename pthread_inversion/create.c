#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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


void create_matrix_file(double* matr, int n, const char* argv)
{
	double a;
	FILE* inp;
	int x = 0;

		inp = fopen(argv, "r");
		for(int i=0;i<n*n;i++)
		{	
			x = fscanf(inp,"%lf",&a);
			matr[i + x*0]=a;
		}
		fclose(inp);
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
