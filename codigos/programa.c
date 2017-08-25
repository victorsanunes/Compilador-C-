#include <stdio.h>
#include <stdlib.h>

void output(int n){
    printf("%d\n", n);
}

void main(void){
    int i;
    int media;
    int numeros[5];
    int n;
    n = 5;
    i = 1;
    media = 0;
    numeros[1] = 4;
    numeros[2] = 5;
    numeros[3] = 6;
    numeros[4] = 7;
    numeros[5] = 8; 
    while( i <= n){
        media = numeros[i] + media;
        i = i + 1;
    }

    media = media / n;
    output(media);

}



