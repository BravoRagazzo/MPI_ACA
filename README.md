# Substring Search with application to Genomics

## Introduction
This project implements the Rabin Karp algorithm for substring search using the Message Passing Interface (MPI) library for parallel processing. The Rabin Karp algorithm is a string matching algorithm that uses a rolling hash function to efficiently search for a given pattern within a larger text. MPI is a library that allows for the parallel execution of programs across multiple processors or machines.

## Requirements
* C compiler (e.g. GCC)
* MPI library (e.g. OpenMPI)

## Usage
* To compile the program, use the following command: `mpicc -o rabin_karp_mpi rabin_karp_mpi.c`  
* To run the program, use the following command: `mpirun -n [number of processes] ./rabin_karp_mpi [pattern file] [text file]`

The program takes at most two command line arguments:

* `pattern file`: the file containing the string that will be searched within the text;
* `text file` : the file containing the text in which the pattern will be searched;
* Notice that all the txt file must be inside the  `dataset/` directory.

The program will output the positions at which the pattern was found within the text.

## Example
`mpirun -n 4 ./rabin_karp_mpi pat.txt text.txt`  
This command will run the program using 4 processes and search in the file "text.txt" for the substring contained in "pat.tx". 

## Note
The program assumes that the text file provided is small enough to be read into memory and distributed among the processes, and that the pattern provided is smaller than the text file.

## References
* Rabin, Michael O., and Richard M. Karp. "Efficient randomized pattern-matching algorithms." IBM journal of research and development 31.2 (1987): 249-260.
* MPI - Message Passing Interface Forum. (n.d.). Retrieved from https://www.mpi-forum.org/
