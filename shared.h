
#ifndef HUFFMAN_SHARED_H
#define HUFFMAN_SHARED_H

typedef struct {
    long inicio;
    long fin;
    long chunkSize;
} Tarea;

typedef struct {
    char ip[32];
    char archivo[256];
    Tarea task;
    int freq[256];
    char chunkName[64];
    int sock;

} NodoThread;

#endif //HUFFMAN_SHARED_H
