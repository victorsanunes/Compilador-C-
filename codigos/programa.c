#include <stdio.h>
#include <stdlib.h>

void output(int n){
    printf("%d\n", n);
}

int calculaFatorial(int n){
    int i;
    int fatorial;
    int resultado;
    fatorial = n;
    resultado = 1;
    i = n;
    while(i >= 1){
        resultado = fatorial * resultado;
        i = i - 1;
        fatorial = fatorial - 1;
    }
    return resultado;
}

void main(void){
    int resultado;
    resultado = calculaFatorial(5);
    output(resultado);
}




