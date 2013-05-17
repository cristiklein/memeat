`memeat`: A Memory Eater
========================
This utility uses `mmap()` and `mlock()` to eat up RAM. This prevents the kernel from using RAM for more useful purposes, like page caching. The utility can be used as an alternative to the `mem=` kernel command-line, when page caching stands in the way of testing disk performance.

Installation
-----------
Building `memeat` from source code is very simple and only requires a C compiler and POSIX headers. On Ubuntu, the following command will suffice to install all dependencies:

    sudo apt-get install build-essential

A `Makefile` is provided for convenience:

    make

Example Usage
-------------
This example uses [iozone](http://www.iozone.org/) to show how to make file-system operations disk-bound. The experiments have been conducted on a machine with 8GB of RAM and a Western Digital Caviar Blue Serial ATA disk. We first execute:

    iozone -s 1g -r 256k -i 0 -i 2

This measures a random read throughput of **4.8 GB/s**. Clearly, most reads have been **served from RAM**. If we now eat 5 GB of RAM

    sudo ./memeat 5G

and re-run the same `iozone` command, we obtain a random read throughput of **22 MB/s**. This is the expected random read throughput of the disk, thus, we can conclude that most of the reads have been **served from disk**.

Notes
-----
The Linux kernel limits the amount of memory that can be `mlock()`. If you run this utility as an unprivileged (non-root) user, you will most likely get the following error:

    mlock() failed: Cannot allocate memory

To fix this, either run `memeat` as root or change the resource limits by adding the following entries to `/etc/security/limits.conf`:

    myuser          soft     memlock         16000000
    myuser          hard     memlock         16000000

This will allow the user `myuser` to eat up to 16 GB of RAM.

Eating too much memory will freeze your system for a while, then, the [OOM killer](http://linux-mm.org/OOM_Killer) will kick in and most likely kill `memeat`.
