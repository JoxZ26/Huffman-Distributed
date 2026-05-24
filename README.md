This repository contains a distributed implementation of the Huffman algorithm for compression and decompression of any type of file.

The main idea of this program is to distribute across different computers the tasks related to:

* frequency calculation,
* Huffman tree generation,
* and file compression.

The system uses IP addresses and TCP sockets to connect different computers and distribute the workload between multiple nodes in the network.

The client divides the original file into chunks depending on the number of available nodes. Each server receives a chunk, calculates frequencies, compresses the data using Huffman coding, and sends the compressed chunk back to the client.

Finally, the client merges all compressed chunks and generates the final compressed file.

The repository also includes a sequential decompressor capable of reconstructing the original file using the generated compressed file and the Huffman table.

The Huffman algorithm guarantees, in the worst case, to preserve the size of the original file while maintaining lossless compression.

This distributed version aims to improve performance by parallelizing the computation across multiple nodes, allowing the analysis of the real impact and benefits of parallelism and distributed processing.
