#include<stdio.h>
void somma(int *ris, int *a, int b){
    *ris=*a+b;
}

void main(){
    int a=0;
    int b=2;
    int ris=0;

    somma(&ris,&a,b);

    printf("%i\n",ris);
}