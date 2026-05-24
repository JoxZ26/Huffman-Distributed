#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared.h"
#include "compresor.h"

#define MAX_NODOS 100

int sendAll(int sock, void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = send(sock,(char*)buffer + total,size - total,0);
        if (n <= 0)
            return -1;
        total += n;
    }
    return total;
}

int recvAll(int sock, void *buffer, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = recv(sock,(char*)buffer + total,size - total,0);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

void *workerThread(void *arg) {
    NodoThread *data = (NodoThread *)arg;

    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        pthread_exit(NULL);
    }

    char ip[64];
    int port;
    sscanf(data->ip,"%[^:]:%d",ip,&port);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET,ip,&serv_addr.sin_addr) <= 0) {
        printf("IP invalida: %s\n", ip);
        pthread_exit(NULL);
    }
    if (connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        printf("No se pudo conectar a %s\n", data->ip);
        pthread_exit(NULL);
    }
    sendAll(sock,&data->task,sizeof(Tarea));
    FILE *f = fopen(data->archivo,"rb");

    if (!f) {
        printf("No se pudo abrir archivo\n");
        close(sock);
        pthread_exit(NULL);
    }

    fseek(f,data->task.inicio,SEEK_SET);

    unsigned char buffer[1024];
    long restantes = data->task.chunkSize;
    while (restantes > 0) {
        int leer = restantes < sizeof(buffer) ? restantes : sizeof(buffer);

        int bytes = fread(buffer,1,leer,f);
        if (bytes <= 0)break;
        sendAll(sock,buffer,bytes);
        restantes -= bytes;
    }

    fclose(f);
    recvAll(sock,data->freq,sizeof(data->freq));
    data->sock = sock;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso:\n");
        printf("./cliente archivo salida ips.txt\n");
        return 1;
    }

    FILE *ipsFile = fopen(argv[3],"r");
    if (!ipsFile) {
        printf("No se pudo abrir ips.txt\n");
        return 1;
    }

    char ips[MAX_NODOS][64];
    int numIps = 0;
    while (fgets(ips[numIps],sizeof(ips[numIps]),ipsFile)) {
        ips[numIps][strcspn(ips[numIps],"\n")] = '\0';
        numIps++;
    }
    fclose(ipsFile);
    if (numIps == 0) {
        printf("No hay IPs\n");
        return 1;
    }

    FILE *f = fopen(argv[1],"rb");
    if (!f) {
        printf("No se pudo abrir archivo\n");
        return 1;
    }

    fseek(f,0,SEEK_END);
    long size = ftell(f);
    fclose(f);

    long chunk = size / numIps;
    pthread_t threads[numIps];
    NodoThread data[numIps];

    for (int i = 0; i < numIps; i++) {
        strcpy(data[i].ip,ips[i]);
        strcpy(data[i].archivo,argv[1]);

        data[i].task.inicio = i * chunk;
        if (i == numIps - 1) data[i].task.fin = size;
        else data[i].task.fin = data[i].task.inicio + chunk;
        data[i].task.chunkSize = data[i].task.fin - data[i].task.inicio;

        printf("Nodo %d (%s): %ld -> %ld\n", i, data[i].ip, data[i].task.inicio, data[i].task.fin);
        pthread_create(&threads[i],NULL,workerThread,&data[i]);
    }

    for (int i = 0; i < numIps; i++) {
        pthread_join(threads[i],NULL);
    }

    int freqGlobal[256] = {0};
    for (int i = 0; i < numIps; i++) {
        for (int j = 0; j < 256; j++) {
            freqGlobal[j] += data[i].freq[j];
        }
    }
    printf("Frecuencias mergeadas\n");

    for (int i = 0; i < numIps; i++) {
        sendAll(data[i].sock,freqGlobal,sizeof(freqGlobal));
    }

    for (int i = 0; i < numIps; i++) {
        long chunkSize;
        recvAll(data[i].sock,&chunkSize,sizeof(long));
        sprintf(data[i].chunkName,"chunk_%ld.bin",data[i].task.inicio);

        FILE *chunk = fopen(data[i].chunkName,"wb");
        if (!chunk) {
            printf("No se pudo crear chunk local\n");
            close(data[i].sock);
            continue;
        }

        unsigned char buffer[1024];
        long recibidos = 0;

        while (recibidos < chunkSize) {
            long faltan = chunkSize - recibidos;

            int leer = faltan < sizeof(buffer) ? faltan : sizeof(buffer);
            int n = recv(data[i].sock,buffer,leer,0);

            if (n <= 0) break;
            fwrite(buffer,1,n,chunk);
            recibidos += n;
        }
        fclose(chunk);
        close(data[i].sock);
    }

    FILE *out = fopen(argv[2],"wb");
    if (!out) {
        printf("No se pudo crear salida\n");
        return 1;
    }

    fwrite(&size,sizeof(long),1,out);
    fwrite(&numIps,sizeof(int),1,out);

    for (int i = 0; i < numIps; i++) {
        FILE *chunkFile = fopen(data[i].chunkName,"rb");
        if (!chunkFile) {
            printf("No se pudo abrir %s\n",data[i].chunkName);
            continue;
        }

        fseek(chunkFile,0,SEEK_END);
        long chunkSize = ftell(chunkFile);
        rewind(chunkFile);
        fwrite(&chunkSize,sizeof(long),1,out);

        unsigned char buffer[1024];
        size_t bytes;

        while ((bytes = fread(buffer,1,sizeof(buffer),chunkFile)) > 0) {
            fwrite(buffer,1,bytes,out);
        }
        fclose(chunkFile);
    }

    fclose(out);
    printf("Archivo comprimido creado: %s\n", argv[2]);
    return 0;
}