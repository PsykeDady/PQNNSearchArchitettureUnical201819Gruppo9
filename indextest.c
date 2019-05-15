#include<stdio.h>
void somma(int *ris, int *a, int b){
    *ris=*a+b;
}

int cindex(int i,int j,int k){
    int index=0;
    for(int c=0;c<i;c++){
        index+=k-c-1;
    }

    return index+(j-i-1);
}

void main(){
    int k=20;
    int i=0;
    int j=0;
    int c=0;
    int index=0;

    for(i=0;i<k;i++){
        for(j=i+1;j<k;j++){
            index=cindex(i,j,k);
            printf("riga %3i, |%2i|%2i|\tindex=%i\n",c++,i,j,index);
        }
    }


    
}