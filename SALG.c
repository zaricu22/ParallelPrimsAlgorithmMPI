/*
 ============================================================================
 Name        : SALG.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello MPI World in C 
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "mpi.h"

typedef enum { false, true } bool;

int main(int argc, char* argv[]){
	int  my_rank; /* rank of process */
	int  p;       /* number of processes */

	//	int source;   /* rank of sender */
	//	int dest;     /* rank of receiver */
	//	int tag=0;    /* tag for messages */
	//	char message[100];        /* storage for message */
	//	MPI_Status status ;   /* return status for receive */
	
	/* start up MPI */
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p); 

	// BROJ CVOROVA GRAFA
	int n = atoi(argv[1]);
	// GRANE GRAFA(VEZA IZMEDJU SVAKA SVA CVORA)
	// 0 - VEZA SA SOBOM, SHRT_MAX - nepostojanje veze izmedju dva cvora
	short* matricaGrafa = (short*) calloc(n*n,sizeof(short));;

	if (my_rank == 0){
		// UCITAVANJE MATRICE KOJA OPISUJE ULAZNI POVEZANI NEUSMEREN GRAF
		FILE* file;
		char* filename = argv[3];
		file = fopen(filename,"rb");
		fread(matricaGrafa,n*n*sizeof(short),1,file);

		/*
		for(int i=0; i<n; i++){
			printf("\n");
			for(int j=0; j<n; j++){
				printf("%d  \t", matricaGrafa[i*n+j]);
			}
		}
		printf("\n"); fflush(stdout);

		printf("\nUZORAK (6x6 + KRAJEVI): \n");
		for(int j=0; j<6; j++){
			for(int i=0; i<6; i++){
				printf("%d  \t", matricaGrafa[j*n+i]);
			}
			printf("|  %d  \t", matricaGrafa[j*n+(n-1)]);
			printf("\n");
		}
		printf("--------------------------------------------------------\n");
		for(int i=0; i<6; i++){
			printf("%d  \t", matricaGrafa[(n-1)*n+i]);
		}
		printf("|  %d  \t", matricaGrafa[(n-1)*n+(n-1)]);
		printf("\n\n");
		*/

		fclose(file);

		// PRONALAZENJE MIN STABLA
		short d[n];
		bool Vt[n];
		int root = atoi(argv[2]);

		/* SEKVENCIJALAN ALGORITAM ZA 1 PROCES */
		// RAD SA VREDNOSTIMA GRANA
		for(int v=0; v<n; v++){
			Vt[v] = false;
			d[v] = matricaGrafa[root*n+v];
		}
		Vt[root] = true;
		d[root] = 0;

		double start;
		double finish;
		if(my_rank == 0){
			start = MPI_Wtime();
		}

		for(int i=1; i<n; i++){
			int minU = 0;
			short minUD = SHRT_MAX;
			for(int j=0; j<n; j++){
				if(Vt[j] != true && d[j] < minUD){
						minU = j;
						minUD = d[j];
				}
			}

			minUD = SHRT_MAX;
			Vt[minU] = true;

			for(int v=0; v<n; v++){
				if(Vt[v] != true && d[v] > matricaGrafa[minU*n+v]){
					d[v] = matricaGrafa[minU*n+v];
				}
			}
		}

		finish = MPI_Wtime();
		double elapsed = finish - start;
		printf("RANK %d  |  %f sec\n",my_rank,elapsed);
		printf("\n"); fflush(stdout);

		/*
		char c;	int i = 0;
		printf("\n");
		for (c = 'A'; c <= 'Z'; ++c){
			if(i == n)
				break;
			printf("%c\t", c);	i++;
		}
		printf("\n");
		int sum = 0;
		for(int y=0; y<n; y++){
			sum += d[y];
			printf("%d\t", d[y]);
		}
		printf("=  %d\n",sum);
		printf("\n");


		int sum = 0;
		for(int y=0; y<n; y++){
			sum += d[y];
		} */
	}

	free(matricaGrafa);

	/* shut down MPI */
	MPI_Finalize(); 
	
	
	return 0;
}
