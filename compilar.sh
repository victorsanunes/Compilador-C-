flex cmenos.l
bison -d cmenos.y
gcc -c *.c
gcc -o cminus *.o -ly -lfl

