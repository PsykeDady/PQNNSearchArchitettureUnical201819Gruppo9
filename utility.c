/*
 * File con i metodi che poi verranno spostati nei file che ci manderà il professore.
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>


extern float dist_2_asm(int d, float *x, int xi, float *y, int yi);
extern void diffvf_asm(int d, float *x, int xi, float *y, int yi,float * res, int ri);
extern void azzera_array(int d, float*v);
extern void copyv_asm(int d, float* dest, int desti, float *src, int srci);


//decommentare per abilitare tutti i debug/decommentare per disabilitare tutti i debug
//#define DEBUG 

/**
 * lista di flag di debug, commentare/decommentare singolo flag per disabilitare/abilitare le stampe di debug del relativo metodo
 */
#ifdef DEBUG
    #include "printutility.c"
    //#define DEBUG_DIST2
    //#define DEBUG_DIST
    //#define DEBUG_DIFFVF
    //#define DEBUG_COPYV
    //#define DEBUG_MINDIST
    //#define DEBUG_INITCODEBOOK
    //#define DEBUG_OBIETTIVO
    //#define DEBUG_NUOVICENTROIDI
    //#define DEBUG_PQ
    //#define DEBUG_KMEANS
    //#define DEBUG_CINDEX
    //#define DEBUG_MERGE
    //#define DEBUG_MERGESORT
    //#define DEBUG_SDC
    //#define DEBUG_ANNSDC
    //#define DEBUG_ADC
    //#define DEBUG_ANNADC
    //#define DEBUG_CENTROIDIASSOCIATI
    //#define DEBUG_NOTEXAUSTIVE
#endif

#ifndef ANGIULLI
    #define MATRIX float*
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
#endif


//#### LISTA MACRO ####

/**
 * args:
 * -K = numero colonne
 * -M = numero larghezza (numero celle in cella)
 * -I = indice riga
 * -J = indice colonna
 * -W = indice tridimensionale
 * 
 * descr:
 * -calcola l'indice di una matrice cubica vettorizzata : I*K*M+J*M+W
 */
#define MATRIX3_INDEX(K,M,I,J,W) (I)*(K)*(M)+(J)*(M)+(W)
/** 
 * args:
 * -M = numero colonne
 * -I = indice riga
 * -J = indice colonna
 * 
 * descr:
 * -calcola l'indice di accesso in una matrice quadrata vettorizzata : M*I+J
 */
#define MATRIX2_INDEX(M,I,J) (M)*(I)+(J)
/** indice ultima cella riga i-esima di una matrice con larghezza d : ((I+1)*D-1)*/
#define LAST_INDEX(D,I) (((I)+1)*(D)-1)
/** calcolo delta */
#define DELTA(OLD,NEW) (float)(abs(OLD-NEW)/OLD)

#define ASSEGNA_BLOCCO(TYPE,SIZE) (TYPE*)(malloc(sizeof(TYPE)*(SIZE)))

//#### LISTA METODI-MACRO ####
#ifdef ASM
    /**
     * args:
     * -D   = numero di elementi per vettore
     * -X   = matrice primo vettore
     * -XI  = indice di riga di X
     * -Y   = matrice secondo vettore
     * -YI  = indice di riga Y
     * -RIS = variabile dove inserire il risultato
     * 
     * descr:
     * -calcola la distanza euclidea al quadrato tra il vettore nella riga XI di dimensione D  di X e quello di Y in riga YI, il risultato lo mette in RIS
     */
    #define DIST_E_2(D,X,XI,Y,YI,RIS) RIS=dist_2_asm(D,X,XI,Y,YI);

#else
    /**
     * args:
     * -D   = numero di elementi per vettore
     * -X   = matrice primo vettore
     * -XI  = indice di riga di X
     * -Y   = matrice secondo vettore
     * -YI  = indice di riga Y
     * -RIS = variabile dove inserire il risultato
     * 
     * descr:
     * -calcola la distanza euclidea al quadrato tra il vettore nella riga XI di dimensione D  di X e quello di Y in riga YI, il risultato lo mette in RIS
     */
    #define DIST_E_2(D,X,XI,Y,YI,RIS) \
    {float somma=0,\
    differenza=0;\
    for(register int indice_distanza=0;indice_distanza<D;indice_distanza++){\
            differenza=X[(XI)+indice_distanza]-Y[(YI)+indice_distanza];\
            differenza*=differenza;\
            somma+=differenza;\
        }\
    RIS=somma;}\
    
#endif

#ifdef ASM
    /**
     * args:
     * -D   = numero di elementi per vettore
     * -X   = matrice primo vettore
     * -XI  = indice di riga di X
     * -Y   = matrice secondo vettore
     * -YI  = indice di riga Y
     * -RIS = variabile dove inserire il risultato
     * 
     * descr:
     * -calcola la differenza tra D elementi dei vettori X e Y a partire dall'indice XI e YI (rispettivamente) e salva il risultato in RES a partire dall'indice RI
     */
    #define DIFFVF(D,X,XI,Y,YI,RES,RI) diffvf_asm(D,X,XI,Y,YI,RES,RI);
   
#else
    /**
     * args:
     * -D   = numero di elementi per vettore
     * -X   = matrice primo vettore
     * -XI  = indice di riga di X
     * -Y   = matrice secondo vettore
     * -YI  = indice di riga Y
     * -RIS = variabile dove inserire il risultato
     * 
     * descr:
     * -calcola la differenza tra D elementi dei vettori X e Y a partire dall'indice XI e YI (rispettivamente) e salva il risultato in RES a partire dall'indice RI
     */
     #define DIFFVF(D,X,XI,Y,YI,RES,RI)\
        for(register int indice_diffvf=0;indice_diffvf<D;indice_diffvf++)\
            RES[(RI)+indice_diffvf]=X[(XI)+indice_diffvf]-Y[(YI)+indice_diffvf];

#endif

/**
 * args:
 * -d        = numero di elementi per vettore
 * -m        = numero di sottovettori
 * -n        = numero elementi del dataset
 * -dataset  = set di dati
 * -map      = mappa di corrispondenza dataset-centroide associato
 * -k        = numero elementi del codebook
 * -codebook = set di centroidi
 * 
 * descr:
 * -step ripetitivo all'interno del KMEANS, genera nuovi centroidi sulla media dei punti aggregati ad ogni centroide o inserisce una serie di zeri sui centroidi che non hanno alcun punto associato
 */
#define KMEANS_STEP(d,m,n,dataset,map,k,codebook)\
nuovicentroidi(d,m,n,dataset,map,k,codebook);\
pq(d,m,k,codebook,n,dataset,map);

#ifdef ASM
    /**
     * args:
     * -D  = numero elementi per vettore
     * -A  = vettore da inizializzare
     * -AI = indice iniziale di A
     * -IV = elemento con cui inizializzare il vettore
     * 
     * descr:
     * -inserisce il valore IV in D elementi di A a partire dall'indice AI
     */
    #define INIT_ARRAY(D,A,AI,IV) azzera_array(D,A);
#else
    /**
     * args:
     * -D  = numero elementi per vettore
     * -A  = vettore da inizializzare
     * -AI = indice iniziale di A
     * -IV = elemento con cui inizializzare il vettore
     * 
     * descr:
     * -inserisce il valore IV in D elementi di A a partire dall'indice AI
     */
    #define INIT_ARRAY(D,A,AI,IV)\
        for( register int init_index=0; init_index<d; init_index++){\
            A[(AI)+init_index]=IV;\
        }
#endif

#ifdef ASM
    /**
     * args:
     * -D     = numero di elementi per vettore
     * -DEST  = vettore destinazione
     * -DESTI = indice di riga di DEST
     * -SRC   = vettore sorgente
     * -SRCI  = indice di riga SRC
     * 
     * descr:
     * -copia D elementi dal vettore SRC al vettore DEST partendo dagli indici SRCI e DESTI rispettivamente
     */
    #define COPYV(D,DEST,DESTI,SRC,SRCI) copyv_asm(D,DEST,DESTI*D,SRC,SRCI*D);
#else 
    /**
     * args:
     * -D     = numero di elementi per vettore
     * -DEST  = vettore destinazione
     * -DESTI = indice di riga di DEST
     * -SRC   = vettore sorgente
     * -SRCI  = indice di riga SRC
     * 
     * descr:
     * -copia D elementi dal vettore SRC al vettore DEST partendo dagli indici SRCI e DESTI rispettivamente
     */
    #define COPYV(D,DEST,DESTI,SRC,SRCI)\
        for(int indicecopia=0;indicecopia<D;indicecopia++) DEST[(DESTI)*(D)+indicecopia]=SRC[(SRCI)*(D)+indicecopia];
#endif




//template debug da incollare ogni metodo:
/* #ifdef DEBUG_
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO '' #####\n");
#endif */



/**
 * argomenti:       
 *  -d        = dimensione dei singoli vettori
 *  -dstar    = dimensione sotto vettori ( per algoritmo a sottovettori) 
 *  -mi       = indice di colonna di partenza (per algoritmo a sottovettori pq)
 *  -dataset  = insieme di punti da cui prelevare il punto query 
 *  -di       = indice iniziale del punto query 
 *  -k        = numero di centroidi nel codebook
 *  -codebook = insieme di centroidi (vettori di Rd) 
 * 
 * descr:
 *  -considerato il punto indicizzato nel dataset a partire dall'indice di (in R^d) si cerca un centroide nel codebook (tra k centroidi) tale da minimizzare la distanza. Ogni distanza viene calcolata con la funzione dist. 
 * Funziona sia con sottovettori che non, per non usare i sottovettori porre dstar=d e mi=0
 * 
 * return:
 * -l'indice nel codebook del centroide più vicino al punto di del dataset
 */
int mindist(int d, int dstar, int mi, float *dataset, int di, int k, float* codebook){
    #ifdef DEBUG_MINDIST
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'mindist' #####\n");
    #endif

    #ifdef DEBUG_MINDIST
        printf("punto dataset preso in carico(pt%i):\n",mi+1);
        printfv(dstar,dataset,di*d+mi*dstar);
        printf("\n");
    #endif
    int i,ris=0, j=di*d+mi*dstar;
    #ifdef DEBUG_MINDIST
        printf("indice globale per dizionario= di*d+mi*dstar=%i*%i+%i*%i=%i\n",di,d,mi,dstar,j);
    #endif
    float mind,ndist;

    //mind=dist(dstar,dataset,j,codebook,mi*dstar);
    DIST_E_2(dstar,dataset,j,codebook,mi*dstar,mind);
    #ifdef DEBUG_MINDIST
        printf("centroide 0 (indice %i) preso in carico:\n",mi*dstar);
        printfv(dstar,codebook,mi*dstar);
        printf("\n");
        printf("distanza=%f\n",mind);
    #endif

    for(i=1;i<k;i++){
        //ndist=dist(dstar,dataset,j,codebook,d*i+mi*dstar);
        DIST_E_2(dstar,dataset,j,codebook,d*i+mi*dstar,ndist);
        #ifdef DEBUG_MINDIST
            printf("centroide %i (indice %i) preso in carico:\n",i,d*i+mi*dstar);
            printfv(dstar,codebook,d*i+mi*dstar);
            printf("\n");
            printf("distanza=%f\n",ndist);
        #endif
        if(mind>ndist){
            mind=ndist;
            ris=i;
        }
    }

    #ifdef DEBUG_MINDIST
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'mindist' #####\n");
    #endif
    return ris;
}



/**
 * argomenti:
 *  -d        = dimensione spazio vettoriale
 *  -n        = dimensione dataset
 *  -dataset  = insieme di punti in Rd
 *  -k        = numero di centroidi
 *  -codebook = codebook da riempire
 * 
 * descrizione:
 * -si riempie il codebook con i primi k valori facenti parte del datase di partenza
 */
void init_codebook(int d, int n, float* dataset, int k, float* codebook){
    #ifdef DEBUG_INITCODEBOOK
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'init_codebook' #####\n");
    #endif

    int i=0,j=0;
    for(i=0; i<k && i<n ;i++){
        COPYV(d,codebook,i,dataset,i);
    }//for i
    //nel caso k>n
    for(; i<k ;i++){        
        COPYV(d,codebook,i,dataset,n-1);
    }//for i

    #ifdef DEBUG_INITCODEBOOK
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'init_codebook' #####\n");
    #endif
}


/**
 * argomenti:
 *  -d   = coordinate del singolo punto
 *  -n   = numero di righe della map 
 *  -map = una matrice che contiene per ogni riga l'indice del relativo centroide
 * 
 * descrizione:
 * -calcola il valore della funzione obiettivo attuale come 'la somma delle distanze al quadrato di ogni punto dal suo centroide'
 * 
 * return:
 * -il valore dell'obiettivo
 */
double obiettivo(int d, int m, int n, float* dataset, int *map,  float* codebook){
    #ifdef DEBUG_OBIETTIVO
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'obiettivo' #####\n");
    #endif

 
    int i,icent,w,dstar=d/m;
    double smm=0,parziale;
    /* for(i=0;i<n;i++){
        parziale=dist_q(d, m, dataset,i,map,codebook);
        somma+=parziale;
    } */
    for(i=0;i<n;i++){
        for(w=0;w<m;w++){
            icent=map[MATRIX2_INDEX(m,i,w)];
            DIST_E_2(dstar,dataset,MATRIX2_INDEX(d,i,w*dstar),codebook,MATRIX2_INDEX(d,icent,w*dstar),parziale);

        }
    }
    #ifdef DEBUG_OBIETTIVO
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'obiettivo' #####\n");
    #endif
    return smm;
}

/**
 * argomenti:
 *  - 'd'        = numero di coordinate per punto
 *  - 'm'        = numero di sottovettori
 *  - 'n'        = numero di elementi del dataset (righe di map)
 *  - 'dataset'  = insieme di punti
 *  - 'map'      = mappa di corrispondenza punto-centroide associato 
 *  - 'k'        = numero di centroidi
 *  - 'codebook' = lista di centroidi
 * descrizione:
 * - genera un set di nuovi centroidi calcolando la media degli elementi che gli sono associati. Se un centroide non ha punti associati, gli viene dato tutto 0
 */
void nuovicentroidi (int d, int m, int n, float* dataset, int* map, int k, float* codebook){

    #ifdef DEBUG_NUOVICENTROIDI
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'nuovicentroidi' #####\n");
    #endif

    
    /*
     * - per ogni sottovettore del codebook, comparare un sotto vettore della mappa
     * - se uguali sommali al nuovo codebook il dataset in indice corrispondete, aumenta contatore
     * -dividi per n elementi sommati
     * -cambia sottovettore
     * 
     */
    int i,icent,z,w,c,j,dstar=d/m;
    float*nc=ASSEGNA_BLOCCO(float,d);

    for( i=0; i<k; i++){ // per ogni centroide
        #ifdef DEBUG_NUOVICENTROIDI
            printf("analisi centroide i=%i\n",i);
        #endif
        
        INIT_ARRAY(d,nc,0,0);

        for (w=0; w<m; w++){ // per ogni sottocentroide
            #ifdef DEBUG_NUOVICENTROIDI
                printf("in analisi sottocentroide w=%i\n",w);
            #endif
            c=0;
            for(j=0;j<n;j++){ // per ogni elemento della mappa
                icent=map[m*j+w];
                #ifdef DEBUG_NUOVICENTROIDI
                    printf("in analisi elemento della mappa j=%i\ncon valore di c=%i\nicent=%i\n",j,c,icent);
                #endif
                if (icent==i){
                    #ifdef DEBUG_NUOVICENTROIDI
                        printf("WOW icent e' uguale ad i, %i=%i\n",icent,i);
                    #endif
                    for(z=0;z<dstar;z++){
                        nc[w*dstar+z]+=dataset[j*d+w*dstar+z];
                    }
                    c++;
                    #ifdef DEBUG_NUOVICENTROIDI
                        printf("nc dopo la somma parziale= ");
                        printfv(d,nc,0);
                        printf("c aggioranto: %i\n",c);
                    #endif
                }
            }
            if(c==0) c=1;
            for(z=0;z<dstar;z++){
                codebook[i*d+w*dstar+z]=nc[w*dstar+z]/c;
            }//z
        }//w
    }//i

    #ifdef DEBUG_NUOVICENTROIDI
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'nuovicentroidi' #####\n");
    #endif
    free(nc);
}

/**
 * argomenti:
 * -d        = dimensione singoli vettori
 * -m        = numero di sottovettori
 * -k        = numero di centroidi
 * -codebook = insieme centroidi
 * -n        = dimensione dataset
 * -dataset  = insieme di punti
 * -map      = matrice di indici interi di dimensione n a cui associare per ogni cella un indice del codebook
 * 
 * descrizione:
 * -viene creata un associazione tra elementi del dataset e codebook memorizzando i sotto centroidi più vicini per ogni punto
 * 
 */
void pq(int d, int m, int k, float *codebook, int n, float *dataset, int*map){
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
    
    for(i=0;i<n;i++){ // indice di riga del ds
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
            map[i*m+w]=icent;
            
        }//w
       
   }//i

}




/**
 * args:
 * -d        = dimensione dei vettori
 * -m        = numero di sottovettori
 * -eps      = epsilon, fattore di precisione
 * -tmin     = numero minimo di ricollocamento dei centroidi
 * -tmax     = numero massimo di ricollocamento dei centroidi
 * -k        = numero centroidi
 * -codebook = insieme di centroidi
 * -n        = numero di elementi del dataset
 * -dataset  = insieme di punti
 * -map      = associazioni dataset-codebook
 * 
 * descr:
 * -algoritmo di costruzione del miglior codebook possibile. bisogna passare un codebook già esistente
 */

void k_means( int d, int m, float eps, int tmin, int tmax, int k, float* codebook, int n, float* dataset, int* map){

    #ifdef DEBUG_KMEANS
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'kmeans' #####\n");
    #endif

    /*
     1- avendo codebook i centroidi e il dataset, crea una mappa che ad ogni elemento del dataset, associa un centroide (vq- quantizzazione)
     2- calcola la funziona la obiettivo
     3- delta=obiettivo(old)-obiettivo(nuovo)
     4- condizione di terminazione: (tmin<=t && (tmax<t || delta<=eps)), se vera termina, sennò avanti
     5- sostituisce i centroidi con la media
     6- goto 2
    */

    float tmp=0;
    int t=1;
    float delta=0;
    
    double ob=0; 
    double nuovo_ob=0;

    pq(d,m,k,codebook,n,dataset,map);
     #ifdef DEBUG_KMEANS
        printf("pq prima di partire, mappa\n");
        printmi(1,n,map);
    #endif

    while(t++<tmin){
        KMEANS_STEP(d,m,n,dataset,map,k,codebook);
        #ifdef DEBUG_KMEANS
            printf("nuovi centroidi al passo %i\n",t-1);
            printmf(d,k,codebook);
        #endif
    }

    ob=obiettivo(d,m,n,dataset,map,codebook);
        #ifdef DEBUG_KMEANS
            printf("nuovi centroidi al passo %i\n",t-1);
            printmf(d,k,codebook);
        #endif
        
    // abbiamo fatto il numero minimo di passi, andiamo alla seconda condizione
    while(tmax>=t++ && delta>eps ){
        KMEANS_STEP(d,m,n,dataset,map,k,codebook);
        nuovo_ob= obiettivo(d,m,n,dataset,map,codebook);
        #ifdef DEBUG_KMEANS
            printf("stampa obiettivo passo t=%i\nob=%f\n",t-1,nuovo_ob);
        #endif
        delta=DELTA(ob,nuovo_ob);
        ob=nuovo_ob;

    }

}




/**
 * args:
 * -values  = insieme di valori da ordinare  
 * -indices = indici correlati ai valori da ordinare
 * -start   = colonna da cui iniziare l'ordinamento
 * -mean    = indice medio ordinamento. primo vettore [start,mean-1] secondo [mean,end-1]
 * -end     = indice finale ordinamento
 * -offset  = cella da cui iniziare ad ordinare
 * 
 * descr:
 * -fonde due sottovettori parzialmente ordinati dall'indice offset+start e offset+mean-1 e da offset+mean a offset+end-1 
 */
void merge(double* values, int* indices, int start, int mean, int end, int offset) {

    #ifdef DEBUG_MERGE
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'merge' #####\n");
    #endif

#ifdef DEBUG_MERGE
    printf("");
#endif


    int i = start, j= mean+1, k=0, length=(end-start)+1, bi[length] ;
    double b[length];
#ifdef DEBUG_MERGE
    printf("stato del metodo\nstart=%i\nmean=%i\nend=%i\n\ncalcolo length=end-mean+2=%i-%i+2=%i\n\n",start,mean,end,end,mean,length);
    
    printf("controllo i<=mean && j<=end, cioe' %i<=%i && %i<=%i?\n",i,mean,j,end);
#endif
    while (i<=mean && j<=end) {
#ifdef DEBUG_MERGE
    printf("passed\n");
#endif
#ifdef DEBUG_MERGE
    printf("controllo values[%i]<values[%i], cioe' %lf<%lf\n",offset+i,offset+j,values[offset+i],values[offset+j]);
#endif
        //blocco iniziale, si confronta elemento con elemento
        if (values[offset+i]<values[offset+j]) {
#ifdef DEBUG_MERGE
    printf("passed\ncopia dei valori:\nb[%i]=%lf\nbi[%i]=%i",k,values[offset+i],k,indices[i]);
#endif
            b[k] = values[offset+i];
            bi[k] = indices[offset+i];
            i++;
        } else {
#ifdef DEBUG_MERGE
    printf("not passed\ncopia dei valori:\nb[%i]=%lf\nbi[%i]=%i",k,values[offset+j],k,indices[j]);
#endif
            b[k] = values[offset+j];
            bi[k] = indices[offset+j];
            j++;
        }
        k++;
#ifdef DEBUG_MERGE
    printf("\n\ncontrollo i<=mean && j<=end, cioè %i<=%i && %i<=%i?\n",i,mean,j,end);
#endif
    }
#ifdef DEBUG_MERGE
    printf("not passed\n\n");
#endif
    while (i <= mean) {
#ifdef DEBUG_MERGE
    printf("i<=mean, %i<=%i\n",i,mean);
#endif
        //i non è arrivato a media
        b[k] = values[offset+i];
        bi[k] = indices[offset+i];
#ifdef DEBUG_MERGE
    printf("copia del valori i-esimi: %lf per values e %i per indices\n",values[offset+i],indices[offset+i]);
#endif
        i++;
        k++;
    }
    while (j <= end) {
#ifdef DEBUG_MERGE
    printf("j<=end, %i<=%i\n",j,end);
#endif
        //j non è arrivato alla fine
        b[k] = values[offset+j];
        bi[k] = indices[offset+j];
#ifdef DEBUG_MERGE
    printf("copia del valori j-esimi: %lf per values e %i per indici\n",values[offset+j],indices[offset+j]);
#endif
        j++;
        k++;
    }
#ifdef DEBUG_MERGE
    printf("Valori b e bi copiati, ecco i risultati\n");
    for(int h=0;h<length;h++){
        printf("|%lf|",b[h]);
    }

    printf("\n");
    for(int h=0;h<length;h++){
        printf("|%i|",bi[h]);
    }

    printf("\n");
    printf("\n\ncopia dei valori!\n");
#endif
    for (k=start; k<=end; k++){
#ifdef DEBUG_MERGE
    printf("values[%i]=%lf\nindices[%i]=%i\n\n",offset+k,k,b[k-start],bi[k-start]);
#endif
        values[offset+k] = b[k-start];
        indices[offset+k]=bi[k-start];
    }
}


/**
 * args:
 * -values  = insieme di valori da ordinare  
 * -indices = indici correlati ai valori da ordinare
 * -start   = colonna da cui iniziare l'ordinamento
 * -end     = indice finale ordinamento
 * -offset  = cella da cui iniziare ad ordinare
 * 
 * descr:
 * -ordina i vettori values e indices seuendo l'ordinamento crescente di values
 */
void mergeSort(double* values, int* indices, int start, int end, int offset) {
    int mean;
    if (start < end) {
        mean = (start+end)/2;
        mergeSort(values, indices, start, mean, offset);
        mergeSort(values, indices, mean+1, end,offset);
        merge(values, indices, start, mean, end, offset);
    }

}

/**
 * args:
 * -d          = dimensione del vettore
 * -k          =
 * -m          = numero di sottovettori
 * -nrd        = numero distanze
 * -distanze   = distanze tra tutti i punti del codebook
 * -K          = numero elementi in ANN
 * -ANN        = risultati (set già inizializzato)
 * -ANN_values = valori delle distanze relative ai risultati
 * -n          = numero di elementi nel dataset
 * -map        = dataset
 * -ix         = indice del punto query, per memorizzarlo in ANN 
 * -qx       = pq(x), vettore a m celle con il centroide di x
 * 
 * descrizione:
 * -implementa la distanza simmetrica con un singolo punto ( e la scrive in ANN, il valore in ANN_values)
 */
void SDC (int d, int k, int m, int nrd, double* distanze, int K, int*ANN, double* ANN_values, int n, int* map, int ix, int *qx){
    #ifdef DEBUG_SDC
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'SDC' #####\n");
    #endif

    #ifdef DEBUG_SDC
        printf("\n");
    #endif


    /*
     * passi:
     * 
     * -calcolare i centroidi di x
     * -calcolare insieme distanze 
     * -per ogni centroide in map, calcolare la distanza euclidea tra un sottocentroide e un altro come:
     * --radice(somma( distanza(qx,qy)^2 ))
     * --confrontare con minore distanza attualmente calcolata 
     * -restituire i risultati 
     * 
     */
   
    int j=0,w, imax=0;
    double sommaparz=0,vmax=-1,tmp;

    //double ANN_values[K]; //valori del singolo punto;

    

    //abbiamo i centroidi associati al punto, adesso dobbiamo prendere per ogni punto un centroide e vedere se è il punto vicino 
    for(j=0;j<K;j++){
        sommaparz=0;
        //i primi K passi riempiamo il suo rispettivo ANN
       for(w=0;w<m;w++){
            
            sommaparz+=distanze[MATRIX3_INDEX(k,m,qx[w],map[j*m+w],w)];
            #ifdef DEBUG_SDC
                printf("indice=%i\n",MATRIXBLABLA);
                printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[*m+w],sommaparz);
            #endif
       }
       ANN[ix*K+j]=j;
       tmp=sqrt(sommaparz);
       ANN_values[ix*K+j]=tmp;

       
        if(vmax<tmp){
            vmax=tmp;
            imax=j;
        }
       

       
    }

    for(;j<n;j++){
        sommaparz=0;
        for(w=0;w<m;w++){
            
            sommaparz+=distanze[MATRIX3_INDEX(k,m,qx[w],map[j*m+w],w)];
            #ifdef DEBUG_SDC
                printf("indice=%i\n",MATRIXBLABLA);
                printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[*m+w],sommaparz);
            #endif
       }
       tmp=sqrt(sommaparz);

        if(vmax>tmp){
            //entrata in ANN del valore
            ANN[ix*K+imax]=j;
            ANN_values[ix*K+imax]=tmp;

            //ricerca del nuovo massimo
            vmax=tmp;
            for(w=0;w<K;w++){
                if(vmax<ANN_values[ix*K+w]){
                    vmax=ANN_values[ix*K+w];
                    imax=w;
                }
            }
        }//if
       
    }//for
    #ifdef DEBUG_SDC
        printf("ANN_values prima dell' ordinamento:");
        printdv(K,ANN_values,0);
    #endif
    mergeSort(ANN_values,ANN,0,K-1,ix*K);

    

    #ifdef DEBUG_SDC
        printf("ANN_values dopo l' ordinamento:");
        printdv(K,ANN_values,0);
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'SDC' #####\n");
    #endif
}

/**
 * args:
 * -d          = dimensione del vettore
 * -m          = numero di sottovettori
 * -k          = numero centroidi
 * -codebook   = centroidi
 * -K          = numero elementi in ANN
 * -ANN        = risultati (set già inizializzato)
 * -ANN_values = valori delle distanze relative ai risultati
 * -n          = numero di elementi nel dataset
 * -map        = dataset
 * -nq         = numero di punti 
 * -qs         = struttura con punti query
 * 
 * descrizione:
 * -scrive in ANN gli indici dei punti più vicini a x usando l'algoritmo di distanza simmetrica (in ANN_values scrive invece i valori)
 */
void ANNSDC(int d, int m, int k, float* codebook, int K, int*ANN, double* ANN_values, int n, int*map, int nq, float*qs){
    #ifdef DEBUG_ANNSDC
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'ANNSDC' #####\n");
    #endif

    /*
     * -calcolare tutte le distanze tra centroidi
     * -per ogni punto query calcolare gli ANN con SDC
     */
    int dstar=d/m;
    int nrd=((k-1)*(k))/2;
    int ncd=m;
    int i,j,w;
    /**
     */ 
    int * icent= ASSEGNA_BLOCCO(int,m);
    /**
     * matrice cubica dove :
     *  -ogni riga corrisponde al vettore i-esimo
     *  -ogni colonna corrisponde al vettore j-esimo
     *  -ogni cella contiene un vettore di m-distanze corrispondenti alle m distanze dei sottovettori di centroidi
     *
     */
    double distanze[k*k*m];
    double tmp;

    for( i=0; i<k;i++){
        //centroide i
        for(w=0;w<m;w++) 
            distanze[MATRIX3_INDEX(k,m,i,i,w)]=0; 
        for( j=i+1;j<k;j++){
            //centroide j
            #ifdef DEBUG_ANNSDC
                printf("distanza tra i=%i e j=%i\n",i,j);
            #endif
            for(w=0;w<m;w++){
                #ifdef DEBUG_ANNSDC
                    printf("sottovettore %i\n",w);
                #endif
                //tmp=(double)(dist_2(dstar,codebook,d*i+w*dstar,codebook,d*j+w*dstar)); //distanze al quadrato
                DIST_E_2(dstar,codebook,d*i+w*dstar,codebook,d*j+w*dstar,tmp);
                distanze[MATRIX3_INDEX(k,m,i,j,w)]=tmp;
                distanze[MATRIX3_INDEX(k,m,j,i,w)]=tmp;
                #ifdef DEBUG_ANNSDC
                    printf("distanze[%i]=%lf\n\n",,distanze[]);
                #endif
            }//w
        }//j
    }//i
    for(i=0;i<nq;i++){
        for(int w=0; w<m;w++){
            icent[w]=mindist(d,dstar,w,qs,i,k,codebook);
            #ifdef DEBUG_ANNSDC
                printf("icent[%i]=%i\n\n",w,icent[w]);
            #endif
        }
        SDC(d,k,m,nrd,distanze,K,ANN,ANN_values,n,map,i,icent);
    }
    #ifdef DEBUG_ANNSDC
    printf("icent e':    ");

        printiv(m,icent,0);
    #endif

    #ifdef DEBUG_ANNSDC
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'ANNSDC' #####\n");
    #endif
    free(icent);

}//ANNSDC



//macro per ADC
//#define ADCFOR1(W,M,J,K) for(w=0;w<m;w++){ int idist=W*M+J*K; int sommaparz+=distanze[idist*m+w]; }


/**
 * args:
 * -d          = dimensione del vettore
 * -k          = numero di centroidi
 * -m          = numero di sottovettori
 * -distanze   = distanze tra tutti i punti del codebook
 * -K          = numero elementi in ANN
 * -ANN        = risultati (set già inizializzato)
 * -ANN_values = valori delle distanze relative ai risultati
 * -n          = numero di elementi nel dataset
 * -map        = associazioni dataset-codebook
 * -ix         = indice del punto query, per memorizzarlo in ANN
 * -qs         = insieme di punti del queryset
 * 
 * descrizione:
 * -implementa la distanza asimmetrica con un singolo punto ( e la scrive in ANN, il valore in ANN_values)
 */
void ADC (int d, int k, int m, double* distanze, int K, int*ANN, double* ANN_values,int n, int* map, int ix, float *qs){
    #ifdef DEBUG_ADC
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'ADC' #####\n");
    #endif

    /*
    * passi:
    * 
    * -calcolare i centroidi di x
    * -calcolare insieme distanze 
    * -per ogni centroide in map, calcolare la distanza euclidea tra un sottocentroide e un altro come:
    * --radice(somma( distanza(qx,qy)^2 ))
    * --confrontare con minore distanza attualmente calcolata 
    * -restituire i risultati 
    * 
    */
   
    int j=0,w,idist, imax=0;
    double sommaparz=0,vmax,tmp;

    //double ANN_values[K]; //valori del singolo punto;

    //primo giro va fatto a parte

    for(w=0;w<m;w++){
        idist=map[j*m+w];
        sommaparz+=distanze[idist*m+w];
        #ifdef DEBUG_ADC
            printf("idist=%i\n",idist);
            printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
        #endif
    }
    ANN[ix*K+j]=j;
    tmp=sqrt(sommaparz);
    ANN_values[ix*K+j]=tmp;
    vmax=tmp;
 
    for(j=1;j<K;j++){
        sommaparz=0;
        //i primi K passi riempiamo il suo rispettivo ANN
       for(w=0;w<m;w++){
            idist=map[j*m+w];
            sommaparz+=distanze[idist*m+w];
            #ifdef DEBUG_ADC
                printf("idist=%i\n",idist);
                printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
            #endif
        }
        ANN[ix*K+j]=j;
        tmp=sqrt(sommaparz);
        ANN_values[ix*K+j]=tmp;
        if(vmax<tmp){
            vmax=tmp;
            imax=j;
        }
       

       
    }

    for(;j<n;j++){
        sommaparz=0;
        for(w=0;w<m;w++){
            idist=map[j*m+w];
            sommaparz+=distanze[idist*m+w];
            #ifdef DEBUG_ADC
                printf("idist=%i\n",idist);
                printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
            #endif
        }
        tmp=sqrt(sommaparz);

        if(vmax>tmp){
            //entrata in ANN del valore
            ANN[ix*K+imax]=j;
            ANN_values[ix*K+imax]=tmp;

            //ricerca del nuovo massimo
            vmax=tmp;
            for(w=0;w<K;w++){
                if(vmax<ANN_values[ix*K+w]){
                    vmax=ANN_values[ix*K+w];
                    imax=w;
                }
            }//
        }//if
       
    }//for
    
    #ifdef DEBUG_ADC
        printf("ANN_values prima dell' ordinamento:");
        printdv(K,ANN_values,0);
    #endif
    mergeSort(ANN_values,ANN,0,K-1,ix*K);
    #ifdef DEBUG_ADC
        printf("ANN_values dopo l' ordinamento:");
        printdv(K,ANN_values,0);

        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'ADC' #####\n");
    #endif
}


/**
 * args:
 * -d          = dimensione del vettore
 * -m          = numero di sottovettori
 * -k          = numero centroidi
 * -codebook   = centroidi
 * -K          = numero elementi in ANN
 * -ANN        = risultati (set già inizializzato)
 * -ANN_values = valori delle distanze relative ai risultati
 * -n          = numero di elementi nel dataset
 * -map        = associazioni dataset-codebook
 * -nq         = numero punti query set
 * -qs         = struttura con punti query
 * 
 * descrizione:
 * -scrive in ANN gli indici dei punti più vicini a x usando l'algoritmo di distanza asimmetrica (in ANN_values scrive invece i valori)
 */
void ANNADC(int d, int m, int k, float* codebook, int K, int*ANN, double* ANN_values, int n, int*map, int nq, float*qs){
    #ifdef DEBUG_ANNADC
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'ANNADC' #####\n");
    #endif

    int i,w,j,c=0,dstar=d/m,icent;
    double distanze[k*m];

    #ifdef DEBUG_ANNADC
        printf("riepilogo stato del metodo\nd=%i\nm=%i\nk=%i\nK=%i\nn=%i\nnq=%i\ncodebook=\n",d,m,k,K,n,nq);
        printmf(d,k,codebook);
        printf("map=\n");
        printmi(m,n,map);
    #endif

/**
 * passi:
 * -per ogni punto query
 * --creare una struttura che mantenga le distanze tra ogni punto e e tutti i centroidi
 * --applicare ADC(d,m,k,distanze,n,mappa,ANN,ix,qs) 
 */

    for(i=0;i<nq;i++){ // per ogni punto del query set
        for(j=0;j<k;j++){//per ogni punto del codebook
            for(w=0;w<m;w++){//ogni sottovettore
                #ifdef DEBUG_ANNADC
                    //printf("i=%i,j=%i,w=%i\n",i,j,w);
                #endif
                //distanze[c]=(double)(dist_2(dstar,qs,i*d+w*dstar,codebook,j*d+w*dstar));
                DIST_E_2(dstar,qs,i*d+w*dstar,codebook,j*d+w*dstar,distanze[c]);
                c++;
            }
        }
        ADC(d,k,m,distanze,K,ANN,ANN_values,n,map,i,qs);
        c=0;
    }


    #ifdef DEBUG_ANNADC
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'ANNADC' #####\n");
    #endif
}

/**
 * -d        = dimensione vettori
 * -w        = numero centoidi da associare
 * -qs       = query set
 * -ix       = indice di x nel qs
 * -k        = numero elementi codebook
 * -codebook = struttura centroidi
 * -mapxw    = struttura di ritorno
 * 
 * descr 
 *  -scrive in mapxw i w centroidi più vicini al punto ix di qs
 */

void centroidi_associati(int d, int w, float* qs,int ix, int k, float*codebook, int*mapxw){
    int j,imax,h;
    float distmax=-1,tmp;
    float *rx_values=ASSEGNA_BLOCCO(float,w);
    //per ogni punto query, vanno memorizzati i w centroidi vicini e la loro distanza
    
    //INIZIO RICERCA W
    
    for(j=0; j<w;j++){
        //tmp=dist_2(d,qs,ix*d,codebook,j*d); // dist_2 perche': tanto se e' piu' piccola la radice, e' piu' piccolo anche il radicando
        DIST_E_2(d,qs,ix*d,codebook,j*d,tmp);
        if(distmax<tmp){
            distmax=tmp;
            imax=j;
        }
        mapxw[ix*w+j]= j;
        rx_values[j]=tmp;
       
    }
    
    for(;j<k;j++){
        //tmp=dist_2(d,qs,ix*d,codebook,j*d); // dist_2 perche': tanto se e' piu' piccola la radice, e' piu' piccolo anche il radicando
        DIST_E_2(d,qs,ix*d,codebook,j*d,tmp);
        if(distmax>tmp){
        
            distmax=tmp;
            rx_values[imax]= tmp;
            mapxw[ix*w+imax]= j;

            for(h=0; h<w;h++){
                //ricerca nuovo massimo 
                if(distmax<rx_values[h]){
                    distmax=rx_values[h];
                    imax=h;
                }
            }//h
        }//trovato nuovo max
    }//j
    //FINE RICERCA W
    free(rx_values);
}


/**
 * args:
 * -input = insieme di parametri descritti nella struttura params.
 * 
 * 
 * descr:
 * -applica la ricerca non esaustiva, simmetrica o asimmetrica in relazione a quanto specificato dal parametro 'symmetric'.
 * Supponendo già per calcolati i residui, le associazioni tra residui e centroidi, i codebook coarse e non, i centroidi associati per ogni x e le associazioni qc(x-q(y)), allora vengono precalcolate le distanze punto-residuo come descritto dall'algoritmo SDC o ADC (quindi associazione-associazione o punto-associazione) e ricercate le minori. Gli indici vengono messi in ANN e i valori in ANN_values
 */
void notExaustive(params* input){

    #ifdef DEBUG_NOTEXAUSTIVE
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'notExaustive' #####\n");
    #endif

#ifdef DEBUG_NOTEXAUSTIVE
    printf("\n");
#endif

    //CONTINUARE AD APPLICARE LEMODIFICHE RELATIVE A SPOSTAMENTO INIZIALIZZAZIONE DELLE MAPPE IN PQNN_INDEX

    int i,c=0,j,imax,h,icent,icent2,icent3,icent4,t,z=0,dstar=input->d/input->m;

    double vmax=-1;
    double tmp;
               
    /* pq(x-qc(y) -> pq(rx) */
    int *mapx=ASSEGNA_BLOCCO(int,input->m*input->w*input->nq);

    double *distanze; 

    if(input->symmetric){
        //SDC
        pq(input->d,input->m,input->k,input->codebookp,input->nq*input->w,input->rx,mapx);
        #ifdef DEBUG_NOTEXAUSTIVE
            printf("Stampa Matrice mapx\n");
            printmi(input->w,input->m*input->nq,mapx);
        #endif
        //matrice delle differenze
        distanze=ASSEGNA_BLOCCO(double,input->k*input->k*input->m);
        for( i=0; i<input->k;i++){
            //centroide i
            for(h=0;h<input->m;h++) 
                distanze[MATRIX3_INDEX(input->k,input->m,i,i,h)]=0;
            for( j=i+1;j<input->k;j++){
                //centroide j
               
                for(h=0;h<input->m;h++){
                
                    //tmp=(double)(dist_2(dstar,codebookp,d*i+h*dstar,codebookp,d*j+h*dstar)); //distanze al quadrato
                    DIST_E_2(dstar,input->codebookp,input->d*i+h*dstar,input->codebookp,input->d*j+h*dstar,tmp);
                    distanze[MATRIX3_INDEX(input->k,input->m,i,j,h)]=tmp;
                    distanze[MATRIX3_INDEX(input->k,input->m,j,i,h)]=tmp;
                    
                }//
            }//js
        }//i
        for(i=0;i<input->nq;i++){
            z=0;//contatore di ANN
            for( j=0 ; j< input->w && z<input->knn; j++){
                //w centroidi di x
                icent=input->mapxw[i*input->w+j];
                c=input->codemap[LAST_INDEX(input->n+1,icent)];
                for(h=0;h<c && z<input->knn;h++){
                    tmp=0;
                    //punti vicini a centroide
                    icent3=input->codemap[icent*
                    (input->n+1)+h];
                    for(t=0;t<input->m;t++){
                        icent2=mapx[MATRIX3_INDEX(input->w,input->m,i,j,t)];
                        icent4=input->pqy[icent3*input->m+t];
                        tmp+=distanze[MATRIX3_INDEX(input->k,input->m,icent2,icent4,t)];
                    }
                    input->ANN[i*input->knn+z]=icent3;
                    tmp=sqrt(tmp);
                    input->ANN_values[i*input->knn+z]=tmp;
                    if(vmax<tmp){
                        vmax=tmp;
                        imax=z;
                    }
                    z++;
                }
            }
            for(;h<c;h++){
               tmp=0;
                //punti vicini a centroide
                icent3=input->codemap[icent*
                (input->n+1)+h];
                for(t=0;t<input->m;t++){
                    icent2=mapx[MATRIX3_INDEX(input->w,input->m,i,j,t)];
                    icent4=input->pqy[icent3*input->m+t];
                    tmp+=distanze[MATRIX3_INDEX(input->k,input->m,icent2,icent4,t)];
                }
                tmp=sqrt(tmp);
                if(vmax>tmp){
                    //entrata in ANN del valore
                    input->ANN[i*input->knn+imax]=icent3;
                    input->ANN_values[i*input->knn+imax]=tmp;

                    //ricerca del nuovo massimo
                    vmax=tmp;
                    for(t=0;t<input->knn;t++){
                        if(vmax<input->ANN_values[i*input->knn+t]){
                            vmax=input->ANN_values[i*input->knn+t];
                            imax=t;
                        }
                    }//t
                }//if
            }
            for(; j< input->w; j++){
                //w centroidi di x
                icent=input->mapxw[input->w*i+j];
                c=input->codemap[LAST_INDEX(input->n+1,j)];
                for(h=0;h<c;h++){
                   tmp=0;
                    //punti vicini a centroide
                    icent3=input->codemap[icent*
                    (input->n+1)+h];
                    for(t=0;t<input->m;t++){
                        icent2=mapx[MATRIX3_INDEX(input->w,input->m,i,j,t)];
                        icent4=input->pqy[icent3*input->m+t];
                        tmp+=distanze[MATRIX3_INDEX(input->k,input->m,icent2,icent4,t)];
                    }
                    tmp=sqrt(tmp);
                    if(vmax>tmp){
                        //entrata in ANN del valore
                        input->ANN[i*input->knn+imax]=icent3;
                        input->ANN_values[i*input->knn+imax]=tmp;

                        //ricerca del nuovo massimo
                        vmax=tmp;
                        for(t=0;t<input->knn;t++){
                            if(vmax<input->ANN_values[i*input->knn+t]){
                                vmax=input->ANN_values[i*input->knn+t];
                                imax=t;
                            }
                        }
                    }//if
                }//for h
            }//for j
            mergeSort(input->ANN_values,input->ANN,0,input->knn-1,i*input->knn);
        }//i
    }else{
        //ADC
        //matrice delle differenze
        distanze=ASSEGNA_BLOCCO(double,input->k*input->m);
        for(i=0;i<input->nq;i++){
            //punti query st
            z=0;//contatore per ANN
            for( j=0 ; j< input->w && z<input->knn; j++){
                c=0;//contatore distanze
                for(t=0;t<input->k;t++){//per ogni punto del codebookp
                    for(h=0;h<input->m;h++){//ogni sottovettore
                        //distanze[c]=(double)(dist_2(dstar,diffx,j*d+h*dstar,codebookp,t*d+h*dstar));
                        DIST_E_2(dstar,input->rx,MATRIX3_INDEX(input->w,input->d,i,j,h*dstar),input->codebookp,t*input->d+h*dstar,distanze[c]);
                        c++;
                    }
                }
                //c è inutilizzato da qua, diventa numero di punti associati al centroide j
                icent=input->mapxw[MATRIX2_INDEX(input->w,i,j)]; //indice j del centroide i di x
                c=input->codemap[LAST_INDEX(input->n+1,icent)];
                for(h=0;h<c && z<input->knn;h++){
                    tmp=0;
                    //punti vicini a centroide
                    icent3=input->codemap[MATRIX2_INDEX(input->n+1,icent,h)];
                    for(t=0;t<input->m;t++){
                        icent4=input->pqy[icent3*input->m+t];
                        tmp+=distanze[MATRIX2_INDEX(input->m,icent4,t)];
                    }
                    input->ANN[i*input->knn+z]=icent3;
                    tmp=sqrt(tmp);
                    input->ANN_values[i*input->knn+z]=tmp;
                    if(vmax<tmp){
                        vmax=tmp;
                        imax=z;
                    }
                    z++;
                }
            }
            for(;h<c;h++){
                tmp=0;
                //punti vicini a centroide
                icent3=input->codemap[MATRIX2_INDEX(input->n+1,icent,h)];
                for(t=0;t<input->m;t++){
                    icent4=input->pqy[icent3*input->m+t];
                    tmp+=distanze[MATRIX2_INDEX(input->m,icent4,t)];
                }
                tmp=sqrt(tmp);
                if(vmax>tmp){
                    //entrata in ANN del valore
                    input->ANN[i*input->knn+imax]=icent3;
                    input->ANN_values[i*input->knn+imax]=tmp;

                    //ricerca del nuovo massimo
                    vmax=tmp;
                    for(t=0;t<input->knn;t++){
                        if(vmax<input->ANN_values[i*input->knn+t]){
                            vmax=input->ANN_values[i*input->knn+t];
                            imax=t;
                        }
                    }
                }//if
            }
            for(; j< input->w; j++){
                c=0;//contatore distanze
                for(t=0;t<input->k;t++){//per ogni punto del codebookp
                    for(h=0;h<input->m;h++){//ogni sottovettore
                        //distanze[c]=(double)(dist_2(dstar,diffx,j*d+h*dstar,codebookp,t*d+h*dstar));
                        DIST_E_2(dstar,input->rx,MATRIX3_INDEX(input->w,input->d,i,j,h*dstar),input->codebookp,t*input->d+h*dstar,distanze[c]);
                        c++;
                    }
                }
                //c è inutilizzato da qua, diventa numero di punti associati al centroide j
                icent=input->mapxw[MATRIX2_INDEX(input->w,i,j)]; //indice j del centroide i di x
                c=input->codemap[LAST_INDEX(input->n+1,icent)];
                for(h=0;h<c;h++){
                    tmp=0;
                    //punti vicini a centroide
                    icent3=input->codemap[MATRIX2_INDEX(input->n+1,icent,h)];
                    for(t=0;t<input->m;t++){
                        icent4=input->pqy[icent3*input->m+t];
                        tmp+=distanze[MATRIX2_INDEX(input->m,icent4,t)];
                    }
                    tmp=sqrt(tmp);
                    if(vmax>tmp){
                        //entrata in ANN del valore
                        input->ANN[i*input->knn+imax]=icent3;
                        input->ANN_values[i*input->knn+imax]=tmp;

                        //ricerca del nuovo massimo
                        vmax=tmp;
                        for(t=0;t<input->knn;t++){
                            if(vmax<input->ANN_values[i*input->knn+t]){
                                vmax=input->ANN_values[i*input->knn+t];
                                imax=t;
                            }
                        }
                    }//if
                }//for h
            }//for j
            mergeSort(input->ANN_values,input->ANN,0,input->knn-1,i*input->knn);
        }//i
    }//else asymmetric
    free(mapx);
    free(distanze);

    #ifdef DEBUG_NOTEXAUSTIVE
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'notExaustive' #####\n");
    #endif
}//notExaustive