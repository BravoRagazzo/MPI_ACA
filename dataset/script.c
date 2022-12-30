#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {
  FILE *fd;
  FILE *fout;
  char buf[80];
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
    if(buf[0] == '<')
      continue;

    for(int i = 0;i < 80; i++){
      if(buf[i]== 'A' || buf[i]== 'C' || buf[i]== 'T' || buf[i]== 'G' || buf[i]== 'a' || buf[i]== 'c' || buf[i]== 't' || buf[i]== 'g')
        fprintf(fout, "%c", toupper(buf[i]));
		}
	}
  fclose(fd);
	fclose(fout);
  return 0;
}
