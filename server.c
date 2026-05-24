#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "compresor.h"
#include "shared.h"

#define PORT 8080


int sendAll(int sock, void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = send(sock, (char*)buffer + total, size - total,0);
        if (n <= 0)return -1;
        total += n;
    }
    return total;
}

int recvAll(int sock, void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = recv(sock, (char*)buffer + total, size - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

int main(int argc, char *argv[]) {
    int port = PORT;
    if (argc > 1) port = atoi(argv[1]);

    int server_fd;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket falló");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind falló");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen falló");
        exit(EXIT_FAILURE);
    }

    printf("Nodo esperando conexiones en puerto %d...\n", port);

    int cliente_sock;
    while ((cliente_sock = accept(server_fd,NULL,NULL)) >= 0) {
        printf("Cliente conectado\n");

        Tarea task;
        if (recvAll(cliente_sock,&task, sizeof(task)) <= 0) {
            close(cliente_sock);
            continue;
        }

        printf("Chunk recibido: %ld -> %ld\n", task.inicio, task.fin);

        char tempName[64];
        sprintf(tempName, "temp_%d_%ld.bin", getpid(), task.inicio);
        FILE *f = fopen(tempName,"wb");

        if (!f) {
            printf("No se pudo crear archivo temporal\n");
            close(cliente_sock);
            continue;
        }

        unsigned char buffer[1024];
        long recibidos = 0;

        while (recibidos < task.chunkSize) {
            int leer = (task.chunkSize - recibidos) < sizeof(buffer) ? (task.chunkSize - recibidos): sizeof(buffer);
            int n = recv(cliente_sock, buffer, leer, 0);
            if (n <= 0) break;

            fwrite(buffer,1,n,f);
            recibidos += n;
        }

        fclose(f);
        int freq[256] = {0};

        FILE *in = fopen(tempName,"rb");
        if (!in) {
            printf("No se pudo abrir temporal\n");
            close(cliente_sock);
            continue;
        }

        unsigned char c;
        while (fread(&c,1,1,in)) freq[c]++;
        fclose(in);

        sendAll(cliente_sock, freq, sizeof(freq));

        printf("Frecuencias enviadas\n");

        int freqGlobal[256];
        if (recvAll(cliente_sock, freqGlobal, sizeof(freqGlobal)) <= 0) {
            close(cliente_sock);
            continue;
        }

        printf("Frecuencias globales recibidas\n");

        char output[64];

        sprintf(output, "chunk_%d_%ld.bin", getpid(), task.inicio);

        buildAll(freqGlobal, tempName,output,0,task.chunkSize);
        FILE *chunkFile = fopen(output,"rb");

        if (!chunkFile) {
            printf("No se pudo abrir chunk\n");
            close(cliente_sock);
            continue;
        }

        fseek(chunkFile,0,SEEK_END);
        long chunkSize = ftell(chunkFile);
        rewind(chunkFile);
        sendAll(cliente_sock,&chunkSize, sizeof(long));

        size_t bytes;
        while ((bytes =fread(buffer,1,sizeof(buffer),chunkFile)) > 0) {
            sendAll(cliente_sock,buffer, bytes);
        }

        fclose(chunkFile);
        printf("Chunk comprimido enviado\n");
        remove(tempName);
        remove(output);

        close(cliente_sock);
    }
    close(server_fd);
    return 0;
}

