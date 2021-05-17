#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "algorithm.h"



int Index_max(double* matr, int n, int k)
{	
	double max = fabs( matr[k*n+k] );
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

void swap_matrix (double* matr, int n, int i, int j, int column_len)
{
	double a;
	
	for(int k=0; k<column_len; k++)
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


double matrix_norm(int n, double* matr, int my_rank, int fr, int lr, int total_threads)
{
	double max = 0;
	double res_max = 0;
	double sum = 0;
	double* array_max = NULL;

	for(int i = 0; i < (lr - fr); i++)
	{
		sum = 0;
		for(int j = 0; j < n; j++)
		{
			sum+=fabs(matr[i*n + j]);
		}
		if (sum > max)
			max = sum;	
	}
	if (my_rank == 0)
		array_max = (double*)malloc( total_threads  * sizeof(double) );
	
	
	MPI_Gather(&max, 1, MPI_DOUBLE, array_max, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if (my_rank == 0)
	{
		res_max = 0;
		for (int i = 0; i < total_threads; i++)
		{
			if (array_max[i] > res_max)
				res_max = array_max[i];
		}
		if(array_max)	free(array_max);
		return res_max;
	}
	
	if(array_max)	free(array_max);
	return 1;
}








int InvMatrix(int n, double* matr, double* reverse, int* index, int my_rank, int fr, int lr, int column_len, int total_threads, double* main_str, double* main_reverse_str)
{
	
	int i, j, k, p;
	int cur = 0;
	int res = 1;
	int index_max;
	int row_count;
	double norm;
	double main_elem, tmp;
	double* res_array = NULL;
	double* res_reverse = NULL;
	int* recvcounts = NULL;
	int* displs = NULL;
	int* displ_v2 = NULL;
	int my_recvcount;	
	int my_displs;
	


	for (i = 0; i < n; i++)
		index[i] = i;

	for (i = 0; i < n; i++)
	{
		if (i >= fr && i < lr)
		{
			for (j = 0; j < n; j++)
				reverse[(i - fr) * n + j] = (double)(i == j);
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);

	norm = matrix_norm(n, matr, my_rank, fr, lr, total_threads);
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Bcast(&norm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	row_count = n / total_threads;

	for (i=0; i<n; i++)
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
		//if ( i >= fr && i < lr)
		if (my_rank == cur)
		{
			index_max = Index_max(matr, n, i - fr);
		}

		MPI_Barrier(MPI_COMM_WORLD);

	//	printf("\nCHEEEEEEEEEK  11111   from %d chel\n ", my_rank);	

		MPI_Bcast(&index_max, 1, MPI_INT, cur, MPI_COMM_WORLD);

		
		
	//	printf("I am %d chel, my index max = %d (i = %d) (cur = %d)\n", my_rank, index_max, i, cur);
		
		
		swap_matrix(matr, n, i, index_max, column_len);
	//	printf("\nCHEEEEEEEEEK  11122   from %d chel\n ", my_rank);	
	
		swap_index(index, i, index_max);
		
	
	//	printf("OK from chel %d \n", my_rank);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(index, n, MPI_INT, cur, MPI_COMM_WORLD);

	//	printf("\nCHEEEEEEEEEK  22222   from %d chel\n ", my_rank);	


	/*
		if (my_rank == 2)
		{
			printf("INDEX MASSIVE = ");
			for(int y = 0; y < n; y++)
			{
				printf("%d ", index[y]);
			}
			printf("\n");
		}
	*/


		if (my_rank == cur)
		{	
			//проверка на вырожденность
			if ( fabs(matr[(i - fr) * n + i] ) < 1e-15 * norm )
			{			 
				res = -1;
			} 

		//	printf("I AM %d chel , MY RES = %d ////////// ( main elem = %10.3e) , (norm = %10.3e) \n",my_rank, res, fabs(matr[(i - fr) * n + i]) , norm);
	
			main_elem = 1/matr[(i - fr) * n + i];

			for (j=i; j<n; j++)
			{
				matr[(i - fr) * n + j] *= main_elem;
			}
			
			for (j=0; j<n; j++)
			{
				reverse[(i - fr) * n + j] *= main_elem;
			}
			
		}	
	
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast( &res, 1, MPI_INT, cur, MPI_COMM_WORLD );

		
		
		//printf("I AM %d chel , MY RES = %d \n",my_rank, res);
		//MPI_Barrier(MPI_COMM_WORLD);
		if (res < 0)
		{
			return -1;
		}
		MPI_Barrier(MPI_COMM_WORLD);


		if (my_rank == cur)
		{

			for (int m = 0; m < n; m++)
			{
				main_str[m] = matr[(i - fr) * n + m];
				main_reverse_str[m] = reverse[(i - fr) * n + m];
				//printf("%10.3e ", main_reverse_str[m]);
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
 
		MPI_Bcast(main_str, n, MPI_DOUBLE, cur, MPI_COMM_WORLD);
		MPI_Bcast(main_reverse_str, n, MPI_DOUBLE, cur, MPI_COMM_WORLD);
		
		
		for (j = fr; j < lr; j++)
		{
			if (j != i)				// чтоб не вычесть i-ую строку саму из себя
			{	
				tmp = matr[(j - fr) * n + i];
				for (k = i; k < n; k++)
					matr[(j - fr) * n + k] -= tmp * main_str[k];
				for (k = 0; k < n; k++)
					reverse[(j - fr) * n + k] -= tmp * main_reverse_str[k];
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	// for (int z = 0; z < total_threads; z++)
	// {	
	// 	if (my_rank == z)
	// 	{	
	// 		printf("===============\n");
	// 		for (int u = fr; u < lr; u++)
	// 		{
	// 			for (int r = 0; r < n; r++)
	// 				printf("%10.3e ", reverse[(u - fr) * n + r]);
	// 			printf("\n");
				
	// 		}
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		printf("===============\n");
	// 	}
	// 	MPI_Barrier(MPI_COMM_WORLD);
	// }
	// MPI_Barrier(MPI_COMM_WORLD);
	
	
	my_recvcount = n * column_len;
	my_displs = n*fr;
	recvcounts = (int*)malloc(total_threads * sizeof(int));
	displs = (int*)malloc(total_threads * sizeof(int));
	displ_v2 = (int*)malloc(total_threads * sizeof(int));
	//MPI_Allgather(&my_recvcount, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);
	
	for (int m = 0; m < total_threads; m++)
	{
		if (m != my_rank)
			displs[m] = 0;
		else
		{
			displs[m] = my_displs;
		}
		displ_v2[m] = 0;
		
	}
	
	// for (int m = 0; m < total_threads; m++)
	// {
	// 	printf("im %d displ = %d for %d chel \n", m, displs[m], my_rank);	
	// }
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Allreduce(displs, displ_v2, total_threads, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allgather(&my_recvcount, 1, MPI_INT, recvcounts, 1, MPI_INT, MPI_COMM_WORLD);

	// printf("\nCHEEEEEEEEEK   \n ");

	

	// printf("Displs for %d chel \n", my_rank);	
	// for (int g = 0; g < total_threads; g++)
	// 	printf("%d  ", displ_v2[g]);
	// //printf("\nCHEEEEEEEEEK   \n ");	

	// 	MPI_Barrier(MPI_COMM_WORLD);



	
	

	res_array = (double*)malloc(n * n * sizeof(double));  // записываем обратную матрицу, чтобы поменять строки 	
	res_reverse = (double*)malloc(n * n * sizeof(double)); // в ней обратная матрица уже лежит с норм порядком строк
		

	MPI_Barrier(MPI_COMM_WORLD);

// ДОСЮДА ВСЁ НОРМ 

	
	// if (my_rank == 0)
	// {
	// 	printf("\nДолжен быть норм первый блок обратной матрицы до перестановки \n");
	// 	for (int w = 0; w < recvcounts[0]; w++)
	// 		printf("%10.3e ", reverse[w]);
	// 	printf("\n\n");	
		
	// }
	// if (my_rank == 0)
	// {
	// //	printf("\nCHECK 1 ELEM : %10.3e \n",&(reverse[0]) );
	// }



	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Allgatherv(&(reverse[0]), my_recvcount, MPI_DOUBLE, res_array, recvcounts, displ_v2, MPI_DOUBLE, MPI_COMM_WORLD);	// трабл здесь
//	MPI_Allgatherv(&(reverse[0]), my_recvcount, MPI_DOUBLE, res_array, recvcounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);	// трабл здесь
	
	

	
	
	for (int i=0; i<n; i++)
	{
		
		for (int j=0; j<n; j++)
		{
				
			res_reverse[index[i] * n + j] = res_array[ i * n + j];
			//printf("%10.3e ", res_reverse[i * n + j]);
		}
		//printf("\n");
	
	}
	// if (my_rank == 0)
	// {	
	// 	for (int i=0; i<n; i++)
	// 	{
			
	// 		for (int j=0; j<n; j++)
	// 		{
	// 			printf("%10.3e ", res_reverse[i * n + j]);
	// 		}
	// 		printf("\n");
		
	// 	}
		
	// }		
	MPI_Barrier(MPI_COMM_WORLD);
//	рассылаем итоговую обратную матрицу обратно на процессам

	for(i = 0; i < n; i++)
	{
		if (i >= fr && i < lr)
		{
			for(j = 0; j < n; j++)
			{
				reverse[(i - fr) * n + j] = res_reverse[i * n +j];
				//printf(" Reverse = %lf    and	res_reverse = %lf  for man %d  (i = %d; j = %d) \n", reverse[(i - fr) * n + j], res_reverse[i * n +j], my_rank, i, j);
			}
		}
	} 
	MPI_Barrier(MPI_COMM_WORLD);

	// for (int i=fr; i<lr; i++)
	// {

	// 	for (int j=0; j<n; j++)
	// 	{
	// 		printf("***** %10.3e ", reverse[(i - fr) * n + j]);
	// 	}
	// 	printf("\n");

	// }
	// MPI_Barrier(MPI_COMM_WORLD);



/*
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
				a = res_reverse[ (i - cur * row_count) * n + j]; // cur * (n / total_threads) = fr
				
				if(cur == 0)
				{
					reverse[(i - fr)* n + j] = a;
				} 
				else 
				{
					MPI_Send( &a, 1, MPI_DOUBLE, cur, 0, MPI_COMM_WORLD ); 
				}
			}
			
			if(cur != 0 && my_rank == cur)
			{
				MPI_Recv(&a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				reverse[(i - fr) * n + j ] = a;
			}	
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
*/	
	if(res_array)	free(res_array);
	if(res_reverse)	free(res_reverse);
	if(recvcounts) free(recvcounts);
	if(displs)	free(displs);
	if(displ_v2)	free(displ_v2);
	

return 1;		
}
