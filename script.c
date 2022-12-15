#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<string.h>

int main() {
  FILE *fd;
  FILE *fout;
  char buf[79];
  char *res;
	
  fd=fopen("genoma.txt", "r");
  fout=fopen("genoma1.txt", "w");
  if( fd==NULL || fout==NULL) {
    perror("Errore in apertura del file");
    exit(1);
  }

  while(1) {
    res=fgets(buf, sizeof(buf), fd);
    if( res==NULL )
      	break;
   /* if(buf[0] == '>')
    	continue;*/
    int i;
    for(i = 0;i < 79; i++){
    		fprintf(fout, "%c", toupper(buf[i]));
		}
	}
  fclose(fd);
	fclose(fout);
  return 0;
}
