#ifdef DEBUG
    //#define DEBUG_PRINTFV
    //#define DEBUG_PRINTDV
    //#define DEBUG_PRINTMF
    //#define DEBUG_PRINTMI
#endif


/**
 * argomenti:
 *  - 'd' numero di celle da stampare
 *  - 'v' vettore da cui prelevare le celle
 *  - 'vi' indice iniziale di v ( non sarà moltiplicato)
 * descrizione:
 *  FLOAT: stampa d celle del vettore v passato come parametro a partire da vi
 */
void printfv(int d,float *v,int vi){
    #ifdef DEBUG_PRINTFV
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printfv' #####\n");
    #endif

    printf("[");
    for(int i=0; i< d-1;i++){
        printf("%f,",v[vi+i]);
    }
    printf("%f",v[vi+d-1]);
    printf("]\n");
}

/**
 * argomenti:
 *  - 'd' numero di celle da stampare
 *  - 'v' vettore da cui prelevare le celle
 *  - 'vi' indice iniziale di v ( non sarà moltiplicato)
 * descrizione:
 *  DOUBLE: stampa d celle del vettore v passato come parametro a partire da vi
 */
void printdv(int d,double *v,int vi){
    #ifdef DEBUG_PRINTDV
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printdv' #####\n");
    #endif

    printf("[");
    for(int i=0; i< d-1;i++){
        printf("%lf,",v[vi+i]);
    }
    printf("%lf",v[vi+d-1]);
    printf("]\n");
}

/**
 * argomenti:
 *  - 'd' numero di celle da stampare
 *  - 'v' vettore da cui prelevare le celle
 *  - 'vi' indice iniziale di v ( non sarà moltiplicato)
 * descrizione:
 *  INT: stampa d celle del vettore v passato come parametro a partire da vi
 */
void printiv(int d,int *v,int vi){
    

    printf("[");
    for(int i=0; i< d-1;i++){
        printf("%i,",v[vi+i]);
    }
    printf("%i",v[vi+d-1]);
    printf("]\n");
}


/**
 * argomenti:
 *  - 'd' dimensione di ogni vettore
 *  - 'n' numero vettori
 *  - 'm' matrice da stampare
 * descrizione:
 *  stampa un intera matrice di float divisia in n vettori e ognuno di dimensione d
 */
void printmf(int d, int n, float *m){
    #ifdef PRINTMF
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printmf' #####\n");
    #endif
    //printf("%05.1f\n", myVar);  // Total width 5, pad with 0, one digit after .
    int i=0,j=0;
    for(;i<n;i++){
        printf("|");
        for(j=0;j<d-1;j++){
            printf("%+f,",m[i*d+j]);
        }
        printf("%+f|\n",m[i*d+d-1]);
    }
}

/**
 * argomenti:
 *  - 'd' dimensione di ogni vettore
 *  - 'n' numero vettori
 *  - 'm' matrice da stampare
 * descrizione:
 *  stampa un intera matrice di int divisia in n vettori e ognuno di dimensione d
 */
void printmi(int d, int n, int *m){
    #ifdef PRINTMI
        printf("\n\n#### INIZIO SEQUENZA DI DEBUG DEL METODO 'printmi' #####\n");
    #endif
   
    int i=0,j=0;
    for(;i<n;i++){
        printf("|");
        for(j=0;j<d-1;j++){
            printf("%+i,",m[i*d+j]);
        }
        printf("%+i|\n",m[i*d+d-1]);
    }
}



void matprintmi(int d, int n, int*matrix){
    int i=0,j=0;
    printf("[");
    for(;i<n-1;i++){
        printf("[");
        for(j=0;j<d-1;j++){
            printf("%i,",matrix[i*d+j]);
        }
        printf("%i],",matrix[i*d+d-1]);
    }
    printf("[");
    for(j=0;j<d-1;j++){
            printf("%i,",matrix[i*d+j]);
    }
    printf("%i]]",matrix[i*d+d-1]);
}
