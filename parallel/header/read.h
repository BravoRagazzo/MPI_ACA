#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>

char *read_text(MPI_File file, int bufsize, int rest, int rank, int M, MPI_Status status);
char *read_pat(char file_name[]);

#define Q 101     // Big prime number
#define D 256     // Number of characters in the alphabet


char *read_text(MPI_File file, int bufsize, int rest, int rank, int M, MPI_Status status){

  int nrchar;
  char* seq = malloc((bufsize+M)*sizeof(char));
  *(seq+bufsize+M-1) = '\0';

  MPI_File_set_view(file, rank == 0 ? 0 : (bufsize * (rank-1)) + bufsize + rest - (M - 1), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
  MPI_File_read(file, seq, bufsize+M-1, MPI_CHAR, &status);
  MPI_Get_count(&status, MPI_CHAR, &nrchar);

  // printf("Process %2d read %d characters: %s\n", rank, nrchar,seq);

  return seq;
}

char *read_pat(char file_name[]) {

  FILE *file_in;
  char *pat;
  file_in = fopen(file_name,"r");
  pat = (char*) malloc(INT_MAX*sizeof(char));

  while(fread(pat,sizeof(char),INT_MAX,file_in)>0)
    ;

  pat = realloc(pat, strlen(pat));
  *(pat+strlen(pat)-1) = '\0';

  fclose(file_in);

  return pat;

}
