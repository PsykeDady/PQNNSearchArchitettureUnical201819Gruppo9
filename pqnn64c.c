/**************************************************************************************
 * 
 * CdL Magistrale in Ingegneria Informatica
 * Corso di Architetture e Programmazione dei Sistemi di Elaborazione - a.a. 2018/19
 * 
 * Progetto dell'algoritmo di Product Quantization for Nearest Neighbor Search
 * in linguaggio assembly x86-64 + AVX
 * 
 * Fabrizio Angiulli, aprile 2019
 * 
 **************************************************************************************/

/*
 
 Software necessario per l'esecuzione:

     NASM (www.nasm.us)
     GCC (gcc.gnu.org)

 entrambi sono disponibili come pacchetti software 
 installabili mediante il packaging tool del sistema 
 operativo; per esempio, su Ubuntu, mediante i comandi:

     sudo apt-get install nasm
     sudo apt-get install gcc

 potrebbe essere necessario installare le seguenti librerie:

     sudo apt-get install lib32gcc-4.8-dev (o altra versione)
     sudo apt-get install libc6-dev-i386

 Per generare il file eseguibile:

 nasm -f elf64 pqnn64.nasm && gcc -O0 -m64 -msse pqnn64.o pqnn64c.c -o pqnn64c && ./pqnn64c
 
 oppure
 
 ./runpqnn64

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <xmmintrin.h>



#define	MATRIX		float*
#define	VECTOR		float*


typedef struct {
	char* filename; //
	MATRIX ds; // data set 
	MATRIX qs; // query set
	int n; // numero di punti del data set
	int d; // numero di dimensioni del data/query set
	int nq; // numero di punti del query set
	int knn; // numero di ANN approssimati da restituire per ogni query
	int m; // numero di gruppi del quantizzatore prodotto
	int k; // numero di centroidi di ogni sotto-quantizzatore
	int kc; // numero di centroidi del quantizzatore coarse
	int w; // numero di centroidi del quantizzatore coarse da selezionare per la ricerca non esaustiva
	int nr; // dimensione del campione dei residui nel caso di ricerca non esaustiva
	float eps; // 
	int tmin; //
	int tmax; //
	int exaustive; // tipo di ricerca: (0=)non esaustiva o (1=)esaustiva
	int symmetric; // tipo di distanza: (0=)asimmetrica ADC o (1=)simmetrica SDC
	int silent;
	int display;
	// nns: matrice row major order di interi a 32 bit utilizzata per memorizzare gli ANN
	// sulla riga i-esima si trovano gli ID (a partire da 0) degli ANN della query i-esima
	//
	int* ANN; 
	//
	// Inserire qui i campi necessari a memorizzare i Quantizzatori
	//
	MATRIX codebookp;
	MATRIX codebookc;
	double* ANN_values;
	int * map;

	//VALORI ESCLUSIVI DI NOT_EXAUSTIVE

	/** map qc(YR) */
	int * mapc;
	/** map qc(y) */
	int *mapyc;
	/** pq(ry) */
	int *pqy;
	//w centroidi associati a x
	int *mapxw; // nq(righe)*w(colonne)
	/** lista dei punti più vicini ai centroidi coarse, struttura:
	 * 
	 * (riga/centroide)j -> [ indice ds1....indice ds c, 0....0 , numero punti effettivamente presenti c ]
	 *
	 * matrice (n+1)xkc
	 * 
	 */
	int *codemap;
	/** x-qc(y) */
	MATRIX rx; // dim=nq*d*w
	/** y-qc(y) */
	MATRIX ry;

	// ...
	//
} params;

/*
 * 
 *	Le funzioni sono state scritte assumento che le matrici siano memorizzate 
 * 	mediante un array (float*), in modo da occupare un unico blocco
 * 	di memoria, ma a scelta del candidato possono essere 
 * 	memorizzate mediante array di array (float**).
 * 
 * 	In entrambi i casi il candidato dovrà inoltre scegliere se memorizzare le
 * 	matrici per righe (row-major order) o per colonne (column major-order).
 *
 * 	L'assunzione corrente è che le matrici siano in row-major order.
 * 
 */


void* get_block(int size, int elements) { 
	return _mm_malloc(elements*size,16); 
}


void free_block(void* p) { 
	_mm_free(p);
}

//aggiunto dal team9
#define ANGIULLI
#include "utility.c"

MATRIX alloc_matrix(int rows, int cols) {
	return (MATRIX) get_block(sizeof(float),rows*cols);
}


void dealloc_matrix(MATRIX mat) {
	free_block(mat);
}


/*
 * 
 * 	load_data
 * 	=========
 * 
 *	Legge da file una matrice di N righe
 * 	e M colonne e la memorizza in un array lineare in row-major order
 * 
 * 	Codifica del file:
 * 	primi 4 byte numero di colonne (M): --> numero intero a 32 bit
 * 	successivi 4 byte: numero di righe (N)  --> numero intero a 32 bit
 * 	successivi N*M*4 byte: matrix data in row-major order --> numeri floating-point a precisione doppia
 * 
 *****************************************************************************
 *	Se lo si ritiene opportuno, è possibile cambiare la codifica in memoria
 * 	della matrice. 
 *****************************************************************************
 * 
 */
MATRIX load_data(char* filename, int *n, int *d) {	
	FILE* fp;
	int rows, cols, status, i;
	
	fp = fopen(filename, "rb");
	
	if (fp == NULL) {
		printf("'%s' : bad data file name!\n", filename);
		exit(0);
	}
	
	status = fread(&cols, sizeof(int), 1, fp);
	status = fread(&rows, sizeof(int), 1, fp);
		
	MATRIX data = alloc_matrix(rows,cols);
	status = fread(data, sizeof(float), rows*cols, fp);
	fclose(fp);
	
	*n = rows;
	*d = cols;
	
	return data;
}


void save_ANN(char* filename, int* ANN, int nq, int knn) {	
	FILE* fp;
	int i, j;
	char fpath[256];
	
	sprintf(fpath, "%s.ann", filename);
	fp = fopen(fpath, "w");
	for (i = 0; i < nq; i++) {
		for (j = 0; j < knn; j++)
			fprintf(fp, "%d ", ANN[i*knn+j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}


// extern void pqnn64_index(params* input);
// extern int* pqnn64_search(params* input);


/*
 *	pqnn_index
 * 	============
 */
void pqnn_index(params* input) {
	
    // -------------------------------------------------
    // Codificare qui l'algoritmo di indicizzazione
    // -------------------------------------------------

	int n=input->n;
    if(! input->exaustive){
		n=input->nr;
		//calcolo codebook di partenza
		init_codebook(input->d,n,input->ds,input->kc,input->codebookc);

		//migliore codebook coarse
		k_means(input->d,1,input->eps,input->tmin,input->tmax,input->kc,input->codebookc,n,input->ds,input->mapc);

		//inizio notexaustive

		//migliore mappa per il codebook coarse
		pq(input->d,1,input->kc,input->codebookc,input->n,input->ds,input->mapyc);

		//calcolo residui y
		for(int i=0;i<input->n;i++){
			int icent=input->mapyc[i];
			//differenza(y,qc(y))
			DIFFVF(input->d,input->ds,i*input->d,input->codebookc,icent*input->d,input->ry,i*input->d); 
			
        }

		//calcolo pq(ry)
		pq(input->d,input->m,input->k,input->codebookp,input->n,input->ry,input->pqy);

		for(register int i=0;i<input->n;i++){
			//per ogni punto del dataset 
			int icent=input->mapyc[i];
			int c=input->codemap[LAST_INDEX(input->n,icent)]; // accede all'ultima cella di icent
			input->codemap[icent*input->n+c]=i;
			input->codemap[LAST_INDEX(input->n,icent)]=c+1;
    	}
		for (register int i =0; i<input->nq;i++){
			centroidi_associati(input->d,input->w,input->qs,i,input->kc,input->codebookc,input->mapxw);

			for(int j=0 ; j< input->w; j++) {
				int icent=input->mapxw[i*input->w+j]*input->d;

				DIFFVF(input->d,input->qs,i*input->d,input->codebookc,icent,input->rx,MATRIX3_INDEX(input->w,input->d,i,j,0));
			}

		}


	}// non esaustivo
	//totale
	init_codebook(input->d,n,input->ds,input->k,input->codebookp);

	
	k_means(input->d,input->m,input->eps,input->tmin,input->tmax,input->k,input->codebookp,n,input->ds,input->map);
    // -------------------------------------------------

}


/*
 *	pqnn_search
 * 	===========
 */
void pqnn_search(params* input) {
	
    // -------------------------------------------------
    // Codificare qui l'algoritmo di interrogazione
    // -------------------------------------------------
    

	if(input->exaustive){
		if(input->symmetric){
			//se simmetrica
			ANNSDC(input->d,input->m,input->k,input->codebookp,input->knn,input->ANN,input->ANN_values,input->n,input->map,input->nq,input->qs);
		}else{
			ANNADC(input->d,input->m,input->k,input->codebookp,input->knn,input->ANN,input->ANN_values,input->n,input->map,input->nq,input->qs);
		}
	}
	else 
		notExaustive(input);
	//la funzione scritta contiene i parametri già divisi, quindi va fatta una nuova funzione che chiami il metodo tante volte uno per ogni query

	// Restituisce il risultato come una matrice di nq * knn
	// identificatori associati agli ANN approssimati delle nq query.
	// La matrice è memorizzata per righe
    // -------------------------------------------------

}


int main(int argc, char** argv) {	
	char fname[256];
	int i, j;
	
	//
	// Imposta i valori di default dei parametri
	//

	params* input = malloc(sizeof(params));

	input->filename = NULL;
	input->exaustive = 1;
	input->symmetric = 1;
	input->knn = 1;
	input->m = 8;
	input->k = 256;
	input->kc = 150;
	input->w = 16;
	input->eps = 0.01;
	input->tmin = 10;
	input->tmax = 100;
	input->silent = 0;
	input->display = 0;
	input->nr=0;

	//
	// Legge i valori dei parametri da riga comandi
	//

	int par = 1;
	while (par < argc) {
		if (par == 1) {
			input->filename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-s") == 0) {
			input->silent = 1;
			par++;
		} else if (strcmp(argv[par],"-d") == 0) {
			input->display = 1;
			par++;
		} else if (strcmp(argv[par],"-knn") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing knn value!\n");
				exit(1);
			}
			input->knn = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-m") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing m value!\n");
				exit(1);
			}
			input->m = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-k") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing k value!\n");
				exit(1);
			}
			input->k = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-kc") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing kc value!\n");
				exit(1);
			}
			input->kc = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-w") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing w value!\n");
				exit(1);
			}
			input->w = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-nr") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing nr value!\n");
				exit(1);
			}
			input->nr = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-eps") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing eps value!\n");
				exit(1);
			}
			input->eps = atof(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-tmin") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing tmin value!\n");
				exit(1);
			}
			input->tmin = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-tmax") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing tmax value!\n");
				exit(1);
			}
			input->tmax = atoi(argv[par]);
			par++;
 		} else if (strcmp(argv[par],"-exaustive") == 0) {
 			input->exaustive = 1;
 			par++;
 		} else if (strcmp(argv[par],"-noexaustive") == 0) {
 			input->exaustive = 0;
 			par++;
 		} else if (strcmp(argv[par],"-sdc") == 0) {
 			input->symmetric = 1;
 			par++;
 		} else if (strcmp(argv[par],"-adc") == 0) {
 			input->symmetric = 0;
 			par++;
		} else
			par++;
	}
	
	//
	// Visualizza la sintassi del passaggio dei parametri da riga comandi
	//

	if (!input->silent) {
		printf("Usage: %s <data_name> [-d][-s][-exaustive|-noexaustive][-sdc|-adc][...]\n", argv[0]);
		printf("\nParameters:\n");
		printf("\t-d : display ANNs\n");
		printf("\t-s : silent\n");
		printf("\t-m: PQ groups\n");
		printf("\t-k: PQ centroids\n");
		printf("\t-kc: coarse VQ centroids\n");
		printf("\t-w: coarse VQ centroids to be selected\n");
		printf("\t-nr: residual sample size\n");
		printf("\t-eps: k-means termination threshold\n");
		printf("\t-tmin: min k-means iterations\n");
		printf("\t-tmax: max k-means iterations\n");
		printf("\n");
	}
	
	//
	// Legge il data set ed il query set
	//
	
	if (input->filename == NULL || strlen(input->filename) == 0) {
		printf("Missing input file name!\n");
		exit(1);
	}
	
	sprintf(fname, "%s.ds", input->filename);
	input->ds = load_data(fname, &input->n, &input->d);
	
	if(input->nr==0) input->nr = input->n/20;

	sprintf(fname, "%s.qs", input->filename);
	input->qs = load_data(fname, &input->nq, &input->d);

	//
	// Visualizza il valore dei parametri
	//
	
	if (!input->silent) {
		printf("Input file name: '%s'\n", input->filename);
		printf("Data set size [n]: %d\n", input->n);
		printf("Number of dimensions [d]: %d\n", input->d);
		printf("Query set size [nq]: %d\n", input->nq);
		printf("Number of ANN [knn]: %d\n", input->knn);
		printf("PQ groups [m]: %d\n", input->m);
		printf("PQ centroids [k]: %d\n", input->k);
		if (!input->exaustive) {
			printf("Coarse VQ centroids [kc]: %d\n", input->kc);
			printf("Coarse VQ centroids to be selected [w]: %d\n", input->w);
			printf("Number of residuals used to determine PQ centroids [nr]: %d\n", input->nr);
		}
		printf("K-means parameters: eps = %.4f, tmin = %d, tmax = %d\n", input->eps, input->tmin, input->tmax);
	}
	
	//
	// Costruisce i quantizzatori
	//
	input->codebookp=(float*)(get_block(sizeof(float),input->k*input->d));
	input->codebookc=(float*)(get_block(sizeof(float),input->kc*input->d));
	input->rx=(float*)(get_block(sizeof(float),input->nq*input->d*input->w));
	input->ry=(float*)(get_block(sizeof(float),input->n*input->d));
	input->map=(int*)(get_block(sizeof(int),input->m*input->n));
	input->mapc=(int*)(get_block(sizeof(int),input->m*input->nr));
	input->mapyc =(int*)(get_block(sizeof(int),input->n));
	input->pqy =(int*)(get_block(sizeof(int),input->m*input->n));
	input->mapxw =(int*)(get_block(sizeof(int),input->nq*input->w));
	input->codemap =(int*)(get_block(sizeof(int),(input->n+1)*input->kc));
	input->ANN_values=calloc(input->nq*input->knn,sizeof(double));
	input->ANN = calloc(input->nq*input->knn,sizeof(int));
	
	
	clock_t t = clock();
	pqnn_index(input);
	t = clock() - t;
	
	if (!input->silent)
		printf("\nIndexing time = %.3f secs\n", ((float)t)/CLOCKS_PER_SEC);
	else
		printf("%.3f\n", ((float)t)/CLOCKS_PER_SEC);

	//
	// Determina gli ANN
	//
	

	t = clock();
	pqnn_search(input);
	t = clock() - t;
	
	if (!input->silent)
		printf("\nSearching time = %.3f secs\n", ((float)t)/CLOCKS_PER_SEC);
	else
		printf("%.3f\n", ((float)t)/CLOCKS_PER_SEC);
	
	#ifdef DEBUG	
		printf("\n\n########STAMPA DEGLI INDICI!########\n");
		matprintmi(input->knn,input->nq,input->ANN);
		printf("\n\n########STAMPA VALORI!########\n");
		matprintmd(input->knn,input->nq,input->ANN_values);
	#endif
	
	//
	// Salva gli ANN
	//
	
 	if (input->ANN != NULL)
 	{
 		if (!input->silent && input->display) {
 			printf("\nANN:\n");
 			for (i = 0; i < input->nq; i++) {
				printf("query #%d:", i);
				for (j = 0; j < input->knn; j++)
					printf(" %d", input->ANN[i*input->knn+j]);
				printf("\n");
 			}
 		}
 		save_ANN(input->filename, input->ANN, input->nq, input->knn);
	}

	free_block(input->codebookp);
	free_block(input->codebookc);
	free_block(input->map);
	free_block(input->ANN_values);
	free_block(input->codemap);
	free_block(input->mapxw);
	free_block(input->pqy);
	free_block(input->mapyc);
	free_block(input->ry);
	free_block(input->rx);
	
	if (!input->silent)
		printf("\nDone.\n");

	return 0;
}
