#include <stdio.h>
#include <stdlib.h>

void output(int n){
    printf("%d\n", n);
}

void bubblesort(int array[], int length)
{
    int k;
    int j;
    int aux;

    k = length;
    while (k >= 0){
        j = 0;
        while (j < k)
        {
            if (array[j] > array[j + 1]){
                aux = array[j];
                array[j] = array[j + 1];
                array[j + 1] = aux;
            }
            j = j + 1;
        }
        k = k - 1;
    }
}

void main(void)
{
    int array[5];
    int length;
    int i;

    array[0] = 15;
    array[1] = 72;
    array[2] = 14;
    array[3] = 1;
    array[4] = 3;
    length = 5;
    i = 4;
    bubblesort(array, length);
    output(array[i]);
}




