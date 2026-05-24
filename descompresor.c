#include <stdio.h>
#include "huffman.h"
#include "descompresor.h"

//DOMINIO: Un string archivoTabla que representa el nombre del archivo que contiene la tabla de huffman y
// un arreglo de estructuras Codigo tabla, donde se almacenan los carácteres y sus códigos de huffman.
//CODOMINIO: Entero que representa la cantidad de digitos leídos desde el archivo.
int leerTabla(const char *archivoTabla, Codigo tabla[]) {
    FILE *in = fopen(archivoTabla, "r");
    if (!in) {
        return 0;
    }
    int cont = 0;
    while (fscanf(in, "%hhu %s", &tabla[cont].caracter, tabla[cont].code) == 2) cont++;
    fclose(in);
    return cont;
}

//DOMINIO: Un arreglo de estructuras Codigo tabla, donde se almacenan los carácteres y sus códigos de huffman y
// un entero n con la cantidad de dígitiso leídos desde el archivo.
//CODOMINIO: Puntero a la raíz del arbol de Huffman reconstruido.
Nodo* reconstruirArbol(Codigo tabla[], int n) {
    Nodo *raiz = crearNodo('\0',0);
    for (int i = 0; i < n; i++) {
        Nodo *actual = raiz;
        for (int j = 0; tabla[i].code[j] != '\0'; j++) {
            if (tabla[i].code[j] == '0') {
                if (!actual->izq)
                    actual->izq = crearNodo('\0',0);
                actual = actual->izq;
            }
            else {
                if (!actual->der)
                    actual->der = crearNodo('\0',0);
                actual = actual->der;
            }
        }
        actual->caracter = tabla[i].caracter;
    }

    return raiz;
}
//DOMINIO: Un string input que representa el nombre del archivo de entrada,
//un string output que representa el nombre del archivo de salida y
//un puntero al Nodo raíz del arbol de huffman
void descomprimir(const char *input,const char *output,Nodo *raiz) {

    FILE *in = fopen(input, "rb");
    FILE *out = fopen(output, "wb");

    if (!in || !out) return;

    long oSize;
    int numChunks;
    fread(&oSize,sizeof(long),1,in);
    fread(&numChunks,sizeof(int),1,in);

    Nodo *actual = raiz;
    unsigned char byte;
    long escritos = 0;

    for (int c = 0; c < numChunks; c++) {
        long chunkSize;
        fread(&chunkSize,sizeof(long),1, in);
        actual = raiz;
        for (long j = 0; j < chunkSize; j++) {
            fread(&byte,1,1,in);
            for (int i = 7; i >= 0; i--) {
                int bit = (byte >> i) & 1;
                if (bit == 0)actual = actual->izq;
                else actual = actual->der;

                if (actual->izq == NULL && actual->der == NULL) {
                    fwrite(&actual->caracter,1,1,out);
                    escritos++;
                    actual = raiz;
                    if (escritos >= oSize)
                        break;
                }
            }
            if (escritos >= oSize) break;
        }
    }
    fclose(in);
    fclose(out);
}

//DOMINIO: Un string input que representa el nombre del archivo de entrada,
//un string output que representa el nombre del archivo de salida y
//un string archivoTabla que representa el archivo que contiene la tabla de Huffman.
void buildDescompresor(const char *input, const char *output, const char *archivoTabla) {
    Codigo Tabla[256];
    int cont = leerTabla(archivoTabla, Tabla);
    Nodo *raiz = reconstruirArbol(Tabla, cont);
    descomprimir(input,output,raiz);
}


