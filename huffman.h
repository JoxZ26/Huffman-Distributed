#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H


typedef struct nodo {
    char caracter;
    int freq;
    struct nodo *izq;
    struct nodo *der;
} Nodo;

typedef struct minHeap {
    int size;
    int cap;
    Nodo** array; //puntero a punteros de Nodo
} MinHeap;

Nodo* crearNodo(char caracter, int freq);
MinHeap* crearHeap(int cap);
void insertHeap(MinHeap *heap, Nodo *nodo);
void heapify(MinHeap *heap, int i);
Nodo* extractMin(MinHeap *heap);
Nodo* buildArbol(int frecuencias[]);
void printArbol(Nodo *raiz);
void codigosHuffman(Nodo *raiz,char codigoActual[], int profundidad,char tabla[256][256]);

#endif









