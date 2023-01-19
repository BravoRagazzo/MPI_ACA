# Substring Search with application to Genomics

## Introduction
This project implements the Rabin Karp algorithm for substring search using the Message Passing Interface (MPI) library for parallel processing. The Rabin Karp algorithm is a string matching algorithm that uses a rolling hash function to efficiently search for a given pattern within a larger text. MPI is a library that allows for the parallel execution of programs across multiple processors or machines.

## Requirements
* C compiler (e.g. GCC)
* MPI library (e.g. OpenMPI)

## Usage
* To compile the program, use the following command: `mpicc -o rabin_karp_mpi rabin_karp_mpi.c`  
* To run the program, use the following command: `mpirun -n [number of processes] ./rabin_karp_mpi [text file] {pattern}`

The program takes at most two command line arguments:

* `text file`: the file containing the text in which the pattern will be searched
* `pattern` (optional): the string that will be searched for within the text. If not present the pattern is obtained from the file `pat.txt`

The program will output the positions at which the pattern was found within the text.

## Example
`mpirun -n 4 ./rabin_karp_mpi text.txt "the"`  
This command will run the program using 4 processes and search for the substring "the" in the file "text.txt". The program will output the positions where "the" was found in the text.

## Note
The program assumes that the text file provided is small enough to be read into memory and distributed among the processes, and that the pattern provided is smaller than the text file.

## References
* Rabin, Michael O., and Richard M. Karp. "Efficient randomized pattern-matching algorithms." IBM journal of research and development 31.2 (1987): 249-260.
* MPI - Message Passing Interface Forum. (n.d.). Retrieved from https://www.mpi-forum.org/
