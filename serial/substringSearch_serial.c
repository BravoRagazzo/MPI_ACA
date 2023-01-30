#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DIM 2147483640

#define NAME1 "../dataset/genoma1.txt"
#define NAME2 "../dataset/seq.txt"

#define Q 101
#define D 256

char *read_file(char file_name[]);
void rabin_karp(char *pat, char *txt);

int main(int argc, char **argv) {

  clock_t start = clock();    //----- starting time counter

  char *txt, *pat;            //----- pointers to text and pattern
  txt = read_file(NAME1);     //----- reading text

  if(argc == 1) {
    pat = read_file(NAME2);   //----- reading patter
    *(pat+strlen(pat)-1) = '\0';
  } else if(argc == 2) {
    pat = malloc((strlen(argv[1]))*sizeof(char));
    strcpy(pat,argv[1]);
  } else {
    printf("INVALID INPUT\n");
  }

  printf("----- SEQUENCE -----\n%s\n\n",pat);

  rabin_karp(pat, txt);       //----- executing the rabin karp algorithm

  free(pat);    //----- unallocating pattern array
  free(txt);    //----- unallocating text array

  clock_t end = clock();

  printf("TIME %f\n",(float)(end-start)/CLOCKS_PER_SEC);

  return 0;

}

void rabin_karp(char *pat, char *txt){

  int pat_len = strlen(pat);    //----- pattern length
  int txt_len = strlen(txt);    //----- text length
  int i,j;                      //----- counters
  int pat_hash = 0;             //----- pattern hash value
  int txt_hash = 0;             //----- text hash value
  int freq = 0;                 //----- frequency of found pattern
  int h = 1;

    for (i = 0; i < pat_len - 1; i++)
      h = (h * D) % Q;          //----- h = D^(pat_len - 1)

    for(i = 0; i < pat_len; i++){
      pat_hash = (D * pat_hash + *(pat + i)) % Q;   //----- computing the pattern hash value
      txt_hash = (D * txt_hash + *(txt + i)) % Q;   //----- computing the hash value of the first chunk of text
    }

    for(i = 0; i <= txt_len - pat_len; i++){
      if(pat_hash == txt_hash){                     //----- checking if the hashes are equal
        for(j = 0; j < pat_len; j++){               //      if yes the pattern and the chunk of text are checked
          if(*(txt + i + j) != *(pat + j))          //      character by character
            break;
        }

        if(j == pat_len){     //----- if the hash values match all the cores print the indexes and increment the frequency
          printf("Pattern found at index %d\n",i);
          freq++;
        }
      }

      if(i < txt_len - pat_len){    //---- calculating the hash value of the next text chunk adding the next character and removing the first one
        txt_hash = (D * (txt_hash - *(txt + i) * h) + *(txt + i + pat_len)) % Q;

        if(txt_hash < 0)            //----- if the hash value is negative it will be converted in positive
          txt_hash = txt_hash + Q;
      }
    }
}

char *read_file(char file_name[]) {   //----- reading the input file and allocating the arrays

  FILE *file_in;
  char *buf;
  file_in = fopen64(file_name,"rb");
  buf = (char*) malloc(DIM*sizeof(char));

  while(fread(buf,sizeof(char),DIM,file_in)>0)
    ;

  buf = realloc(buf, strlen(buf));

  fclose(file_in);

  return buf;

}
