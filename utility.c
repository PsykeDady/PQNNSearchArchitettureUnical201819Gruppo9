/*
 * File con i metodi che poi verranno spostati nei file che ci manderà il professore.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
    //#define DEBUG_DIST
#endif

/**
 * argomenti:
 *  -'d' numero elementi da copiare
 *  -'dest' vettore destinazione
 *  -'desti' indice iniziale di dest
 *  -'src' vettore sorgente
 *  -'srci' indice iniziale vettore sorgente
 * descrizione: 
 *  copia d elementi dal vettore src ( a partire dall'indice srci) sul vettore dest ( a partire dall'indice desti ) 
 */
void copyv(int d, double* dest, int desti, double *src, int srci){
    for(int i=0;i<d;i++){
        dest[desti*d+i]=src[srci*d+i];
    }
}

/**
 * argomenti:
 *  - 'd' dimensione elementi 
 *  - 'x' matrice di vettori in Rd
 *  - 'xi' indice iniziale di x
 *  - 'y' matrice di vettori in Rd
 *  - 'yi' indice iniziale di y 
 * descrizione:
 *  calcola la distanza euclidea tra un punto di x (segnato a partire da xi) e un punto di y (segnato a partire da yi).
 * 
 * A partire da xi e da yi vengono analizzati d elementi
 * 
 */
double dist(int d,double *x, int xi, double *y, int yi){

    double somma=0, differenza=0;
    for(int i=0;i<d;i++){
        differenza=x[xi+i]-y[yi+i];
        #ifdef DEBUG_DIST
            printf("differenza: %lf\n",differenza);
        #endif
        differenza*=differenza;
        #ifdef DEBUG_DIST 
            printf("al quadrato %lf\n",differenza);
        #endif
        somma+=differenza;
        #ifdef DEBUG_DIST
            printf("somma parziale %lf\n",somma);
        #endif
    }

    #ifdef DEBUG_DIST
        printf("somma completa %lf\n",somma);
        printf("con la radice %lf\n",sqrt(somma));
    #endif

    return sqrt(somma);
}

/**
 * argomenti:
 *  - 'd' numero di celle da stampare
 *  - 'v' vettore da cui prelevare le celle
 *  - 'vi' indice iniziale di v
 * descrizione:
 *  stampa d celle del vettore v passato come parametro a partire da vi
 */
void printv(int d,double *v,int vi){
    printf("[");
    for(int i=0; i< d-1;i++){
        printf("%lf,",v[i]);
    }
    printf("%lf",v[d-1]);
    printf("]\n");
}

/**
 * argomenti:
 *  - 'd' dimensione di ogni vettore
 *  - 'n' numero vettori
 *  - 'm' matrice da stampare
 * descrizione:
 *  stampa un intera matrice divisia in n vettori e ognuno di dimensione d
 */
void printm(int d, int n, double *m){
    int i=0,j=0;
    for(;i<n;i++){
        printf("|");
        for(j=0;j<d-1;j++){
            printf("%lf,",m[i*d+j]);
        }
        printf("%lf|\n",m[i*d+d-1]);
    }
}

/**
 * argomenti:
 *  - 'd' dimensione dei singoli vettori
 *  - 'dataset' insieme di punti da cui prelevare il punto query 
 *  - 'di' indice iniziale del punto query 
 *  - 'k' numero di centroidi nel codebook
 *  - 'codebook' insieme di centroidi (vettori di Rd) 
 * 
 * descrizione:
 *  considerato l'elemento indicizzato in dataset a partire dall'indice di (con size d) si  
 */
int mindist(int d, double *dataset, int di, int k, double* codebook){

    int i,ris=0;
    float mind,ndist;

    mind=dist(d,dataset,di,codebook,0);


    for(i=1;i<k;i++){
        ndist=dist(d,dataset,di,codebook,i*d);
        if(mind>ndist){
            mind=ndist;
            ris=i;
        }
    }

    return ris;
}

/**
 * restituisce un insieme della cardinalita' del dataset*2 dove ad ogni punto viene associato il suo centroide (ogni valore viene diviso in una coppia quindi, punto-centroide)
 * 
 * n=dimensione dataset
 * k=numero di centroidi
 * d=dimensione singoli vettori
 * codebook=insieme centroidi
 * dataset=insieme di punti
 */
double* vq(int d, int k, int n, double *codebook, double *dataset){
    /*
    - creare una struttura dataset=2*dataset
    - prendi un elemento del dataset
    - cercare il centroide (mindist)
    - aggiungere al risultato dataset(i),centroide(dataset(i))
    */
   int i,j;
   double *res=(double*)malloc(sizeof(double) * n*2*d);
   for(i=0;i<n;i++){
       int icent=mindist(d,dataset,i*d,k,codebook);
       for(j=0;j<d;j++){
           res[(i*2*d)+j]=dataset[(i*d)+j];
           res[(i*2*d)+d+j]=codebook[(icent*d)+j];
        }
   }

   return res;
}

/**
 * dati:
 *  d=dimensione spazio vettoriale
 *  n= dimensione dataset
 *  dataset= insieme di punti in Rd
 *  k= numero di centroidi
 *  codebook= codebook da riempire
 * 
 * si riempie il codebook con valori random
 */
void init_codebook(int d, int n, double* dataset, int k, double* codebook){
    int i=0,j=0,flag=1,r=0;
    int *rp=(int*)malloc(sizeof(int)*k);

    for(;i<k;i++){        
        do{
            flag=1;
            r= (int)(rand()%n); 
            for(j=0;j<i;j++) {
                if(r==rp[j])
                {
                    flag=0;
                }
            }//for j
        }while(flag==0); //do flag
        rp[i]=r;
        copyv(d,codebook,i,dataset,r);
    }//for i
}

/**
 * argomenti:
 *  - 'd' coordinate del singolo punto
 *  - 'n' numero di righe della map (quindi la map contiene n*d*2 elementi)
 *  - 'map' una matrice che contiene per ogni riga un punto del dataset e il relativo centroide
 * 
 * descrizione:
 *  
 */
double obiettivo(int d, int n, double *map){
    int i,j;
    double somma=0,parziale,yi,qci;
    for(i=0;i<n;i++){
        parziale=dist(d,map,i*d,map,d*(i+1));
        parziale*=parziale;
        somma+=parziale;
    }
    return somma;
}
//verifica du evettori uguali
int equals(int d, double *x, int xi, double *y, int yi){
    int i=0,flag=0;
    
    for(i=0; i<d&&flag==0; i++){
        flag=(x[xi*d+i]!=y[yi*d+i])?1:0;
    }

    return flag; //ritorna 0 se uguali, 1 se diversi
}

/**
 * argomenti:
 *  - 'd' numero di coordinate per punto
 *  - 'n' numero di elementi del dataset (righe di map)
 *  - 'map' mappa di corrispondenza punto-centroide associato
 *  - 'k' numero di centroidi
 *  - 'codebook' lista di centroidi
 * descrizione:
 * 
 */
void nuovicentroidi(int d, int n, double *map, int k, double *codebook){
    /*
    -per ogni centroide, media geometrica dei suoi punti
    -sostituire elementi del vecchio codebook
    */
   int i,j,w,c,flag;
   double *nc=(double*)malloc(sizeof(double)*d); //vettore da mettere al posto di ci (tutto pari ad 1)
   for(i=0; i<k; i++){
        for(w=0;w<d;w++) nc[w]=0;
        c=0;
        for(j=0;j<n;j++){
            flag=equals(d,codebook,i,map,j*2+1);
            
            if(flag==0){
                for(w=0;w<d;w++){
                    nc[w]+=map[j*2*d+w];
                }
                c++;
            }
        }
       //a questo punto nc è completo, va solo fatta la radice d-esima e sostituita a codebook
       double esp=1.0/c;
       
       
       for(w=0;w<d;w++){
            codebook[i*d+w]=nc[w]/c;
       }
   }
}


int main (char*args, int argv){
    /**dimensione dei singoli punti (vettori)*/
    int d;
    /** il data set */
    double *dataset;
    /** la dimensione n del dataset*/
    int n;
    /**codebook*/
    double *codebook;
    /**la dimensione k del codebook*/
    int k=2;

    /**variabili temporanee*/
    int j;
    int i;
    double *res;


    srand(time(NULL));
    
    printf("dai un numero di elementi:>");
    scanf("%i",&d);

    printf("dai un numero di vettori:>");
    scanf("%i",&n);

    dataset=(double*) malloc(d*n*sizeof(double));
    codebook=(double*)malloc(k*d*sizeof(double));

    printf("Riempiamo il set di punti\n");
    for(i=0; i<n;i++) {
        printf("\n\npunto numero %i:\n",i);
        for(j=0;j<d;j++){
            printf("dai un numero dataset:> ");
            scanf("%lf",&dataset[i*d+j]);
        }
    }
    printf("riepilogando hai inserito dataset:\n"); 
    printm(d,n,dataset);

    printf("creando il codebook...\n");
    init_codebook(d,n,dataset,k,codebook);
    printv(d*k,codebook,0);

    res=vq(d,k,n,codebook,dataset);
    printf("ordunque il tuo res è uscito:\n");
    printm(2*d,n,res);

    printf("calcolando nuovi centroidi...\n");
    nuovicentroidi(d,n,res,k,codebook);

    printf("ecco i nuovi centroidi che sono usciti:\n");
    printv(d*k,codebook,0);

    free(dataset);
    free(codebook);
    free(res);
}

