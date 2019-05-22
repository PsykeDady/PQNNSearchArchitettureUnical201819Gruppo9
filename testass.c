#include <stdio.h>

void assegna(int *arr, int length){
    int patata[length];

    for(int i=0;i<length;i++){
        patata[i]=i;
    }

    
}


int main (int argc, char** args){

    int length=3;
    int arr[length];

    assegna(arr,length);

    for(int i=0;i<length;i++){
        printf("%i ",arr[i]);
    }

    printf("\n");

    return 0;
}