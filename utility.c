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
    #include "printutility.c"
    //#define DEBUG_DIST2
    //#define DEBUG_DIST
    //#define DEBUG_COPYV
    //#define DEBUG_MINDIST
    //#define DEBUG_INITCODEBOOK
    //#define DEBUG_DISTQ
    //#define DEBUG_OBIETTIVO
    //#define DEBUG_EQUALS
    //#define DEBUG_NUOVICENTROIDI
    //#define DEBUG_PQ
    //#define DEBUG_KMEANS
    //#define DEBUG_CINDEX
    //#define DEBUG_MERGE
    //#define DEBUG_MERGESORT
    #define DEBUG_SDC
    #define DEBUG_ANNSDC
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
 *  calcola la distanza euclidea al quadrato tra un punto di x (segnato a partire da xi) e un punto di y (segnato a partire da yi).
 * A partire da xi e da yi vengono analizzati d elementi
 * 
 */
float dist_2(int d, float *x, int xi, float *y, int yi){
    
    #ifdef DEBUG_DIST2
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'dist2' #####\n");
    #endif

    float somma=0, differenza=0;
    for(int i=0;i<d;i++){
        differenza=x[xi+i]-y[yi+i];
        #ifdef DEBUG_DIST2
            printf("differenza %f-%f: %f\n",x[xi+i],y[yi+i],differenza);
        #endif
        #ifdef DEBUG_DIST2
            printf("al quadrato (%f)^2=",differenza);
        #endif
        differenza*=differenza;
        #ifdef DEBUG_DIST2
            printf("%f\n",differenza);
        #endif
        #ifdef DEBUG_DIST2
            printf("somma parziale %f+%f=",somma,differenza);
        #endif
        somma+=differenza;
        #ifdef DEBUG_DIST2
            printf("%f\n\n",somma);
        #endif
    }

    #ifdef DEBUG_DIST2
        printf("somma completa %f\n",somma);
        printf("con la radice %f\n",sqrt(somma));
    #endif

    #ifdef DEBUG_DIST2
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'dist' #####\n");
    #endif

    return somma;
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
 * A partire da xi e da yi vengono analizzati d elementi
 * 
 */
float dist(int d, float *x, int xi, float *y, int yi){
    float dista=dist_2(d,x,xi,y,yi);
    return sqrt(dista);
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
void copyv(int d, float* dest, int desti, float *src, int srci){
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

    mind=dist(dstar,dataset,j,codebook,mi*dstar);
    #ifdef DEBUG_MINDIST
        printf("centroide 0 (indice %i) preso in carico:\n",mi*dstar);
        printfv(dstar,codebook,mi*dstar);
        printf("\n");
        printf("distanza=%f\n",mind);
    #endif

    for(i=1;i<k;i++){
        ndist=dist(dstar,dataset,j,codebook,d*i+mi*dstar);
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
 *  -d=dimensione spazio vettoriale
 *  -n= dimensione dataset
 *  -dataset= insieme di punti in Rd
 *  -k= numero di centroidi
 *  -codebook= codebook da riempire
 * 
 * descrizione:
 * -si riempie il codebook con valori random facenti parte dell'insieme di partenza
 */
void init_codebook(int d, int n, float* dataset, int k, float* codebook){
    #ifdef DEBUG_INITCODEBOOK
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'init_codebook' #####\n");
    #endif

    #ifdef DEBUG_INITCODEBOOK
        printf("Ecco il dataset di dimensione %i passato:\n",n);
        printmf(d,n,dataset);
    #endif


    int i=0,j=0,flag=1,r=0;
    #ifdef DEBUG_INITCODEBOOK
        printf("numero di elementi da creare:%i\n\n",k);
    #endif
    int *rp=(int*)malloc(sizeof(int)*k);

    for(;i<k;i++){        
        do{
            flag=1;
            r= (int)(rand()%n);
            #ifdef DEBUG_INITCODEBOOK
                printf("ecco l'indice generato: %i\n",r);
            #endif 
            for(j=0;j<i;j++) {
                if(r==rp[j])
                {
                    #ifdef DEBUG_INITCODEBOOK
                        printf("ATTENZIONE INDICE GIA' PRESENTE!!\n\n");
                    #endif 
                    flag=0;
                }
            }//for j
        }while(flag==0); //do flag
        rp[i]=r;
        copyv(d,codebook,i,dataset,r);
        #ifdef DEBUG_INITCODEBOOK
            printf("\n\n");
        #endif
    }//for i

    #ifdef DEBUG_INITCODEBOOK
        printf("stampiamo ora il codebook risultato\n");
        printmf(d,k,codebook);
    #endif

    #ifdef DEBUG_INITCODEBOOK
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'init_codebook' #####\n");
    #endif
}

/**
 * args:
 * -d=numero di elementi a vettore
 * -m=numero di sottovettori
 * -dataset=dataset
 * -i=indice dataset
 * -map=mappa dataset-centroide associato
 * -codebook=insieme centroidi
 * 
 * desc:
 * -calcola la distanza al quadrato tra un punto e i suoi sottocentroidi
 */
float dist_q(int d, int m, float*dataset, int i, int * map, float*codebook){
    #ifdef DEBUG_DISTQ
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'dist_q' #####\n");
    #endif
    int w,j,icent,dstar=d/m;
    float distz=0,parz;
    for(w=0; w<m;w++){
        icent=map[i*m+w];
        for(j=0; j<dstar;j++){
            parz=(dataset[i*d+w*dstar+j]-codebook[icent*d+w*dstar+j]);
            parz*=parz;
            distz+=parz;
        }
    }
    #ifdef DEBUG_DISTQ
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'dist_q' #####\n");
    #endif
    return distz;
}

/**
 * argomenti:
 *  - 'd' coordinate del singolo punto
 *  - 'n' numero di righe della map 
 *  - 'map' una matrice che contiene per ogni riga l'indice del relativo centroide
 * 
 * descrizione:
 *  -calcola il valore della funzione obiettivo attuale come 'la somma delle distanze al quadrato di ogni punto dal suo centroide'
 */
double obiettivo(int d, int m, int n, float* dataset, int *map,  float* codebook){
    #ifdef DEBUG_OBIETTIVO
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'obiettivo' #####\n");
    #endif

 
    int i;
    double somma=0,parziale;
    for(i=0;i<n;i++){
        parziale=dist_q(d, m, dataset,i,map,codebook);
        somma+=parziale;
    }
    #ifdef DEBUG_OBIETTIVO
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'obiettivo' #####\n");
    #endif
    return somma;
}


/**
 * argomenti:
 * -d: dimensione degli elementi da controllare per ogni vettore
 * -*x: primo vettore
 * -xi: indice inizio da cui controllare gli elementi di x ( non moltiplicato)
 * -*y: secondo vettore 
 * -yi: indice inizio da cui controllare gli elementi di y ( non moltiplicato)
 * 
 * descrizione:
 * - verifica se due vettori x e y, a partire ognuno da un certo indice indicato (xi/yi rispettivamente), sono uguali per ogni elemento
 * restituisce 1 se sono uguali, 0 se sono diversi
 */
int equals(int d, float *x, int xi, float *y, int yi){
    #ifdef DEBUG_EQUALS
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'equals' #####\n");
    #endif
    int i=0,flag=0;
    
    for(i=0; i<d&&flag==0; i++){
        flag=(x[xi+i]!=y[yi+i])?0:1;
    }

    return flag; //ritorna 1 se uguali, 0 se diversi
}

/**
 * argomenti:
 *  - 'd' numero di coordinate per punto
 *  - 'm' numero di sottovettori
 *  - 'n' numero di elementi del dataset (righe di map)
 *  - 'dataset' insieme di punti
 *  - 'map' mappa di corrispondenza punto-centroide associato 
 *  - 'k' numero di centroidi
 *  - 'codebook' lista di centroidi
 * descrizione:
 * - genera un set di nuovi centroidi calcolando la media degli elementi che gli sono associati
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
    float*nc;
    nc=(float*)(malloc(sizeof(float)*d));

    for( i=0; i<k; i++){ // per ogni centroide
        #ifdef DEBUG_NUOVICENTROIDI
            printf("analisi centroide i=%i\n",i);
        #endif
        for(z=0;z<d;z++){
            nc[z]=0;
        }
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
            for(z=0;z<dstar;z++){
                //possibile mettere sotto?
                codebook[i*d+w*dstar+z]=nc[w*dstar+z]/c;
            }//z
        }//w
    }//i

    #ifdef DEBUG_NUOVICENTROIDI
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'nuovicentroidi' #####\n");
    #endif

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
int* pq(int d, int m, int k, float *codebook, int n, float *dataset){
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

        TODO passare mappa da aggiornare, non creare qua
    */

   int dstar=d/m; //numero elementi sottovettori
    int i,j,w;
    int *map=(int*)malloc(sizeof(int) * n*m);
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

   return map;
}





void k_means( int d, int m, float eps, int tmin, int tmax, int k, float* codebook, int n, float* dataset){

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
    int t=0;
    float delta=0;
    
    double ob=0; 
    double nuovo_ob=0;
    int * map;

    do{
        map=pq(d,m,k,codebook,n,dataset);
        #ifdef DEBUG_KMEANS
            printf("stampa MAP al passo t=%i\n",t);
            printmi(m,n,map);
            printf("\n");
        #endif
        nuovicentroidi(d,m,n,dataset,map,k,codebook); 
        #ifdef DEBUG_KMEANS
            printf("stampa centroidi al passo t=%i\n",t+1);
            printmf(d,k,codebook);
            printf("\n");
        #endif
    }while(t++<tmin-1);
    ob=obiettivo(d,m,n,dataset,map,codebook);
    //l'ultima volta il ciclo lo facciamo al di fuori del while
    map=pq(d,m,k,codebook,n,dataset);
    #ifdef DEBUG_KMEANS
        printf("stampa RES al passo t=%i\n",t);
        printmi(m,n,map);
        printf("\n");
    #endif
    nuovo_ob=obiettivo(d,m,n,dataset,map,codebook);
    #ifdef DEBUG_KMEANS
        printf("stampa obiettivo passo t=%i\nob=%f\n",t,nuovo_ob);
    #endif
    t++;
    delta=(float)(ob-nuovo_ob);
        
    // abbiamo fatto il numero minimo di passi, andiamo alla seconda condizione
    while(tmax>=t++ && delta>eps ){
        nuovicentroidi(d,m,n,dataset,map,k,codebook); 
        #ifdef DEBUG_KMEANS
            printf("stampa centroidi al passo t=%i\n",t-1);
            printmf(d,k,codebook);
            printf("\n");
        #endif
        map=pq(d,m,k,codebook,n,dataset);
        #ifdef DEBUG_KMEANS
            printf("stampa RES al passo t=%i\n",t-1);
            printmi(m,n,map);
            printf("\n");
        #endif
        nuovo_ob= obiettivo(d,m,n,dataset,map,codebook);
        #ifdef DEBUG_KMEANS
            printf("stampa obiettivo passo t=%i\nob=%f\n",t-1,nuovo_ob);
        #endif
        delta=(float)(ob-nuovo_ob);
        ob=nuovo_ob;
    }

    free(map);

}



/**
 * args:
 * -i : indice primo centroide
 * -j :  indice secondo centroide
 * -k : numero di centroidi
 * 
 * descrizione:
 * -tirava fuori l'indice di riga della struttura che accoppia a due centroidi la loro distanza
 * 
 */ 
int cindex(int i,int j,int k){

    if(j<i){
        int tmp=i;
        i=j;
        j=tmp;
    }

    int index=0;
    for(int c=0;c<i;c++){
        index+=k-c-1;
    }

    return index+(j-i-1);
}

void merge(double* values, int* indices, int start, int mean, int end, int offset) {

    #ifdef DEBUG_MERGE
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'kmeans' #####\n");
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
            bi[k] = indices[i];
            i++;
        } else {
#ifdef DEBUG_MERGE
    printf("not passed\ncopia dei valori:\nb[%i]=%lf\nbi[%i]=%i",k,values[offset+j],k,indices[j]);
#endif
            b[k] = values[offset+j];
            bi[k] = indices[j];
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
        bi[k] = indices[i];
#ifdef DEBUG_MERGE
    printf("copia del valori i-esimi: %lf per values e %i per indices\n",values[offset+i],indices[i]);
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
        bi[k] = indices[j];
#ifdef DEBUG_MERGE
    printf("copia del valori j-esimi: %lf per values e %i per indici\n",values[offset+j],indices[j]);
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
        indices[k]=bi[k-start];
    }
}

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
 * -d= dimensione del vettore
 * -m=numero di sottovettori
 * -nd=numero distanze
 * -distanze=distanze tra tutti i punti del codebook
 * -K=numero elementi in ANN
 * -ANN=risultati (set già inizializzato)
 * -n=numero di elementi nel dataset
 * -map=dataset
 * -ix= indice del punto query, per memorizzarlo in ANN
 * -icent=il punto query quantizzato (contiene una lista di indici a cui è associato il centroide corrispondente)
 * 
 * descrizione:
 * implementa la distanza simmetrica con un singolo punto
 */
void SDC (int d, int k, int m, int nrd, double* distanze, int K, int*ANN, int n, int* map, int ix, int *qx){
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
   
    int j=0,w,idist, imax=0;
    double sommaparz=0,vmax,tmp;

    double ANN_values[K]; //valori del singolo punto;

    //primo giro va fatto a parte

    for(w=0;w<m;w++){
        #ifdef DEBUG_SDC
            printf("qx[%i]=map[%i*%i+%i] cioè %i=%i ?",w,j,m,w,qx[w],map[j*m+w]);
        #endif
        if(qx[w]!=map[j*m+w]){
            #ifdef DEBUG_SDC
                printf("NO\n");
            #endif
            idist=cindex(qx[w],map[j*m+w],k);
            sommaparz+=distanze[idist*m+w];
            #ifdef DEBUG_SDC
                printf("idist=%i\n",idist);
                printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
            #endif
        }
    }
    ANN[ix*K+j]=j;
    tmp=sqrt(sommaparz);
    ANN_values[j]=tmp;
    vmax=tmp;

    //abbiamo i centroidi associati al punto, adesso dobbiamo prendere per ogni punto un centroide e vedere se è il punto vicino 
    for(j=1;j<K;j++){
        sommaparz=0;
        //i primi K passi riempiamo il suo rispettivo ANN
       for(w=0;w<m;w++){
            #ifdef DEBUG_SDC
                printf("qx[%i]=map[%i*%i+%i] cioè %i=%i ?",w,j,m,w,qx[w],map[j*m+w]);
            #endif
            if(qx[w]!=map[j*m+w]){
                #ifdef DEBUG_SDC
                    printf("NO\n");
                #endif
                idist=cindex(qx[w],map[j*m+w],k);
                sommaparz+=distanze[idist*m+w];
                #ifdef DEBUG_SDC
                    printf("idist=%i\n",idist);
                    printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
                #endif
            }
       }
       ANN[ix*K+j]=j;
       tmp=sqrt(sommaparz);
       ANN_values[j]=tmp;

       
        if(vmax<tmp){
            vmax=tmp;
            imax=j;
        }
       

       //TODO RIPRENDERE DA QUI, DOPO CHE ABBIAMO FINITO DI RIEMPIRE KGRANDE VALORI DOBBIAMO INIZIARE CON GLI ALTRI CERCANDO SEMPRE QUELLO MINORE
    }

    for(;j<n;j++){
        sommaparz=0;
        for(w=0;w<m;w++){
            #ifdef DEBUG_SDC
                printf("qx[%i]=map[%i*%i+%i] cioè %i=%i ?",w,j,m,w,qx[w],map[j*m+w]);
            #endif
            if(qx[w]!=map[j*m+w]){
                #ifdef DEBUG_SDC
                    printf("NO\n");
                #endif
                idist=cindex(qx[w],map[j*m+w],k);
                sommaparz+=distanze[idist*m+w];
                #ifdef DEBUG_SDC
                    printf("idist=%i\n",idist);
                    printf("sommeparziali aumenta di: %lf\n prima era %lf\n\n",distanze[idist*m+w],sommaparz);
                #endif
            }
        }
       tmp=sqrt(sommaparz);

        if(vmax>tmp){
            //entrata in ANN del valore
            ANN[ix*K+imax]=j;
            ANN_values[imax]=tmp;

            //ricerca del nuovo massimo
            vmax=tmp;
            for(w=0;w<K;w++){
                if(vmax<ANN_values[w]){
                    vmax=ANN_values[w];
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
 * -d= dimensione del vettore
 * -m=numero di sottovettori
 * -k=numero centroidi
 * -codebook=centroidi
 * -K=numero elementi in ANN
 * -ANN=risultati (set già inizializzato)
 * -n=numero di elementi nel dataset
 * -map=dataset
 * -qs: struttura con punti query
 * 
 * descrizione:
 * -scrive in ANN i punti più vicini a x usando l'algoritmo di distanza simmetrica
 */
void ANNSDC(int d, int m, int k, float* codebook, int K, int*ANN, int n, int*map, int nq, float*qs){
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
    int c=0,i,j,w;
    /**
     */ 
    int icent[m];
    /**
     * la matrice distanze e' una matrice che per ogni riga contiene m distanze
     *
     */
    double distanze[nrd*ncd];

    for( i=0; i<k;i++){
        //centroide i
        for( j=i+1;j<k;j++){
            //centroide j
            #ifdef DEBUG_ANNSDC
                printf("distanza tra i=%i e j=%i\n",i,j);
            #endif
            for(w=0;w<m;w++){
                #ifdef DEBUG_ANNSDC
                    printf("sottovettore %i\n",w);
                #endif
                distanze[c++]=(double)(dist_2(dstar,codebook,d*i+w*dstar,codebook,d*j+w*dstar)); //distanze al quadrato
                #ifdef DEBUG_ANNSDC
                    printf("distanze[%i]=%lf\n\n",c-1,distanze[c-1]);
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
        SDC(d,k,m,nrd,distanze,K,ANN,n,map,i,icent);
    }
    #ifdef DEBUG_ANNSDC
        printf("\n\n#### FINE SEQUENZA DI DEBUG DEL METODO 'ANNSDC' #####\n");
    #endif

}//ANNSDC