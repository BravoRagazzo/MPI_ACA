#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DIM 5500000

#define NAME1 "b.txt"
#define NAME2 "seq.txt"

#define B 101

char *read_file(char file_name[]);
int *rabin_karp(char *txt, char*seq);
int hash(char *pattern);

int main(int argc, char **argv) {

  char *txt, *seq;
  txt = read_file(NAME1);

  //printf("----- TESTO -----\n%s\n\n",txt);

  if(argc == 1) {
    seq = read_file(NAME2);
    *(seq+strlen(seq)-1) = '\0';
  } else if(argc == 2) {
    seq = malloc((strlen(argv[1]))*sizeof(char));
    strcpy(seq,argv[1]);
  } else {
    printf("INVALID INPUT\n");
  }

printf("----- SEQUENZA -----\n%s\n\n",seq);

  int *addr;

  unsigned long timee, times = time(NULL);


  addr = rabin_karp(txt,seq);

  timee = time(NULL);


  printf("\nseq found %d times\n", addr[0]);

  for(int i=1; i<=addr[0]; i++){
    printf("n.%d at: %d\n", i, addr[i]);
  }

  printf("\n\ntime: %lu",timee-times);

  free(seq);
  free(addr);
  free(txt);

  return 0;

}

int *rabin_karp(char *txt, char*seq) {

  int *ret;
  ret = malloc(2*sizeof(int));
  char buf[strlen(seq)];
  buf[strlen(seq)] = '\0';


  strncpy(buf,txt,sizeof(buf));

  int seqhash = 0;
  for(int i=0; i<strlen(seq); i++)
    seqhash += *(seq+i)*(B^(strlen(seq)-i));
  //printf("seqhash: %d\n",seqhash);

  int j = 0;
  for(int i=0;i<strlen(txt)-strlen(buf);i++) {
    //printf("%d\n",i);
    strncpy(buf,txt+i,strlen(buf));
    //printf("\ntratto analizzato: %s\n",buf);
    if(hash(buf) == seqhash) {
      ret[j+1] = i;
      j++;
      ret = realloc(ret,(j+2)*sizeof(int));
    }
  }

  ret[0] = j;

  return ret;

}

int hash(char *pattern) {
  int hash = 0, dim = strlen(pattern);

  for(int i=0; i<dim; i++) {

    hash += *(pattern+i)*(B^(dim-i));

  }
  //printf("hash: %d\n",hash);
  return hash;

}

char *read_file(char file_name[]) {

  FILE *file_in;
  char *buf;
  file_in = fopen64(file_name,"rb");
  buf = (char*) malloc(DIM*sizeof(char));

  while(fread(buf,sizeof(char),DIM,file_in)>0)
    ;


  fclose(file_in);

  return buf;

}
