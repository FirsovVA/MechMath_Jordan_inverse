#include <math.h>
#include <stdio.h>
#include <stdlib.h>


//#include <sys/sysinfo.h> 
//#include <unistd.h>     
#include <mpi.h>

#include "matrix.h"
#include "algorithm.h"

// mpirun -n x ./prog n m k ( input file)

int error = 1;

int main (int argc, char** argv)
{
	
	
	int n, k, m, result, column_len, count=0;
	double a;
	double time;
	FILE* inp = NULL;
	double* matr = NULL;
	double* reverse = NULL;
	int* index = NULL;
	double* main_str = NULL;
	double* main_reverse_str = NULL;
//	double norm_of_matr;
	double res_norm;
	//struct sysinfo info;
	//long int time_full;
	int total_threads;
	int my_rank;
	int fr, lr, row_count;
	int my_recvcount;
	
	int* recvcounts = NULL;
	int* displs = NULL;
	double* res_array = NULL;
	double* res_reverse = NULL;
	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &total_threads);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


	if (argc==4 || argc==5)
	{
		result = sscanf(argv[1], "%d", &n);
		if (result == 1)
		{
			if (result == EOF)			
			{
				if(my_rank == 0)
				{
					printf("Error of input n 1.1\n");
				}

				MPI_Finalize();
				return -1;
			}
		}
		else
		{	
			if(my_rank == 0)
			{
				printf("Error of input n 1.2\n");
			}
			MPI_Finalize();
			return -1;
		}

		result = sscanf(argv[2],"%d", &m);
		if (result == 1)
		{
			if (result == EOF)			
			{
				if(my_rank == 0)
				{
					printf("Error of input m 2.1 \n");
				}

				MPI_Finalize();
				return -2;
			}
		}
		else
		{
			if(my_rank == 0)
			{
				printf("Error of input m 2.2\n");
			}
			MPI_Finalize();
			return -1;
		}


		result = sscanf(argv[3],"%d", &k);
		if (result == 1)
		{
			if (result == EOF)			
			{
				if (result == EOF)			
				{
					printf("Error of input k 3.1\n");
				}
				MPI_Finalize();
				return -3;
			}
		}
		else
		{
			if(my_rank == 0)
			{
				printf("Error of input k 3.2\n");
			}
			MPI_Finalize();
			return -3;
		}


		if ( (k!=0 && argc==5) || (k==0 && argc==4)  )
		{
			printf("Error of input 4\n");
			return -3;
		}		

		if (n<=0 || m<=0 || k<0 || k>4 || m>n || total_threads < 1)
		{
			if(my_rank == 0)
			{
				printf("Error of input \n");
			}
			MPI_Finalize();
			return -3;
		}
	}
	else
	{
		if(my_rank == 0)
		{
			printf("Wrong number of arguments \n");
		}
		MPI_Finalize();
		return -7;
	}
	
	
	if (k==0)
	{
		inp = fopen(argv[4],"r");	
		if (inp == NULL)
		{
			if(my_rank == 0)
			{
				printf("Can't open file \n");
			}
			MPI_Finalize();
			return -4;
		}

		while(fscanf(inp,"%lf",&a) == 1)
		{
			count++;
		}
		fclose(inp);
				
		if (count!=n*n)
		{
			if(my_rank == 0)
			{
				printf("Error of input: wrong size of matrix !!!!!!!!!!\n");
			}
			MPI_Finalize();
			return -5;
		}
	}


	row_count = n / total_threads;
	fr = my_rank * row_count;
	lr = (my_rank + 1) * row_count;
	if ( my_rank == total_threads - 1 )
	{
		lr = n;
	} /* для последнего*/

	// высота полосы
	column_len = lr - fr;
//	printf("\nCHEEEEEEEEEK  column_len : %d  row_count = %d  - from %d thread \n ",column_len, row_count, my_rank);	
	

	matr = (double*)malloc(column_len * n * sizeof(double));
	reverse = (double*)malloc(column_len * n * sizeof(double));
	index = (int*)malloc(n*sizeof(int));
	main_str = (double*)malloc(n * sizeof(double));
	main_reverse_str = (double*)malloc(n * sizeof(double));
/*	
    if (sysinfo(&info) != 0)
	{
        printf("sysinfo: error reading system statistics");
	}


	if ( (long long)((long long)(info.totalram-info.freeram) - (long long)8*n*n*7) < 0 )   // 7 ~ кол-во массивов 
	{
		printf("Memory troubles \n");
		return -10;
	}
*/
	
	
	if ( !(matr) || !(reverse) || !(index)  )
	{
		if(my_rank == 0)
		{
			printf("Memory troubles\n");
		}
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (main_str) free(main_str);
		if (main_reverse_str) free(main_reverse_str);
		MPI_Finalize();
		return -7;
	}

	
	if (k==0)
	{
		inp = fopen(argv[4],"r");	
		if (inp == NULL)
		{
			if(my_rank == 0)
			{
				printf("Can't open file \n");
			}
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			if (main_str) free(main_str);
			if (main_reverse_str) free(main_reverse_str);
			
			MPI_Finalize();
			return -4;
		}
	}
	result = Input_matr(n, matr, k, inp, my_rank, total_threads, fr, lr);
	
	if (result == -1)
	{
			if (matr) free(matr);
			if (reverse) free(reverse);
			if (index) free(index);
			if (main_str) free(main_str);
			if (main_reverse_str) free(main_reverse_str);
			MPI_Finalize();
			return -8;
	}

	
	
	if(my_rank == 0)	printf("Input matrix : \n");

	MPI_Barrier(MPI_COMM_WORLD);
	output (n, matr, m, total_threads, my_rank, fr, lr);
	MPI_Barrier(MPI_COMM_WORLD);
		
	
	// MPI_Bcast(&norm_of_matr, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// if (my_rank == 0)
	// 	printf("Matrix NORM = %10.3e \n", norm_of_matr);




	MPI_Barrier(MPI_COMM_WORLD);
	time = MPI_Wtime();
	
	result = InvMatrix(n, matr, reverse, index, my_rank, fr, lr, column_len, total_threads, main_str, main_reverse_str);
	
	MPI_Barrier(MPI_COMM_WORLD);
	time = MPI_Wtime() - time;



	if (result < 0)
	{
		if (my_rank == 0)
		{
			printf("Input matrix is degenerate \n");
		}
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (main_str) free(main_str);
		if (main_reverse_str) free(main_reverse_str);
		MPI_Finalize();
		return -8;
		
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(my_rank == 0)	
		printf("Inverse matrix : \n");

	MPI_Barrier(MPI_COMM_WORLD);
	output (n, reverse, m, total_threads, my_rank, fr, lr);
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	
if (my_rank == 0)
{
	printf("Time spent while inverting = %lf\n", time);
}

if (k==0)
{
	inp = fopen(argv[4],"r");	
	if (inp == NULL)
	{
		if(my_rank == 0)
		{
			printf("Can't open file \n");
		}
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (main_str) free(main_str);
		if (main_reverse_str) free(main_reverse_str);
		
		MPI_Finalize();
		return -4;
	}
}
result = Input_matr(n, matr, k, inp, my_rank, total_threads, fr, lr);

if (result == -1)
{
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (main_str) free(main_str);
		if (main_reverse_str) free(main_reverse_str);
		MPI_Finalize();
		return -8;
}


my_recvcount = n * column_len;

recvcounts = (int*)malloc(total_threads * sizeof(int));
displs = (int*)malloc(total_threads * sizeof(int));
if ( !(recvcounts) || !(displs))
	{
		if(my_rank == 0)
		{
			printf("Memory troubles\n");
		}
		if (recvcounts)	free(recvcounts);
		if (displs)	free(displs);
		if (matr) free(matr);
		if (reverse) free(reverse);
		if (index) free(index);
		if (main_str) free(main_str);
		if (main_reverse_str) free(main_reverse_str);
		MPI_Finalize();
		return -7;
	}

if (my_rank == 0)
{
	res_array = (double*)malloc(n * n * sizeof(double));
	res_reverse = (double*)malloc(n * n * sizeof(double));

	if ( !(res_array) || !(res_reverse))
	{
		if (res_array)	free(res_array);
		if (res_reverse)	free(res_reverse);
		error = -1;
	}
}
MPI_Barrier(MPI_COMM_WORLD);

if (error < 0)
{
	if(my_rank == 0)
	{
		printf("Memory troubles\n");
	}
	if (recvcounts)	free(recvcounts);
	if (displs)	free(displs);
	if (matr) free(matr);
	if (reverse) free(reverse);
	if (index) free(index);
	if (main_str) free(main_str);
	if (main_reverse_str) free(main_reverse_str);
	MPI_Finalize();
	return -7;
}
MPI_Barrier(MPI_COMM_WORLD);


for (int m = 0; m < total_threads; m++)
{
	displs[m] = m * n * row_count;
}

	
MPI_Gather(&my_recvcount, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
MPI_Gatherv(&(matr[0]), my_recvcount, MPI_DOUBLE, res_array, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Gatherv(&(reverse[0]), my_recvcount, MPI_DOUBLE, res_reverse, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);


if (my_rank == 0)
{
	res_norm = norm(res_array, res_reverse, n);
	printf("Norm = %10.3e \n", res_norm);
}

MPI_Barrier(MPI_COMM_WORLD);

if (my_rank == 0)
{
	if (res_array)	free(res_array);
	if (res_reverse)	free(res_reverse);
}

if (recvcounts)	free(recvcounts);
if (displs)	free(displs);
if(matr)	free(matr);
if(reverse)	free(reverse);
if(index)	free(index);
if(main_str)	free(main_str);
if(main_reverse_str)	free(main_reverse_str);
MPI_Finalize();

return 1;
}
