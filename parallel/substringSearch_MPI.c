#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>
#include "header/read.h"
#include "header/rabinkarp.h"

#define NAME1 "../dataset/genoma1.txt"
#define NAME2 "../dataset/seq.txt"

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size;
  int buf_rest_array[2], M_send;

  MPI_Offset N;
  MPI_File file_in;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  MPI_File_open(MPI_COMM_SELF, NAME1, MPI_MODE_RDONLY,MPI_INFO_NULL, &file_in);
  MPI_File_get_size(file_in, &N);
  N = (N/sizeof(char)) - 1;

  if(myrank == 0) {
    int bufsize, rest;
    bufsize = N/size;
    rest = N%size;
    char *pat;
    if(argc == 1) {
      pat = read_pat(NAME2);
    } else if(argc == 2) {
      pat = malloc((strlen(argv[1]))*sizeof(char));
      strcpy(pat,argv[1]);
    } else {
      fflush(stdout);
      printf("INVALID INPUT\n");
    }

    int M = strlen(pat);
    *(pat+M) = '\0';

    int newsize = size;
    int i = 1;

    while((M > bufsize + rest + 1) && (size-i > 1)) {
      newsize = size - i;
      bufsize = N/newsize;
      rest = N%newsize;
      i++;
    }

    fflush(stdout);
    printf("NUMERO DI CPU UTILIZZATE : %d\n",newsize);

    int send[2];
    buf_rest_array[0] = bufsize;
    buf_rest_array[1] = rest;
    int m[size];

    for(int i=0; i<size; i++) {
      if(i < newsize){
        m[i]=M;
      }else{
        m[i]=0;
      }
    }

    MPI_Scatter(m, 1, MPI_INT, &M_send, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pat, M, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(buf_rest_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

    char *buf;
    buf = read_text(file_in, bufsize + rest, rest, myrank, 1, status);

    int freq,tmp;
    freq = rabin_karp(pat,buf,myrank,rest);

    free(buf);
    free(pat);
    MPI_Barrier(MPI_COMM_WORLD);

    for(i = 1; i<newsize; i++) {
      MPI_Recv(&tmp, 1, MPI_INT, i, 111, MPI_COMM_WORLD, &status);
      freq = freq + tmp;
    }

    fflush(stdout);

    printf("SEQUENCE FOUND %d TIMES\n",freq);

  } else{
    char *pat;
    int bufsize,rest,freq;

    MPI_Scatter(NULL, 1, MPI_INT, &M_send, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(M_send != 0) {
      pat = malloc(M_send*sizeof(char));
      MPI_Bcast(pat, M_send, MPI_CHAR, 0, MPI_COMM_WORLD);

      *(pat+M_send)='\0';

      MPI_Bcast(buf_rest_array, 2, MPI_INT, 0, MPI_COMM_WORLD);

      bufsize = buf_rest_array[0];
      rest = buf_rest_array[1];

      char *buf;
      buf = read_text(file_in, bufsize, rest, myrank, M_send, status);

      freq = rabin_karp(pat,buf,myrank,rest);


      free(buf);
      free(pat);

      MPI_Barrier(MPI_COMM_WORLD);
      MPI_Send(&freq, 1, MPI_INT, 0, 111, MPI_COMM_WORLD);
    }

  }

  MPI_File_close(&file_in);
  MPI_Finalize();

}
