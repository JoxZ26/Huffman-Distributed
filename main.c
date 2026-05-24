/* ENCABEZADO
 * Autor: Jose Andrés Villegas Murillo (2024089256)
 *
 * Detalle:
 *
 * El programa implementa un sistema distribuido de compresión Huffman utilizando sockets TCP e hilos en C.
 * El archivo original se divide en múltiples chunks (dependiendo de la cantidad de nodos) que son procesados
 * en paralelo por diferentes nodos en la red.
 *
 * El compresor está compuesto por:
 *
 * - Cliente: Coordina la compresión distribuida.
 * - Servidores: Calculan frecuencias y comprimen chunks del archivo.
 * - Huffman: Algoritmo de compresión sin pérdida.
 *
 * A su vez, se implementó un descompresor, el cual reconstruye el archivo original a partir del archivo
 * comprimido generado por el sistema distribuido Huffman.
 *
 *
 * Funcionamiento compresor:
 *
 * 1. Levantar los servidores:
 *
 *    Cada servidor debe ejecutarse indicando el puerto que se utilizará.
 *
 *    Ejemplo:
 *
 *    ./server 8080
 *    ./server 8081
 *
 *
 * 2. Configurar ips.txt:
 *
 *    El archivo contiene la dirección IP y puerto de cada nodo disponible,
 *    utilizando el formato IP:PUERTO.
 *
 *    Ejemplo:
 *
 *    127.0.0.1:8080
 *    127.0.0.1:8081
 *    192.168.100.227:8080
 *    192.168.100.227:8081
 *
 *
 * 3. Ejecutar el cliente:
 *
 *    El cliente recibe:
 *
 *    - archivo_entrada
 *    - comprimido.bin
 *    - ips.txt
 *
 *    Ejemplo:
 *
 *    ./cliente video.mp4 comprimido.bin ips.txt
 *
 *    El cliente genera:
 *
 *    - comprimido.bin:
 *      Archivo comprimido final.
 *    - tabla.huff:
 *      Archivo que almacena los códigos Huffman asociados a cada byte.
 *
 *
 * 4. Funcionamiento:
 *
 *    El cliente:
 *
 *    - Lee el archivo original.
 *    - Divide el archivo en chunks dependiendo de la cantidad de nodos.
 *    - Crea un hilo por nodo.
 *    - Envía cada chunk a un servidor.
 *
 *    Los servidores:
 *
 *    - Reciben el chunk.
 *    - Calculan frecuencias.
 *    - Envían frecuencias al cliente.
 *    - Reciben frecuencias globales.
 *    - Comprimen el chunk.
 *    - Envían el chunk comprimido al cliente.
 *
 *    Finalmente, el cliente:
 *
 *    - Recibe todos los chunks comprimidos.
 *    - Realiza el merge.
 *    - Genera el archivo comprimido.bin.
 *
 *
 * Funcionamiento descompresor:
 *
 * 1. Ejecutar el descompresor:
 *
 *    El descompresor recibe:
 *    - comprimido.bin
 *    - archivo de salida
 *    - tabla.huff
 *
 *    Ejemplo:
 *    ./descompresor_exec comprimido.bin salida.mp4 tabla.huff
 *
 *
 * 2. Funcionamiento:
 *
 *    El descompresor:
 *
 *    - Abre el archivo comprimido.
 *    - Reconstruye la información.
 *    - Reconstruye el árbol de Huffman.
 *    - Recorre el árbol de Huffman según cada bit leído.
 *    - Reconstruye los bytes originales.
 *    - Genera el archivo final reconstruido.
 *
 */



int main(int argc, char *argv[]) {
    return 0;
}
