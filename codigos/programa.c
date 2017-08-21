#include <stdio.h>
#include <stdlib.h>

void output(int n){
    printf("%d\n", n);
}

int pow(int base, int expoente){
    int i;
    int resultado = 1;
    i = 1;
    while(i <= expoente){
        resultado = base * resultado;
        i = i + 1;
    }

    return resultado;
}
void main(void){
    output(pow(3, 3));
}





