#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size;

  //debug
  int nrchar;

  char *buf;
  MPI_Offset filesize;
  MPI_File file_in;
  int bufsize, rest, n;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  MPI_File_open (MPI_COMM_WORLD, "b.txt", MPI_MODE_RDONLY,MPI_INFO_NULL, &file_in);
  MPI_File_get_size(file_in, &filesize);
  filesize = (filesize/sizeof(char)) - 1;
  bufsize = filesize/size;
  rest = filesize%size;

  if(myrank == 0) {

    printf("\n\nInserire lunghezza sequenza: ");
    scanf("%d",&n);

    for(int i=1; i<size; i++)
      MPI_Send(&n, 1, MPI_INT, i, 555, MPI_COMM_WORLD);

    printf("filesize : %li, rest : %d\n\n",filesize,rest);
    buf = (char *) malloc((bufsize+rest)*sizeof(char));
    MPI_File_set_view(file_in, myrank*bufsize*sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
    MPI_File_read(file_in, buf, bufsize+rest, MPI_CHAR, &status);

  }else {

    MPI_Recv(&n, 1, MPI_INT, 0, 555, MPI_COMM_WORLD, &status);

    buf = (char *) malloc((bufsize+n)*sizeof(char));
    MPI_File_set_view(file_in, myrank*bufsize*sizeof(char)+rest*sizeof(char)-n*sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
    MPI_File_read(file_in, buf, bufsize+n, MPI_CHAR, &status);


  }

  //debug
  MPI_Get_count(&status, MPI_CHAR, &nrchar);
  buf[nrchar] = (char)0;
  printf("Process %2d read %d characters: ", myrank, nrchar);
  printf("%s\n", buf);

  free(buf);
  MPI_File_close(&file_in);
  MPI_Finalize();

}