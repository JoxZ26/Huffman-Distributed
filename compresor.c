#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

//DOMINIO: Un string input que representa el nombre del archivo
//y un arreglo de enteros con la frecuencia de cada carácter ASCII
void countFreq(const char *nombre, int freq[]) {
    FILE *file = fopen(nombre,"rb"); //read binary
    if (!file) {
        printf("No existe el archivo\n");
        return;
    }
    unsigned char c;
        while (fread(&c,1,1,file)) freq[c]++;
    fclose(file);
}

//DOMINIO: Un string input que representa el nombre del archivo de entrada,
//un string output que representa el nombre del archivo de salida, una matriz de chars donde se almacenan los códigos
// Huffman asociados a cada carácter, dos longs que representan el inicio y final del chunk a comprimir.
void comprimir(const char *input, const char *output, char tabla[256][256], long inicio, long final) {
    FILE *in = fopen(input,"rb");
    FILE *out = fopen(output, "wb");

    if (!in || !out) return;
    fseek(in,inicio,SEEK_SET);

    unsigned char c;
    unsigned char buff = 0;
    int bitCount = 0;
    for (long i = inicio; i < final; i++) {
        if (fread(&c,1,1,in) != 1) break;
        char *code = tabla[c];
        for (int j = 0; code[j] != '\0'; j++) {
            buff <<= 1;
            if (code[j] == '1') buff |=1;
            bitCount++;
            if (bitCount == 8) {
                fwrite(&buff,1,1,out);
                buff = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        buff <<= (8 - bitCount);
        fwrite(&buff,1,1,out);
    }
    fclose(in);
    printf("Chunk comprimido: %s\n", output);
    fclose(out);
}
//Un string archivoTabla que representa el nombre del archivo de escritura y una matriz de chars
//donde se almacenan los códigos Huffman asociados a cada carácter.
void guardarTabla(const char *archivoTabla, char tabla[256][256]) {
    FILE *out = fopen(archivoTabla, "w");
    if (!out) {
        printf("Error creando tabla\n");
        return;
    }
    for (int i = 0; i < 256; i++) {
        if (tabla[i][0] != '\0') {
            fprintf(out,"%d %s\n", i, tabla[i]);
        }
    }
    fclose(out);
}
//DOMINIO: Un arreglo de enteros que almacena las frecuencias de los carácteres,
//un string input que representa el nombre del archivo de entrada,
//un string output que representa el nombre del archivo de salida y
//dos longs que representan el inicio y final del chunk a comprimir.
void buildAll(int freq[], const char *input, const char *output, long inicio, long fin) {
    Nodo *raiz = buildArbol(freq);
    char tabla[256][256] = {0};
    char codigoActual[256];
    codigosHuffman(raiz,codigoActual,0,tabla);
    comprimir(input,output,tabla, inicio, fin);
}

//DOMINIO: Un arreglo de enteros que almacena las frecuencias de los carácteres.
void crearTabla(int freq[]) {
    Nodo *raiz = buildArbol(freq);
    char tabla[256][256] = {0};
    char codigoActual[256];
    codigosHuffman(raiz,codigoActual,0, tabla);
    guardarTabla("tabla.huff", tabla);
}



