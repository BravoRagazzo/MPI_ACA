#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>

char *read_text(MPI_File file, int bufsize, int rest, int rank, int M, MPI_Status status);
char *read_pat(char file_name[]);

char *read_text(MPI_File file, int bufsize, int rest, int rank, int M, MPI_Status status){

  char* txt = malloc((bufsize+M)*sizeof(char));   //----- allocating the array for the chunk of text to be analyzed
  *(txt+bufsize+M-1) = '\0';

  //----- each core access to the input file in the correct position and read a different chunk of text
  MPI_File_set_view(file, rank == 0 ? 0 : (bufsize * (rank-1)) + bufsize + rest - (M - 1), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
  MPI_File_read(file, txt, rank == 0 ? bufsize : bufsize+M-1, MPI_CHAR, &status);

  return txt;
}

char *read_pat(char file_name[]) {

  FILE *file_in;
  char *pat;
  file_in = fopen(file_name,"r");               //----- opening file containing the pattern
  pat = (char*) malloc(INT_MAX*sizeof(char));   //----- allocating the pattern

  while(fread(pat,sizeof(char),INT_MAX,file_in)>0)  //- reading the file
    ;

  pat = realloc(pat, strlen(pat));    //----- reallocating the array with the correct dimension
  *(pat+strlen(pat)-1) = '\0';

  fclose(file_in);    //----- closing the file

  return pat;

}
