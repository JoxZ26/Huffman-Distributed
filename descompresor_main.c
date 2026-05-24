#include <stdio.h>
#include "descompresor.h"
int main(int argc,char *argv[]) {
    if (argc < 4) {
        printf("Faltan o sobran argumentos");
        return 1;
    }
    buildDescompresor(argv[1],argv[2],argv[3]);
    printf("Archivo descomprimido\n");
    return 0;
}
