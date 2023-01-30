#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define Q 101     //----- big prime number
#define D 256     //----- number of characters in the alphabet

int rabin_karp(char *pat, char *txt, int rank, int rest);

int rabin_karp(char *pat, char *txt, int rank, int rest){

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
          printf("Pattern found at index %d\n", rank == 0 ? i : i+(txt_len-(pat_len-1)+rest)-pat_len+1+((rank-1)*(txt_len-pat_len+1)));
          freq++;
        }
      }

      if(i < txt_len - pat_len){    //---- calculating the hash value of the next text chunk adding the next character and removing the first one
        txt_hash = (D * (txt_hash - *(txt + i) * h) + *(txt + i + pat_len)) % Q;

        if(txt_hash < 0)            //----- if the hash value is negative it will be converted in positive
          txt_hash = txt_hash + Q;
      }
    }

  return freq;
}
