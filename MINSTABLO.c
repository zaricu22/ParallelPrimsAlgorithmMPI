/*
 ============================================================================
 Name        : MINSTABLO.c
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

int main(int argc, char* argv[]){
	int  my_rank; /* rank of process */
	int  p;       /* number of processes */

	/* start up MPI */
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p); 

	/* SHORT da ne zauzima graf mnogo memorije lako se prebaci na vece  */

//=====================	   UCITAVANJE i RASPODELA	 ================================
	// UCITAVANJE GRAFA
		/* BROJ CVOROVA GRAFA	*/
		int n = atoi(argv[1]);	// unosi se kao arg main-a
		/* GRANE GRAFA(VEZA IZMEDJU SVAKA DVA CVORA)
		* 0 - VEZA SA SOBOM, SHRT_MAX - nepostojanje veze izmedju dva cvora
		* velicine N*N, alocira po potrebi jer nije potreban svim procesima */
		short* matricaGrafa;
		if(my_rank == 0)
			matricaGrafa = (short*) malloc(n*n*sizeof(short));
	// ZA RASPODELU NA LOKALNE PODGRAFOVE
		// velicina dela grafa(br redova matrice) zaduzen jedan proces
		int local_n = n/p;
		// deo grafa(podmatrica) zaduzen jedan proces
		short* lokalniPodgraf = (short*) malloc(local_n*n*sizeof(short));
		// visak ako graf nije deljiv na procese(velicine tacno jednog reda matrice)
		// dodeljuje se procesima pocevsi od 1
		// velicine N, alocira po potrebi jer nije potreban svim procesima
		short* visakPodgraf;
		if(n % p != 0 &&  my_rank <= n%p && my_rank != 0)
			visakPodgraf = (short*) malloc(n*sizeof(short));
	
	//-----------------------	 ROOT	  ------------------------------
	if (my_rank == 0){

	// UCITAVANJE MATRICE KOJA OPISUJE ULAZNI POVEZANI NEUSMEREN GRAF
		FILE* file;
		char* filename = argv[3];	// unosi se kao arg main-a
		file = fopen(filename,"rb");
		fread(matricaGrafa,n*n*sizeof(short),1,file);

		/* ISPIS ULAZNE MATRICE(UZORAK 6x6 + KRAJEVI)

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
		printf("\n\n"); fflush(stdout);

		*/

		fclose(file);

	//-----------------------	 RASPODELA - SLANJE	  ------------------------------
	// RASPODELA MATRICE ULAZNOG GRAFA NA LOKALNE MATRICE PODGRAFOVA KA SVIM PROCESIMA

		// AKO JE GRAF DELJIV NA PROCESE
		if(n % p == 0){
			MPI_Scatter(matricaGrafa, local_n*n, MPI_SHORT, lokalniPodgraf, local_n*n, MPI_SHORT, 0, MPI_COMM_WORLD);
		}
		// AKO GRAF NIJE DELJIV NA PROCESE
		else{
			// root posalje svima pa i sebi njihove lokalne podgrafove
			MPI_Scatter(matricaGrafa, local_n*n, MPI_SHORT, lokalniPodgraf, local_n*n, MPI_SHORT, 0, MPI_COMM_WORLD);
			// visak raspodeliti na preostale procese
			for(int i=1; i<=n%p; i++){
				MPI_Send(matricaGrafa+(p*local_n*n)+((i-1)*n), n, MPI_SHORT, i, 0, MPI_COMM_WORLD);
			}
		}
	}

	//-----------------------	OSTALI	  ------------------------------

	if (my_rank != 0) {
	//-----------------------	 RASPODELA - PRIMANJE	  ------------------------------
		// AKO JE GRAF DELJIV NA PROCESE
		if(n % p == 0){
			// UCITAVANJE LOKALNIH PODGRAFOVA SA ROOT-A
			MPI_Scatter(matricaGrafa, local_n*n, MPI_SHORT, lokalniPodgraf, local_n*n, MPI_SHORT, 0, MPI_COMM_WORLD);
		}
		// AKO GRAF NIJE DELJIV NA PROCESE
		else{
			// svakako prima glavni deo lokalnog grafa
			MPI_Scatter(matricaGrafa, local_n*n, MPI_SHORT, lokalniPodgraf, local_n*n, MPI_SHORT, 0, MPI_COMM_WORLD);
			// ako je zaduzen prima visak lokalnog grafa
			if(my_rank <= n%p){
				MPI_Recv(visakPodgraf, n, MPI_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			}
		}
	}


//==========================	IZRACUNAVANJE	================================

	// ZA IZRACUNAVANJE
		/* cvorovi dodati u minimalno stablo, njihove grane su vec razmatrane pa ne moraju ponovo
		* 1 = predjen, ostalo = nije predjen
		* alocira po potrebi jer nije potreban svim procesima u istoj velicini
		* calloc() da bi inic sve na 0(obrisali prethodne vrednosti u memoriji) */
		char* predjeniCvorovi;
		if(my_rank == 0)	// root cuva za sve zbog provere najmanjeg
			predjeniCvorovi = (char*) calloc(n,sizeof(char));
		else
			if(n % p != 0 && my_rank <= n%p)	// ostali sa viskom
				predjeniCvorovi = (char*) calloc((local_n+1),sizeof(char));
			else	// ostali bez viska
				predjeniCvorovi = (char*) calloc(local_n,sizeof(char));
		//------------------------------
		/* cvor poslednji dodat u minimalno stablo, krece se od prvog
		* na pocetku dodamo koren stabla koji se unosi se kao arg main-a */
		int poslMinCvor = atoi(argv[2])-1;
		// oznacimo pocetni cvor kao predjeni na odgovarajucim procesima
		if(my_rank == 0)
			predjeniCvorovi[poslMinCvor] = '1';
		else {
			if(n % p != 0 && poslMinCvor == p*local_n+my_rank-1)
				predjeniCvorovi[local_n] = '1';
			else if(poslMinCvor >= my_rank*local_n && poslMinCvor < my_rank*local_n+local_n)
				predjeniCvorovi[poslMinCvor%local_n] = '1';
		}

		//------------------------------
		/* deo konacnog minimalnog stabla za koji zaduzen jedan proces
		* na pocetku nema stabla pa se svi cvorovi postavljaju na beskonacno
		* alocira po potrebi jer nije potreban svim procesima u istoj velicini */
		short* lokalnoStablo;
		if(n % p != 0 && my_rank <= n%p && my_rank != 0){ // sa viskom
			lokalnoStablo = (short*) malloc((local_n+1)*sizeof(short));
			for(int x=0; x<local_n+1; x++)
				lokalnoStablo[x] = SHRT_MAX;
		} else {	// bez viska
			lokalnoStablo = (short*) malloc(local_n*sizeof(short));
			for(int x=0; x<local_n; x++)
				lokalnoStablo[x] = SHRT_MAX;
		}
		//------------------------------
		/*  konacno minimalno stablo(grane svakog cvora), indeks-vrednost <=> cvor-vred min grane
		* velicine N, alociraju po potrebi jer nisu potrebani svim procesima */
		short* minimalnoStablo;
		if(my_rank == 0){
			minimalnoStablo = (short*) malloc(n*sizeof(short));
			for(int x=0; x<n; x++){
				minimalnoStablo[x] = SHRT_MAX;
			}
			minimalnoStablo[poslMinCvor] = 0;
			lokalnoStablo[poslMinCvor] = 0;
		}

	double start;
	double finish;
	start = MPI_Wtime();

	//-------------------------	  ALGORITAM	  ------------------------
	/* algoritam ima tacno (ukupno cvorova - 1) iteracija
	* ne moze vise cvorova niti iteracija istovremeno,
	* jer svaka iter daje novi minimalni cvor od kojeg zavise dalja izracunavanja
	* ali moze unutar svake iteracije paralelno azuriranje minimalnog stabla na osnovu novog min cvora  */
	int globalMinValue;	// prom se ne smaju dekl unutar petlji(zauz previse mem)
	for(int i=0; i<n-1; i++){

		for(int j=0; j<local_n; j++){
			if(predjeniCvorovi[j] != '1' && lokalniPodgraf[j*n+poslMinCvor] < lokalnoStablo[j]){
				lokalnoStablo[j] = lokalniPodgraf[j*n+poslMinCvor];
			}
		}

		if(my_rank != 0 && n % p != 0 && my_rank <= n%p){
			if(predjeniCvorovi[local_n] != '1' && visakPodgraf[poslMinCvor] < lokalnoStablo[local_n]){
				lokalnoStablo[local_n] = visakPodgraf[poslMinCvor];
			}
		}

		// sakupljane lokalnih stabala u minimalno stablo root-a
		MPI_Gather(lokalnoStablo, local_n, MPI_SHORT, minimalnoStablo, local_n, MPI_SHORT, 0, MPI_COMM_WORLD);
		// slanje viska lokalnog stabala minimalnom stablu root-a
		if(my_rank != 0 && n % p != 0 && my_rank <= n%p)
			MPI_Send(lokalnoStablo+local_n, 1, MPI_SHORT, 0, 0, MPI_COMM_WORLD);

		if(my_rank == 0){
			// prijem visaka lokalnih stabala u minimalno stablo root-a
			if(n % p != 0)
				for(int z=0; z<n-p*local_n; z++)
					MPI_Recv(minimalnoStablo+p*local_n+z, 1, MPI_SHORT, z+1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			// pronalazenje trenutnog najmanjeg elementa u minimalnom stablu
			// nepozeljno paralelizovati jer bi zahtevalo dodatnu mreznu komunikaciju
			globalMinValue = SHRT_MAX;
			for(int z=0; z<n; z++){
				if(predjeniCvorovi[z] != '1' && minimalnoStablo[z] < globalMinValue){
					poslMinCvor = z;
					globalMinValue = minimalnoStablo[z];
				}
			}
			// oznacavanje novoizabranog najmanjeg elementa kao predjeni na root-u
			predjeniCvorovi[poslMinCvor] = '1';
		}

		// svima se salje samo novoizabrani najmanji cvor
		MPI_Bcast(&poslMinCvor, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// oznacavanje novoizabranog najmanjeg cvora kao predjeni na ostalim procesima
		// izbegavanje mrezne komunikacije vec lokalna provera na osnovu pristiglog poslednjeg minimalnog cvora
		if(my_rank != 0) {
			if(n % p != 0 && poslMinCvor == p*local_n+my_rank-1)
				predjeniCvorovi[local_n] = '1';
			else if(poslMinCvor >= my_rank*local_n && poslMinCvor < my_rank*local_n+local_n)
				predjeniCvorovi[poslMinCvor%local_n] = '1';
		}
	}

	finish = MPI_Wtime();
	double elapsed = finish - start;

	// ISPIS REZULTATA
	 if(my_rank == 0){
	 /*	char c;	int i = 0;
		printf("\n");
		for (c = 'A'; c <= 'Z'; ++c){
			if(i == n)
				break;
			printf("%c\t", c);	i++;
		}
		printf("\n");
	  */
		int sum = 0;
		for(int y=0; y<n; y++){
			sum += minimalnoStablo[y];
		}
		printf("RANK %d =  %d sum	|	%f sec\n",my_rank,sum,elapsed);
		printf("\n"); fflush(stdout);
	 }
	 else {
		 printf("RANK %d	|	%f sec\n",my_rank,elapsed);
		 printf("\n"); fflush(stdout);
	 }


	// OSLOBADJANJE ALOCIRANJE MEMORIJE OD STRANE PROCESA KOJI JE ZAUZEO
	if(my_rank == 0){
		free(matricaGrafa);
		free(minimalnoStablo);
	}
	if(n % p != 0 &&  my_rank <= n%p && my_rank != 0)
		free(visakPodgraf);
	free(predjeniCvorovi);
	free(lokalnoStablo);

	/* shut down MPI */
	MPI_Finalize();
	
	
	return 0;
}
