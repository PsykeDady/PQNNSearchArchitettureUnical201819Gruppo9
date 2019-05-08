/*
 * File con i metodi che poi verranno spostati nei file che ci manderà il professore.
 */

//scaricrare gprof

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//decommentare per abilitare tutti i debug/decommentare per disabilitare tutti i debug
#define DEBUG

/**
 * lista di flag di debug, commentare/decommentare singolo flag per disabilitare/abilitare le stampe di debug del relativo metodo
 */
#ifdef DEBUG
    //#define DEBUG_DIST
    //#define DEBUG_PRINTV
    //#define DEBUG_PRINTM
    //#define DEBUG_COPYV
    #define DEBUG_MINDIST
    //#define DEBUG_VQ
    //#define DEBUG_INITCODEBOOK
    //#define DEBUG_OBIETTIVO
    //#define DEBUG_EQUALS
    //#define DEBUG_NUOVICENTROIDI
    //#define DEBUG_PQ
    //#define DEBUG_MAIN
#endif

//template debug da incollare ogni metodo:
/* #ifdef DEBUG_
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO '' #####\n");
#endif */


/**
 * argomenti:
 *  - 'd' dimensione elementi 
 *  - 'x' matrice di vettori in Rd
 *  - 'xi' indice iniziale di x
 *  - 'y' matrice di vettori in Rd
 *  - 'yi' indice iniziale di y 
 * descrizione:
 *  calcola la distanza euclidea tra un punto di x (segnato a partire da xi) e un punto di y (segnato a partire da yi).
 * A partire da xi e da yi vengono analizzati d elementi
 * 
 */
double dist(int d, double *x, int xi, double *y, int yi){

    #ifdef DEBUG_DIST 
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'dist' #####\n");
    #endif

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
    #ifdef DEBUG_PRINTV
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printv' #####\n");
    #endif

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
    #ifdef PRINTV
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printm' #####\n");
    #endif
    
    int i=0,j=0;
    for(;i<n;i++){
        printf("|");
        for(j=0;j<d-1;j++){
            printf("%+lf,",m[i*d+j]);
        }
        printf("%+lf|\n",m[i*d+d-1]);
    }
}

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
    #ifdef DEBUG_COPYV
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'copyv' #####\n");
    #endif
    for(int i=0;i<d;i++){
        dest[desti*d+i]=src[srci*d+i];
    }
}

/**
 * argomenti:       
 *  - 'd' dimensione dei singoli vettori
 *  - 'dstar' dimensione sotto vettori ( per algoritmo a sottovettori) 
 *  - 'mi' indice di colonna di partenza (per algoritmo a sottovettori pq)
 *  - 'dataset' insieme di punti da cui prelevare il punto query 
 *  - 'di' indice iniziale del punto query 
 *  - 'k' numero di centroidi nel codebook
 *  - 'codebook' insieme di centroidi (vettori di Rd) 
 * 
 * descrizione:
 *  considerato il punto indicizzato nel dataset a partire dall'indice di (in R^d) si cerca un centroide nel codebook (tra k centroidi) tale da minimizzare la distanza. Ogni distanza viene calcolata con la funzione dist
 * 
 * Funziona sia con sottovettori che non, per non usare i sottovettori porre dstar=d e mi=0
 */
int mindist(int d, int dstar, int mi, double *dataset, int di, int k, double* codebook){
    #ifdef DEBUG_MINDIST
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'mindist' #####\n");
    #endif

    #ifdef DEBUG_MINDIST
        printf("punto dataset preso in carico:%i\n",di);
    #endif
    int i,ris=0, j=di*d+mi*dstar;
    #ifdef DEBUG_MINDIST
        printf("indice globale per dizionario=\ndi*d+mi*dstar=%i*%i+%i*%i=%i\n",di,d,mi,dstar,j);
    #endif
    float mind,ndist;

    mind=dist(dstar,dataset,j,codebook,0);
    #ifdef DEBUG_MINDIST
        printf("\ndistanza con centroide 0=%f\n",mind);
    #endif

    for(i=1;i<k;i++){
        ndist=dist(dstar,dataset,j,codebook,d*i+mi*dstar);
        #ifdef DEBUG_MINDIST
            printf("distanza con centroide %i=%f\n",i,ndist);
        #endif
        if(mind>ndist){
            mind=ndist;
            ris=i;
        }
    }
    #ifdef DEBUG_MINDIST
        printf("\n");
    #endif
    return ris;
}

/**
 * argomenti:
 * -d=dimensione singoli vettori
 * -k=numero di centroidi
 * -n=dimensione dataset
 * -codebook=insieme centroidi
 * -dataset=insieme di punti 
 * 
 * descrizione:
 * -restituisce un insieme della cardinalita' del dataset*2 dove ad ogni punto viene associato il suo centroide (ogni valore viene diviso in una coppia quindi, punto-centroide), ogni centroide viene calcolato con la funzione di minima distanza (mindist)
 * TODO: obsoleto, può essere riscritto come pq con
 */
/* double* vq(int d, int k, int n, double *codebook, double *dataset){
    #ifdef DEBUG_VQ
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'vq' #####\n");
    #endif
    /* 
    passi dell'algoritmo
        - creare una struttura dataset=2*dataset
        - prendi un elemento del dataset
        - cercare il centroide (mindist)
        -aggiungere al risultato dataset(i),centroide(dataset(i))
    
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
} */

/**
 * argomenti:
 *  -d=dimensione spazio vettoriale
 *  -n= dimensione dataset
 *  -dataset= insieme di punti in Rd
 *  -k= numero di centroidi
 *  -codebook= codebook da riempire
 * 
 * descrizione:
 * -si riempie il codebook con valori random facenti parte dell'insieme di partenza
 */
void init_codebook(int d, int n, double* dataset, int k, double* codebook){
    #ifdef DEBUG_INITCODEBOOK
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'init_codebook' #####\n");
    #endif
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
 *  -calcola il valore della funzione obiettivo attuale come 'la somma delle distanze al quadrato di ogni punto dal suo centroide'
 */
double obiettivo(int d, int n, double* dataset, double *map){
    #ifdef DEBUG_OBIETTIVO
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'obiettivo' #####\n");
    #endif
    int i;
    double somma=0,parziale;
    for(i=0;i<n;i++){
        parziale=dist(d,dataset,i*d,map,i*d); 

        /**
         * for (w per tutti i sottovettori){
         *      dist(dstar,w,dataset,i,codebook,map[i]);
         * } POSSIBILE OTTIMIZZAZIONE
         * 
         */

        parziale*=parziale;
        somma+=parziale;
    }
    return somma;
}


/**
 * argomenti:
 * -d: dimensione degli elementi da controllare per ogni vettore
 * -*x: primo vettore
 * -xi: indice inizio da cui controllare gli elementi di x
 * -*y: secondo vettore 
 * -yi: indice inizio da cui controllare gli elementi di 
 * 
 * descrizione:
 * - verifica se due vettori x e y, a partire ognuno da un certo indice indicato (xi/yi rispettivamente), sono uguali per ogni elemento
 * restituisce 0 se sono diversi, 1 se sono uguali
 */
int equals(int d, double *x, int xi, double *y, int yi){
    #ifdef DEBUG_EQUALS
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'equals' #####\n");
    #endif
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
 * - genera un set di nuovi centroidi calcolando la media degli elementi che gli sono associati
 */
void nuovicentroidi(int d, int n, double *map, int k, double *codebook){
    #ifdef DEBUG_NUOVICENTROIDI
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'nuovicentroidi' #####\n");
    #endif
    /*
    -per ogni centroide, media dei suoi punti
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
       for(w=0;w<d;w++){
            codebook[i*d+w]=nc[w]/c;
       }
   }
}

/**
 * argomenti:
 * -d=dimensione singoli vettori
 * -m=numero di sottovettori
 * -k=numero di centroidi
 * -codebook=insieme centroidi
 * -n=dimensione dataset
 * -dataset=insieme di punti
 * 
 * descrizione:
 * -TODO
 * 
 * 
 * OTTIMIZZAZIONE:
 *  - si può restituire la mappa come raccolta di indici del codebook
 * 
 */
double* pq(int d, int m, int k, double *codebook, int n, double *dataset){
    #ifdef DEBUG_PQ
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'pq' #####\n");
    #endif

    /* 
    passi dell'algoritmo
        - creare una struttura result set, ogni riga sarà associata alla riga corrispondente del dataset, le colonne rappresentano le coordinate del centroide associato
        - prendi un elemento del dataset
        - si divide in m sottovettori di dimensione dstar
        - per ogni sottovettore, si cerca un sottocentroide (mindist) più vicino
        -trovati gli m sottovettori, vanno concatenati per ottenere la quantizzazione del vettore
        -aggiungere al risultato dataset(i),centroide(dataset(i))
    */

   int dstar=d/m; //numero elementi sottovettori
    int i,j,w;
    double *res=(double*)malloc(sizeof(double) * n*d);
    for(i=0;i<n;i++){ // indice di riga
        #ifdef DEBUG_PQ
            printf("riga dataset:%i\n",i);
        #endif 
        for(w=0;w<m;w++){ // indice di sottovettore
            #ifdef DEBUG_PQ
                printf("sottovettore w:%i\n",w);
            #endif
            int icent=mindist(d,dstar,w,dataset,i,k,codebook);
            #ifdef DEBUG_PQ
                printf("centroide scelto: %i\n\n",icent);
            #endif 
            //sappiamo  l'indice di codebook del punto più vicino  al sottovettore 
            int mi=w*dstar; 
            for(j=0;j<dstar;j++){
                res[i*d+mi+j]=codebook[icent*d+mi+j];
            }//j
        }//w
       
   }//i

   return res;
}





void k_means( int d, int m, float eps, int tmin, int tmax, int k, double* codebook, int n, double* dataset, double* res){

    /*
     1- avendo codebook i centroidi e il dataset, crea una mappa che ad ogni elemento del dataset, associa un centroide (vq- quantizzazione)
     2- calcola la funziona la obiettivo
     3- delta=obiettivo(old)-obiettivo(nuovo)
     4- condizione di terminazione: (tmin<=t && (tmax<t || delta<=eps)), se vera termina, sennò avanti
     5- sostituisce i centroidi con la media
     6- goto 2
    */

    double tmp=0;
    int t=0;
    double delta=0;//per non uscire la prima volta dal secondo while
    
    double ob=-1; /*valore molto grande, così da non intaccare i risultati*/
    double nuovo_ob=0;

    

    while(t++<tmin-1){
        res=pq(d,m,k,codebook,n,dataset);
        nuovicentroidi(d,n,res,k,codebook); 
    }
    ob=obiettivo(d,n,dataset,res);
    //l'ultima volta il ciclo lo facciamo al di fuori del while
    res=pq(d,m,k,codebook,n,dataset);
    nuovo_ob=obiettivo(d,n,dataset,res);
    delta =ob-nuovo_ob;
        
    // abbiamo fatto il numero minimo di passi, andiamo alla seconda condizione
    while(tmax>=t++ && delta>eps ){
        nuovicentroidi(d,n,res,k,codebook); 
        res=pq(d,m,k,codebook,n,dataset);
        nuovo_ob= obiettivo(d,n,dataset,res);
        delta =ob-nuovo_ob;
        ob=nuovo_ob;
    }

}


int main (char*args, int argv){
    /**dimensione dei singoli punti (vettori)*/
    int d;
    /**numero di sottovettori*/
    int m;
    /** il data set */
    double *dataset;
    /** la dimensione n del dataset*/
    int n;
    /**codebook*/
    double *codebook;
    /**la dimensione k del codebook*/
    int k;

    /**variabili temporanee*/
    int j;
    int i;
    double *res;


    srand(time(NULL));
    
    printf("inserisci d:>");
    scanf("%i",&d);

    printf("inserisci n:>");
    scanf("%i",&n);

    printf("inserisci k:>");
    scanf("%i",&k);

    dataset= (double*) malloc(d*n*sizeof(double));
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
    printm(d,k,codebook);

    printf("inserisci m:>");
    scanf("%i",&m);


    printf("applicando product quantization\n");

    res=pq(d,m,k,codebook,n,dataset);

    /* res=vq(d,k,n,codebook,dataset);
    printf("ordunque il tuo res è uscito:\n");
    printm(2*d,n,res);

    printf("calcolando nuovi centroidi...\n");
    nuovicentroidi(d,n,res,k,codebook);

    printf("ecco i nuovi centroidi che sono usciti:\n");
    printv(d*k,codebook,0); */

    printf("ecco il risultato\n");
    printm(d,n,res);



    free(dataset);
    free(codebook);
    free(res);
}

