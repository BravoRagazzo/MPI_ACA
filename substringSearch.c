g#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

typedef struct{
  int starting_point;
  int dim;
}pos;

char * file_read(FILE *file, pos p);

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);


  FILE *file_in;
  int starting_point;
  file_in = fopen("genoma-test.txt","r");
  const int    nitems=2;
  int          blocklengths[2] = {1,1};
  MPI_Datatype types[2] = {MPI_INT, MPI_INT};
  MPI_Datatype mpi_pos_type;
  MPI_Aint     offsets[2];
  offsets[0] = offsetof(pos, starting_point);
  offsets[1] = offsetof(pos, dim);

  MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_pos_type);
  MPI_Type_commit(&mpi_pos_type);

  if(myrank == 0) {

    fseek(file_in, 0, SEEK_END);
    int dim = ftell(file_in);
    rewind(file_in);
    pos p;
    p.dim = dim/size;
    int resto = dim%size;

    char *seq;

    for(int i=1;i<size;i++) {

      p.starting_point = (p.dim*i)+resto;
      printf("proc%d : %d\n", i, p.starting_point);
      MPI_Send(&p, sizeof(p), mpi_pos_type, i, 555, MPI_COMM_WORLD);
    }

    p.starting_point = 0;
    p.dim = p.dim+resto;

    printf("proc%d : dim:%d start:%d\n", myrank, p.dim, p.starting_point);

    seq = file_read(file_in, p);

    printf("proc%d : %s", myrank, seq);


    free(seq);



  } else{

    char *seq;
    pos p;

    MPI_Recv(&p, sizeof(p), mpi_pos_type, 0, 555, MPI_COMM_WORLD, &status);
    printf("proc%d : dim:%d start:%d\n", myrank, p.dim, p.starting_point);

    seq = file_read(file_in, p);


    free(seq);


  }

  fclose(file_in);
  MPI_Finalize();

}

char * file_read(FILE *file, pos p) {

  char *seq;
  seq = malloc(p.dim * sizeof(char));

  fseek(file,0,p.starting_point);

  for(int i=0; i<p.dim; i++) {

    seq[i] = fgetc(file);

  }

}
