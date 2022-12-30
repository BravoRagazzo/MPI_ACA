#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DIM 2147483640

#define NAME1 "../dataset/genoma1.txt"
#define NAME2 "../dataset/seq.txt"

#define Q 101     // Big prime number
#define D 256     // Number of characters in the alphabet

char *read_file(char file_name[]);
void rabin_karp(char *pat, char *txt);

int main(int argc, char **argv) {

  char *txt, *pat;

  // Reading file to analyze
  txt = read_file(NAME1);

  // Checking if we have to read sequence from file or command line
  // Reading sequence to find in the text
  if(argc == 1) {
    pat = read_file(NAME2);
    *(pat+strlen(pat)-1) = '\0';
  } else if(argc == 2) {
    pat = malloc((strlen(argv[1]))*sizeof(char));
    strcpy(pat,argv[1]);
  } else {
    printf("INVALID INPUT\n");
  }

  printf("----- SEQUENCE -----\n%s\n\n",pat);

  // Rabin Karp algorithm to find the sequence
  rabin_karp(pat, txt);

  free(pat);
  free(txt);

  return 0;

}

void rabin_karp(char *pat, char *txt){

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
        printf("Pattern found at index %d\n",i);
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
  printf("\nPattern found %d times\n\n",freq);
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
