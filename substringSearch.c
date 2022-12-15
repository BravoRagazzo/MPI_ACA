#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

char * file_read(FILE *file,int starting_point, int dim);

struct pos_s{
  int starting_point;
  int dim;
} pos;

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size, starting_point;
  FILE *file_in;
  file_in = fopen("genoma-test.txt","r");

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

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

    int dim = SEEK_SET - SEEK_END;
    pos *p;
    p->dim = dim/size;
    int resto = dim%size;



    for(int i=1;i<size;i++) {

      p->starting_point = (dim_per_cpu*i)+resto;
      MPI_Send(p, 1, mpi_pos_type, i+1, 555, MPI_COMM_WORLD);
    }




  } else{

    char seq[DIM];

    MPI_Recv(&starting_point, 1, MPI_INT, 0, 555, MPI_COMM_WORLD, &status);

    file_read(file_in, starting_point, seq);

  }


  fclose(file_in);

  MPI_Finalize();

}

char * file_read(FILE *file,int starting_point, int dim) {

  char seq[dim];

  fseek(file,starting_point,SEEK_SET);



}
