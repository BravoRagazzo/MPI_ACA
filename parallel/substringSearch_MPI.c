#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>

char *read_text(MPI_File file, int bufsize, int rest, int rank, int M, MPI_Status status);
char *read_pat(char file_name[]);
int rabin_karp(char *pat, char *txt, int rank, int rest);

#define NAME1 "./genoma1.txt"
#define NAME2 "./seq.txt"
#define Q 101     // Big prime number
#define D 256     // Number of characters in the alphabet

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size;

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

    printf("NUMERO DI CPU UTILIZZATE : %d\n",newsize);

    int send[3];
    send[1] = bufsize;
    send[2] = rest;

    for(int i=1; i<size; i++) {
      if(i < newsize){
        send[0]=M;
      }else{
        send[0]=0;
      }

      MPI_Send(send, 3, MPI_INT, i, 555, MPI_COMM_WORLD);
    }

    for(int i=1; i<newsize; i++)
      MPI_Send(pat,M, MPI_CHAR, i, 0, MPI_COMM_WORLD);


    char *buf;
    buf = read_text(file_in, bufsize + rest, rest, myrank, 1, status);

    int freq,tmp;
    freq = rabin_karp(pat,buf,myrank,rest);


    free(buf);
    free(pat);

    for(i = 1; i<newsize; i++) {
      MPI_Recv(&tmp, 1, MPI_INT, i, 111, MPI_COMM_WORLD, &status);
      freq = freq + tmp;
    }

    printf("SEQUENCE FOUND %d TIMES\n",freq);

  }else {
    char *pat;
    int M, bufsize,rest,freq;

    int recv[3];
    MPI_Recv(recv, 3, MPI_INT, 0, 555, MPI_COMM_WORLD, &status);

    M = recv[0];
    bufsize = recv[1];
    rest = recv[2];

    if(M != 0) {
      pat = malloc(M*sizeof(char));
      MPI_Recv(pat, M, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
      *(pat+M)='\0';

      char *buf;
      buf = read_text(file_in, bufsize, rest, myrank, M, status);

      freq = rabin_karp(pat,buf,myrank,rest);


      free(buf);
      free(pat);

      MPI_Send(&freq, 1, MPI_INT, 0, 111, MPI_COMM_WORLD);
    }

  }

  MPI_File_close(&file_in);
  MPI_Finalize();

}

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

int rabin_karp(char *pat, char *txt, int rank, int rest){

  int M = strlen(pat);
  int N = strlen(txt);
  int i,j;
  int txt_hash = 0, pat_hash = 0, freq = 0;
  int h = 1;

    // h = pow(D,M-1) % Q
    for (i = 0; i < M - 1; i++)
      h = (h * D) % Q;

    // Computing the hash value of pattern and first window of the text
    for(i = 0; i < M; i++){
      pat_hash = (D * pat_hash + *(pat + i)) % Q;
      txt_hash = (D * txt_hash + *(txt + i)) % Q;
    }

    for(i = 0; i <= N - M; i++){
      // Check the hash values of current window of text and pattern
      // If the hash values match then only check for characters one by one
      if(pat_hash == txt_hash){
        // Check for characters one by one
        for(j = 0; j < M; j++){
          if(*(txt + i + j) != *(pat + j))
            break;
        }

        if(j == M){
          printf(" \033[0;32m Pattern found at index %d\n\033[0m", rank == 0 ? i : i+(N-(M-1)+rest)-M+1+((rank-1)*(N-M+1)));

          freq++;
        }
      }

      // Calculate hash value for next window of text:
      // remove leading digit, add trailing digit
      if(i < N - M){
        txt_hash = (D * (txt_hash - *(txt + i) * h) + *(txt + i + M)) % Q;

        //We might get negative value of txt_hash so we convert it to positive
        if(txt_hash < 0)
          txt_hash = txt_hash + Q;
      }
    }

  return freq;
}
