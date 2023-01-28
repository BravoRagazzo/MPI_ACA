#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>
#include "header/read.h"
#include "header/rabinkarp.h"

#define MAX_FILE_NAME 256

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank, size, pat_len, flag_send, newsize ;
  int send[3];
  char *pat;
  int *rec_freq;
  int freq = 0;

  if(argc!=3) {
    printf("INVALID INPUT\n");
    exit(0);
  }

  char pat_name[MAX_FILE_NAME] = "../dataset/";
  char text_name[MAX_FILE_NAME] = "../dataset/";
  strcat(pat_name,argv[1]);
  strcat(text_name,argv[2]);

  MPI_Offset txt_len;
  MPI_File file_in;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  int flag [size];
  MPI_File f;

  char file_name[MAX_FILE_NAME];

  MPI_File_open(MPI_COMM_SELF, text_name, MPI_MODE_RDONLY,MPI_INFO_NULL, &file_in);
  MPI_File_get_size(file_in, &txt_len);
  txt_len = (txt_len/sizeof(char))-1;

  int bufsize, rest;

  if(myrank == 0) {
    bufsize = txt_len/size;
    rest = txt_len%size;

    pat = read_pat(pat_name);

    pat_len = strlen(pat);

    newsize = size;
    int i = 0;

    while((pat_len > bufsize + rest + 1) && (size-i >= 1)) {
      newsize = size - i;
      bufsize = txt_len/newsize;
      rest = txt_len%newsize;
      i++;
    }

    printf("NUMERO DI CPU UTILIZZATE : %d\n",newsize);

    send[0] = bufsize;
    send[1] = rest;
    send[2] = pat_len;


    for(int i=0; i<size; i++) {
      if(i < newsize){
        flag[i]=1;
      }else{
        flag[i]=0;
      }
    }
    system("rm -rf ./result/*");
  }


  MPI_Bcast(send, 3, MPI_INT, 0, MPI_COMM_WORLD);
  bufsize = send[0];
  rest = send[1];
  pat_len = send[2];

  //printf("%d - %d - %d \n", bufsize, rest, pat_len);

  if(myrank!=0){
    pat = (char*)malloc((pat_len)*sizeof(char)) ;
    *(pat+pat_len) = '\0';
  }

  MPI_Bcast(pat,pat_len,MPI_CHAR,0,MPI_COMM_WORLD);
  MPI_Scatter(flag, 1, MPI_INT, &flag_send, 1, MPI_INT, 0, MPI_COMM_WORLD);


  if(flag_send == 0){
    freq = 0;
  }else{

    sprintf(file_name,"./result/rank%d.txt",myrank);
    MPI_File_open(MPI_COMM_SELF, file_name, MPI_MODE_CREATE|MPI_MODE_WRONLY,MPI_INFO_NULL, &f);
    char *buf;
    buf = read_text(file_in, (myrank==0? (bufsize+rest) : bufsize), rest, myrank, pat_len, status);
    freq = rabin_karp(pat,buf,myrank,rest,f);
    free(buf);
  }



  if(myrank==0){
    rec_freq = (int*)malloc(sizeof(int)*size);
  }

  MPI_Gather(&freq, 1, MPI_INT, rec_freq, 1, MPI_INT, 0, MPI_COMM_WORLD);


  if(myrank == 0){
    int total_freq = 0;
    for(int i = 0; i<size;i++){
      total_freq = total_freq + rec_freq[i];
    }
    printf("PATTERN FOUND %d TIMES\n",total_freq);

    FILE *file_array[newsize];
    FILE *out = fopen("./result/output.txt","wb");
    char str[256];
    for(int i = 0;i<newsize;i++){
      sprintf(str,"./result/rank%d.txt",i);
      file_array[i] = fopen(str,"rb");
      fseek(file_array[i],0,SEEK_END);
      long filesize = ftell(file_array[i]);
      rewind(file_array[i]);
      char buffer[filesize];
      fread(buffer,1,filesize,file_array[i]);
      fwrite(buffer,1,filesize,out);
      fclose(file_array[i]);
      
    }
    fclose(out);

  }

  free(pat);
  MPI_Barrier(MPI_COMM_WORLD);

  if(flag_send!=0){
      MPI_File_close(&f);
  }

  MPI_File_close(&file_in);
  MPI_Finalize();

}
