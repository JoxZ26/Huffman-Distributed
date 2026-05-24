#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

//DOMINIO: Un carácter c y la frecuencia de ese carácter.
//CODOMINIO: Un puntero a una estructura Nodo.
Nodo* crearNodo(char c, int freq) {
    Nodo* nodo = malloc(sizeof(Nodo));
    nodo -> caracter = c;
    nodo -> freq = freq;
    nodo -> izq = NULL;
    nodo -> der = NULL;
    return nodo;
}

//DOMINIO: Un entero cap, que representa la capacidad o cuantos elementos puede almacenar el heap.
//CODOMINIO: Un puntero a una estructura MinHeap
MinHeap* crearHeap(int cap) {
    MinHeap* heap = malloc(sizeof(MinHeap));
    heap -> size = 0;
    heap -> cap = cap;
    heap -> array = malloc(sizeof(Nodo*) * cap);
    return heap;
}

//DOMINIO: Un puntero a un puntero Nodo a y un puntero a un puntero Nodo b.
void swap(Nodo **a, Nodo **b) {
    Nodo *temp = *a;
    *a = *b;
    *b = temp;
}

//DOMINIO: Un puntero a un Heap y un puntero a un nodo.
void insertHeap(MinHeap *heap, Nodo *nodo) {
    if (heap -> size == heap -> cap) return;
    heap -> size++;
    int i = heap -> size - 1;
    heap -> array[i] = nodo;

    while (i != 0 && heap -> array[(i-1)/2] -> freq > heap -> array[i] -> freq) {
            swap(&heap -> array[i], &heap -> array[(i-1)/2]);
            i = (i-1)/2;
    }
}

//DOMINIO: Puntero a una estructura heap y un entero i (indice del elemento).
void heapify(MinHeap *heap, int i) {
    int act = i;
    int izq = 2*i + 1;
    int der = 2*i + 2;

    if (izq < heap -> size && heap -> array[izq]->freq < heap -> array[act] -> freq) act = izq;
    if (der < heap -> size && heap -> array[der]->freq < heap -> array[act] -> freq) act = der;
    if (act != i) {
        swap(&heap -> array[i], &heap -> array[act]);
        heapify(heap, act);
    }
}

//DOMINIO: Un puntero a una estructura heap.
//CODOMINIO: Un puntero a el Nodo con la frecuencia mínima.
Nodo* extractMin(MinHeap *heap) {
    if (heap -> size <= 0) return NULL;
    Nodo* min = heap -> array[0];
    heap -> array[0] = heap -> array[heap -> size - 1];
    heap -> size--;
    heapify(heap,0);
    return min;
}

//DOMINIO: Un arreglo de enteros.
//CODOMINIO: Un puntero a el Nodo raíz de un arbol.
Nodo* buildArbol(int frecuencias[]) {
    MinHeap *heap = crearHeap(256);
    for (int i = 0; i < 256; i++) {
        if (frecuencias[i] > 0){
            Nodo *nuevo = crearNodo(i,frecuencias[i]);
            insertHeap(heap,nuevo);
        }
    }
    while (heap -> size > 1) {
        Nodo *min1 = extractMin(heap);
        Nodo *min2 = extractMin(heap);
        Nodo *padre = crearNodo('\0', min1 -> freq + min2 -> freq);
        padre -> izq = min1;
        padre -> der = min2;
        insertHeap(heap, padre);
    }
    return extractMin(heap);
}

void printArbol(Nodo *raiz) {
    if (raiz == NULL) return;
    printf("char: %c freq: %d\n", raiz->caracter,raiz->freq);
    printArbol(raiz->izq);
    printArbol(raiz->der);
}

//DOMINIO: Puntero a la raiz del árbol (o subárbol) de Huffman, un arreglo de chars que almacena el codigo binario generado,
// la profundidad actual del recorrido en el arbol y una matriz de chars donde se almacenan los códigos
// Huffman asociados a cada carácter.
void codigosHuffman(Nodo *raiz, char codigoActual[], int profundidad, char tabla[256][256]) {
    if (raiz == NULL) return;
    if (raiz -> izq) {
        codigoActual[profundidad] = '0';
        codigosHuffman(raiz -> izq, codigoActual, profundidad + 1, tabla);
    }
    if (raiz -> der) {
        codigoActual[profundidad] = '1';
        codigosHuffman(raiz -> der, codigoActual, profundidad + 1, tabla);
    }
    if (raiz -> izq == NULL && raiz -> der == NULL) {
        codigoActual[profundidad] = '\0';
        strcpy(tabla[(unsigned char) raiz -> caracter], codigoActual);
    }
}
