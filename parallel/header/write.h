#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

void print_freq(int index, MPI_File result, int rank, MPI_Status status);
FILE *open_file(int rank);

void print_freq(int index, MPI_File result, int rank, MPI_Status status) {

  char print_array[256];
  sprintf(print_array,"Pattern found at index: %d\n",index);

  MPI_File_write(result, print_array, strlen(print_array), MPI_CHAR, &status);

}

FILE *open_file(int rank){
  FILE *f;
  char *file_name;
  sprintf(file_name,"./result/rank%d.txt",rank);
  f = fopen(file_name,"w");
  return f;
}
