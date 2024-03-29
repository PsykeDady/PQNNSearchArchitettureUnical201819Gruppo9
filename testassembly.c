#include <stdio.h>
#include <xmmintrin.h>
#include <math.h>

#define D 5

/* float dist_2_asm(int d, float *x, int xi, float *y, int yi){
        float somma=0, differenza=0;
        for(int i=0;i<d;i++){
          differenza=x[xi+i]-y[yi+i];
          differenza*=differenza;
          somma+=differenza;
       }
       return somma;
    } */
extern float dist_2_asm(int, float*, int , float *, int );

//extern float diffvf_asm(int, float*, int , float *, int, float *, int);

float dist(int d, float *x, int xi, float *y, int yi){
    return sqrt(dist_2_asm(d,x,xi,y,yi));
}

/* void differenza(int d, float *x, int xi, float *y, int yi, float *res, int ri){
    diffvf_asm(d,x,xi,y,yi,res,ri);
} */


int main (int argc, char** args){

    //float v11[D]={3,10,9,13,22,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
   // float v2[D]={3.5,-4.2,3,-1.5,1.0,1,1,0,1.5,4,-4.2,-1.5,1.0,1,2,0,1.5,-4.2,-1.5,1.0,1,1,10,1.5,-4.2,3,-1.5,1.0,1,1,0,4.5,5};
    //float v21[D]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64};

    /* float *v1=_mm_malloc(D*sizeof(float),4);
    float *v2=_mm_malloc(D*sizeof(float),4); */
    float *v1=(float*)(malloc(D*sizeof(float)));
    float *v2=(float*)(malloc(D*sizeof(float)));
    //float *v3=(float*)(malloc(D*sizeof(float)));

    register int i =0;


    for(;i<D;i++){
        v1[i]=i+2;
        v2[i]=i;
    }
    

    printf("distanza=%f\n",dist(D,v1,0,v2,0));

    /* differenza(D,v1,0,v2,0,v3,0);

    for(i=0;i<D;i++){
        printf("res[%i]=%f ", i, v3[i]);
    }
    printf("\n");
 */
}