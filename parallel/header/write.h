#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

void merge_files(int newsize);

void merge_files(int newsize) {

  if(newsize==1)
    system("mv ./result/rank0.txt ./result/output.txt");
  else
  {
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

}
