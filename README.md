Memory Eater
============
This utility uses `mmap()` and `mlock()` to eat up RAM memory. This prevents the kernel for using this memory for more useful purposes, like page caching. The utility can be used as an alternative to the `mem=` kernel command-line, when page caching stands in the way of testing disk performance.

Instalation
-----------
The program is very simple and depends only requires a C compile and POSIX headers. On Ubuntu the typing the following command will suffice to install all dependencies:

    sudo apt-get install build-essential

A Makefile is provided for convenience:

    make

Example Usage
-------------
This example uses [iozone](http://www.iozone.org/) to show how to make file-system opperations disk-bound. The experiments have been conducted on a machine with 8GB of RAM. We first execute:

    iozone -s 1g -r 256k -i 0 -i 2

which returns a random read throughput of **4.8 GB/s**. Clearly, most reads have been **served from RAM**. Now we eat 5GB of memory:

    sudo ./memeat 5G

and re-run the same `iozone` command. We now obtain a random read throughput of **22 MB/s**. This is the expected random read throughput of the disk, thus, we can conclude that most of the reads have been **served from disk**.

Notes
-----
The Linux kernel limits the amount of memory that can be `mlock()`. If you run this utility as an unprileged user, you will most likely get the following error:

    mlock() failed: Cannot allocate memory

Therefore, this utility is best run as root.

