/* #include<stdlib.h>
#include<stdio.h> */
char* cat(char p[], char q[]) {
    int c=0, d=0;
    char* ns;

    while (p[c++] != '\0');
    c--;
    while(q[d++]!='\0');
    d--;

    ns=(char*)(malloc(sizeof(char)*(c+d+1)));

    for(int i=0;i<c;i++){
        ns[i]=p[i];
    }
    for(int i=0;i<d;i++){ns[c+i]=q[i];}

    ns[c+d]='\0';

    return ns;
}


/* void main(){
    printf("%s\n",cat("ciao ","dady"));
} */