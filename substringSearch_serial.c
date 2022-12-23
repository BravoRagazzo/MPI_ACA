#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM 200

#define NAME1 "b.txt"
#define NAME2 "seq.txt"

#define B 101

char *read_file(char file_name[]);
int *rabin_karp(char *txt, char*seq);
int hash(char *pattern);

int main(int argc, char **argv) {

  char *txt, *seq;
  txt = read_file(NAME1);

  printf("----- TESTO -----\n%s\n\n",txt);

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
  int i=0;
  addr = rabin_karp(txt,seq);
  while(*(addr+i) != 0) {
    printf("\nfound seq at: %d\n", addr[i]);
    i++;
  }

  return 0;

}

int *rabin_karp(char *txt, char*seq) {

  int *ret;
  ret = malloc(strlen(txt)*sizeof(int));
  char buf[strlen(seq)];
  buf[strlen(seq)] = '\0';

  strncpy(buf,txt,sizeof(buf));

  int seqhash = 0;
  for(int i=0; i<strlen(seq); i++)
    seqhash += *(seq+i)*(B^(strlen(seq)-i));
  printf("seqhash: %d\n",seqhash);

  int j = 0;
  for(int i=0; i<strlen(txt)-sizeof(buf);i++) {
    strncpy(buf,txt+i,sizeof(buf));
    printf("tratto analizzato: %s\n",buf);
    if(hash(buf) == seqhash) {
      ret[j] = i;
      j++;
    } else {
      continue;
    }
  }

  return ret;

}

int hash(char *pattern) {
  printf("\npat: %s\n",pattern);
  int hash = 0, dim = strlen(pattern);

  for(int i=0; i<dim; i++) {

    hash += *(pattern+i)*(B^(dim-i));

  }
  printf("hash: %d\n",hash);
  return hash;

}

char *read_file(char file_name[]) {

  FILE *file_in;
  char *buf;
  file_in = fopen(file_name,"r");
  buf = (char*) malloc(DIM*sizeof(char));

  while(fgets(buf,DIM,file_in))
    ;

  return buf;

}
