#ifndef HUFFMAN_DESCOMPRESOR_H
#define HUFFMAN_DESCOMPRESOR_H
#include "huffman.h"

typedef struct {
    unsigned char caracter;
    char code[256];
} Codigo;

int leerTabla(const char *archivoTabla, Codigo Tabla[]);
Nodo* reconstruirArbol(Codigo tabla[], int n);
Nodo* reconstruirArbol(Codigo tabla[], int n);
void descomprimir(const char *input, const char *output, Nodo *raiz);
void buildDescompresor(const char *input, const char *output, const char *archivoTabla);
#endif

