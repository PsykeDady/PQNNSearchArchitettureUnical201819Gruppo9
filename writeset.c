#include <stdio.h>
#include <stdlib.h>
#include "strutility.c"
int write_matrix(char*filename, int n, int d, float* matrix ){
    FILE*f;
    f=fopen(filename,"wb");

    if(f==NULL){
        fprintf(stderr, "E: file corrotto, inaccessibile o altri problemi I/O\n");
        return -1;
    }

    fwrite(&d,sizeof(int),1,f);
    fwrite(&n,sizeof(int),1,f);

    fwrite(matrix,sizeof(float),n*d,f);
    

    

    return fclose(f);
}


/**
 * 
 * questo file serve a crearsi un proprio dataset di prova
 * sintassi:
 * 
 * writeset nomefilesenzaestensione
 */

int main(int argc, char**args){

    int d=10,
        n=15,
        nq=3;

    float ds[]={ 
        
        1,2,70,5,6,7,8,-1,1.5,-0.5,

        10,2,70,5,6,7,8,-1,1.5,-0.5,

        6,7,8,-1,-1,-1,2,2,3,4,

        5,6,10,10,7,7,6,5,4,4,

        4,4,5,6,7,7,7,5,4,9,

        3,3,-1,-1,.5,.5,1,5,5,6,

        1,2,3,4,5,6,7,8,9,10,

        10,9,8,7,6,5,4,3,2,1,

        1,1,1,1,1,1,1,1,1,1,

        -2,-2,0,0,0,0,0,0,0,0,

        3,7,8,9,10,11,12,11,11,11,

        1,2,3,1,2,3,1,2,3,1,

        1,1,1,1,1,1,1,1,1,0,

        0,1,2,3,-1,-2,-3,1,-2,3,

        0,-1,-2,-3,1,2,3,-1,2,-3,

    };

    float qs[]={

        2,3,4,5,6,7,8,9,10,11,

        5,6,11,11,7,7,6,5,4,4,

        -3,1,0,0,0,0,0,0,0,0,
    };

    if(argc!=2){
        fprintf(stderr,"errore...numero parametri non corretto\n");
        return -1;
    }

    /** filename del ds */
    char*dsfn=cat(args[1],".ds");

    /** filename del qs */
    char*qsfn=cat(args[1],".qs");

    write_matrix(dsfn,n,d,ds);

    write_matrix(qsfn,nq,d,qs);


    return 0;
}
