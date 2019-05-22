#include <stdio.h>
#define DEBUG_MERGE
#define DEBUG_MERGESORT

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

int main (int argc, char*args){

    int vlength=8;
    double v[]={(double)(5.0), (double)(1.0), (double)(2.0), (double)(-1.0), (double)(60.0), (double)(432.0), (double)(2.0), (double)(321.0)};
    int ind[vlength];

    for(int i=0; i< vlength; i++){
      ind[i]=i+1;
    }

    printf("Valori inseriti!\n");
    for(int i=0;i<vlength;i++){
        printf("|%lf|",v[i]);
    }

    printf("\n");

    for(int i=0;i<vlength;i++){
        printf("|%i|",ind[i]);
    }

    printf("\n");

    mergeSort(v,ind,0,vlength-1,0);

    for(int i=0;i<vlength;i++){
        printf("|%lf|",v[i]);
    }

    printf("\n");

    for(int i=0;i<vlength;i++){
        printf("|%i|",ind[i]);
    }

    printf("\n");

    return 0;
}