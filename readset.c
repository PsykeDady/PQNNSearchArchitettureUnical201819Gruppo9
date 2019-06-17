#include <stdio.h>
#include <stdlib.h>
#include "printutility.c"
#include "strutility.c"
int read_matrix(char*filename, int *nx, int *dx, float** matrix ){
    FILE*f;
    int d,n;
    printf("file: %s\n",filename);

    f=fopen(filename,"rb");

    if(f==NULL){
        fprintf(stderr, "E: file %s corrotto, inaccessibile o altri problemi I/O\n",filename);
        return -1;
    }

    fread(&d, sizeof(int), 1, f);
    // printf("d letto %i\n",d);
    *dx=d;
    fread(&n, sizeof(int), 1, f);
    // printf("n letto %i\n",n);
    *nx=n;


    *matrix=(float*)(malloc(sizeof(float)*n*d));

    fread(*matrix, sizeof(float), n*d, f);

   

    return fclose(f);

}


/**
 * 
 * questo file serve a leggere un dataset di prova
 * sintassi:
 * 
 * readset nomefilesenzaestensione
 */

int main(int argc, char**args){

    int d,n,nq;

    float *qs,*ds;

    if(argc!=2){
        fprintf(stderr,"errore...numero parametri (%i) non corretto (necessari: 1)\n",argc);
        return -1;
    }

    /** filename del ds */
    char*dsfn=cat(args[1],".ds");

    /** filename del qs */
    char*qsfn=cat(args[1],".qs");

    read_matrix(dsfn,&n,&d,&ds);

    read_matrix(qsfn,&nq,&d,&qs);

   printf("stampa matrice dati\nd=%i\nn=%i\n",d,n);
    printmf(d,n,ds);
    printf("\n-------- ---------\n");
    printf("stampa matrice query\nd=%i\nnq=%i\n",d,nq);
    printmf(d,nq,qs);

    free(ds);
    free(qs);

    return 0;
}
