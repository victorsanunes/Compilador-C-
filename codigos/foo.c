#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int vetor[2];
int foo(int x, int y, int z, int w){
    int result;
    result = 0;
    while(result >= 0){
        
        if(x < 2){
            puts("x<2");
            result = x + y;
        }
        if(y > 2){
            puts("y>2");
            result = result + y + z;
        }
        else{
            puts("else");
            result = result * 3;
        }
        if(w <= 4){
            puts("w<=4");
            if(z == 0){
                puts("z==0");
                return 0;
            }
            if(result != 0){
                puts("result != 0");
                printf("result = %d", result);
                return result/(w - 1);
            }
        }
    }
}
void main(void){
    scanf("%d", &vetor[0]);
    vetor[1] = foo(1, vetor[0], 3, 4);
    printf("\nfoo: %d", vetor[1]);
}
