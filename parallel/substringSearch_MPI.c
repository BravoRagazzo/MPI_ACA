#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <limits.h>
#include <time.h>
#include "header/read.h"  //------- header containing reading from files functions
#include "header/rabinkarp.h" //--- header containing the actual substring search algorithm

#define MAX_FILE_NAME 256

void main(int argc, char **argv) {

  MPI_Status status;
  int myrank,size;

  int pat_len;        //---------- lenght of the searched pattern
  char *pat;          //---------- char pointer to the pattern
  int newsize;        //---------- new size (number of cores) in case of searching a pattern too long for the initial chosen size
  int send[3];        //---------- array which contains information to be sent to the processes to execute the substring search (see later)
  int flag_send;      //---------- flag to disable the cores which do not have to execute the searching
  int flag[size];     //---------- array containing all the flags
  int *rec_freq;      //---------- integer pointer for the number of found pattern per each core (used by the master to compute the total)
  int freq = 0;       //---------- number of pattern for each core
  MPI_Offset txt_len;     //------ length of the analyzed text
  MPI_File file_in;       //------ MPI_File variable for the input file
  char file_name[MAX_FILE_NAME];// char array containing the file name
  int bufsize;        //---------- dimension of the buffer containing the chunk of text to be analyzed
  int rest;           //---------- remaining characters from dividing the whole text by the number of cores

  if(argc!=3) {       //----- checking arguments
    printf("INVALID INPUT\n");
    exit(0);
  }

  char pat_name[MAX_FILE_NAME] = "../dataset/";     //----- generating the strings for header files path
  char text_name[MAX_FILE_NAME] = "../dataset/";
  strcat(pat_name,argv[1]);
  strcat(text_name,argv[2]);

  MPI_Init(&argc, &argv);                 //-----MPI initialization
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  MPI_File_open(MPI_COMM_SELF, text_name, MPI_MODE_RDONLY,MPI_INFO_NULL, &file_in);   //----- opening the input text file in read only mode
  MPI_File_get_size(file_in, &txt_len);   //----- getting the file size
  txt_len = (txt_len/sizeof(char))-1;     //----- computing the text length


	clock_t start = clock();     //----- starting timing conut

  if(myrank == 0) {            //----- master process
    bufsize = txt_len/size;    //----- computing the buffer size
    rest = txt_len%size;       //----- computing the rest

    pat = read_pat(pat_name);  //----- allocating the pattern array (see /header/read.h)
    pat_len = strlen(pat);     //----- computing pattern length

    newsize = size;            //----- newsize initialization
    int i = 0;

    while((pat_len > bufsize + rest + 1) && (size-i >= 1)) {    //----- computing the correct number of cores to be used in order to
      newsize = size - i;                                       //      be able to search the pattern
      bufsize = txt_len/newsize;
      rest = txt_len%newsize;
      i++;
    }

    send[0] = bufsize;    //----- prepearing "send" array to be sent
    send[1] = rest;
    send[2] = pat_len;


    for(int i=0; i<size; i++) {   //----- populating the flag array in order to disable (if necessary) the excess cores
      if(i < newsize){
        flag[i]=1;
      }else{
        flag[i]=0;
      }
    }

  }

  MPI_Bcast(send, 3, MPI_INT, 0, MPI_COMM_WORLD);   //----- master sends to all the processes the "send" array
  bufsize = send[0];
  rest = send[1];
  pat_len = send[2];

  if(myrank!=0){        //----- all the slaves processes allocates the memory for the pattern
    pat = (char*)malloc((pat_len)*sizeof(char)) ;
    *(pat+pat_len) = '\0';
  }

  MPI_Bcast(pat,pat_len,MPI_CHAR,0,MPI_COMM_WORLD);                             //----- master sends the pattern to all the processes
  MPI_Scatter(flag, 1, MPI_INT, &flag_send, 1, MPI_INT, 0, MPI_COMM_WORLD);     //----- master sends the flag to all the processes

  if(flag_send == 0){   //----- if the flag is 0 the core doesn't search for the pattern and
    freq = 0;           //      his frequency is set to 0
  }else{
    char *buf;          //----- else each core allocates his buffer memory for the correct chunk of text and executes the rabin karp algorithm
    buf = read_text(file_in, (myrank==0? (bufsize+rest) : bufsize), rest, myrank, pat_len, status);
    freq = rabin_karp(pat,buf,myrank,rest);   //----- rabin karp algorith returns the number of times the pattern has been found
    free(buf);          //----- after using the buffer it is unallocated
  }

  free(pat);            //----- each core unallocates the pattern

  if(myrank==0){        //----- master allocates the frequency array
    rec_freq = (int*)malloc(sizeof(int)*size);
  }

  MPI_Gather(&freq, 1, MPI_INT, rec_freq, 1, MPI_INT, 0, MPI_COMM_WORLD);   //----- all the cores send to the master the computed frequency

  if(myrank == 0){      //----- master calculates the total frequency
    int total_freq = 0;
    for(int i = 0; i<size;i++){
      total_freq = total_freq + rec_freq[i];
    }

    clock_t end = clock(); //----- ending the time counder for master which is the slower
                                                                                                  //      the number of cores used
    printf("-----\tPATTERN FOUND %d TIMES\n",total_freq);   //----- printing the total pattern frequency
    printf("-----\tCOMPUTED BY %d CPUs IN %.2fsec\n",newsize,(float)(end-start)/CLOCKS_PER_SEC);  //----- printing the total time needed and

  }

  MPI_File_close(&file_in);     //----- closing the input file
  MPI_Finalize();               //----- MPI finalizing

}
