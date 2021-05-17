#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h> 
#include <unistd.h>     


#include "matrix.h"
#include "algorithm.h"







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
	double* E = NULL;
	double* mult = NULL;
	double sum;
	double max = 0;


	E=(double*)malloc(n*n*sizeof(double));
	mult=(double*)malloc(n*n*sizeof(double));
	
	for (int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			if (j==i)
			{
				E[i*n+j] = 1;
			}
			else
			E[i*n+j] = 0;
		}
	}

	mult_matrix(matr, reverse, mult, n);

	
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			mult[i*n+j] = mult[i*n+j] - E[i*n+j];
		}
	}

	free(E);

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


int main (int argc, char** argv)
{
	int n,k,m, result, res, count=0;
	FILE* inp;
	double* matr = NULL;
	double* matr_v2 = NULL;
	double* reverse = NULL;
	double* answer = NULL;
	int* index = NULL;
	double a;
	clock_t start, stop;
	struct sysinfo info;




	if (argc==5 || argc==4)
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

		result = sscanf(argv[3],"%d", &k);
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

		if ( (k!=0 && argc==5) || (k==0 && argc==4)  )
		{
			printf("Error of input 4\n");
			return -3;
		}		

		if (n<=0 || m<=0 || k<0 || k>4 || m>n)
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
		inp = fopen(argv[4],"r");	
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
			printf("Error of input: wrong size of matrix \n");
			return -5;
		}
	}

	
    if (sysinfo(&info) != 0)
	{
        printf("sysinfo: error reading system statistics");
	}
/*
	printf("Free RAM: %lld MB\n", (long long)(info.totalram-info.freeram));
	printf("%lld \n", (long long)56*n*n);
	printf("%lld \n", (long long)((long long)(info.totalram-info.freeram) - (long long)56*n*n));
*/

	if ( (long long)((long long)(info.totalram-info.freeram) - (long long)8*n*n*7) < 0 )   // 7 ~ кол-во массивов 
	{
		printf("Memory troubles \n");
		return -10;
	}


//	printf("TEEEEEEST   1 \n");

	matr=(double*)malloc(n*n*sizeof(double));
	matr_v2 = (double*)malloc(n*n*sizeof(double));
	reverse=(double*)malloc(n*n*sizeof(double));
	index = (int*)malloc(n*sizeof(int));
	answer = (double*)malloc(n*n*sizeof(double));
	
	
	
//	printf("TEEEEEEST   3 \n");
	
	if ( !(matr) || !(matr_v2)  || !(reverse) || !(index) || !(answer))
	{
		printf("Memory troubles  \n");
		if (matr)
		{
			free(matr);
		}
		if (matr_v2)
		{
			free(matr_v2);
		}
		if (reverse)
		{
			free(reverse);
		}
		if (index)
		{
			free(index);
		}
		if (answer)
		{
			free(answer);
		}
		return -7;
	}

	for (int i=0; i<n; i++)
	{
		index[i] = i;
	}

	for (int i=0;i<n;i++)
		{
			for(int j=0;j<n;j++)
			{
				if (j==i)
				{
					reverse[i*n+j] = 1;
				}
				else
				reverse[i*n+j] = 0;
			}
		}
	


	if (k==0)
	{	
		create_matrix_file(matr, n, argv[4]);
	}
	else
	{
		for (int i=0;i<n;i++)
		{
			for(int j=0;j<n;j++)
			{
				matr[i*n + j] = create_element(k, n, i+1, j+1); 
				matr_v2[i*n + j] = matr[i*n + j];
			}
		}
	}
	
	


	printf("Input matrix : \n");
	output (n, m, matr);

	start = clock();
	res = solve(n, matr, reverse, index, answer);
	stop = clock();

	if (res < 0)
	{
		printf("Input Matrix is degenerate \n");
		free(matr);
		free(matr_v2);
		free(reverse);
		free(index);
		free(answer);
		return -6;
	}

	printf("Reverse matrix : \n");
	output(n, m, reverse);

	printf("Time for solve : %lf seconds \n", (double)(stop - start) / (double)CLOCKS_PER_SEC );

	printf("Norma = %10.3e \n", norma(matr_v2, reverse, n));
	

	free(matr);
	free(matr_v2);
	free(reverse);
	free(index);
	free(answer);
return 1;
}
